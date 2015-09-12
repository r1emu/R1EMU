/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#include "session_manager.h"
#include "common/packet/packet.h"

/**
 * @brief SessionManager contains a hashtable of all sessions in the current zone
 */
struct SessionManager
{
    /** Info for starting session manager */
    SessionManagerStartupInfo info;

    /** Session hashtable<Session *> */
    zhash_t *sessions;

    /** SessionManager Endpoint accepting Workers requests */
    zsock_t *endpoint;
};

SessionManager *sessionManagerNew(SessionManagerStartupInfo *startInfo) {
    SessionManager *self;

    if ((self = malloc(sizeof(SessionManager))) == NULL) {
        return NULL;
    }

    if (!sessionManagerInit(self, startInfo)) {
        sessionManagerDestroy(&self);
        error("SessionManager failed to initialize.");
        return NULL;
    }

    return self;
}

bool sessionManagerInit(SessionManager *self, SessionManagerStartupInfo *startInfo) {
    memset(self, 0, sizeof(SessionManager));

    memcpy(&self->info, startInfo, sizeof(self->info));

    if (!(self->sessions = zhash_new ())) {
        error("Cannot allocate a new hashtable for sessions.");
        return false;
    }

    if (!(self->endpoint = zsock_new (ZMQ_REP))) {
        error("Cannot allocate a new endpoint.");
        return false;
    }

    return true;
}

void *sessionManagerMainLoop (void *arg) {
    SessionManager *self = (typeof(self)) arg;

    zmsg_t *msg = NULL;
    zframe_t *packetTypeFrame = NULL;
    zframe_t *sessionKeyFrame = NULL;

    while (true) {
        msg = zmsg_recv(self->endpoint);

        if (!msg) {
            error("Cannot receive a message.");
            break;
        }

        // read packet type
        if (!(packetTypeFrame = zmsg_pop(msg))) {
            error("Cannot read the packet type frame.");
            break;
        }
        SessionManagerPacketType packetType = *((SessionManagerPacketType *) zframe_data(packetTypeFrame));

        // read session key
        if (!(sessionKeyFrame = zmsg_pop(msg))) {
            error("Cannot read the session key frame.");
            break;
        }
        uint8_t *sessionKey = (uint8_t *) zframe_data(sessionKeyFrame);

        // handle packet
        switch (packetType)
        {
            case SESSION_MANAGER_LOAD_SESSION: {
                // read the session from the hashtable
                Session session;
                SessionManagerStatus status = SESSION_MANAGER_STATUS_SUCCESS;

                if (!(sessionManagerLoadSession(self, sessionKey, &session))) {
                    error("Cannot load the session.");
                    status = SESSION_MANAGER_CANNOT_LOAD;
                }

                // header : status
                if ((zmsg_addmem(msg, PACKET_HEADER(status), sizeof(status)) != 0)) {
                    error("Cannot add the packet header to the msg.");
                    break;
                }

                // add data only in case of success
                if (status == SESSION_MANAGER_STATUS_SUCCESS) {
                    // Build the answer packet
                    if ((zmsg_addmem(msg, &session, sizeof(session)) != 0)) {
                        error("Cannot add the session to the msg.");
                        status = SESSION_MANAGER_CANNOT_LOAD;
                    }
                }

                // send packet
                if (zmsg_send(&msg, self->endpoint) != 0) {
                    error("Cannot send back the session.");
                    break;
                }

            }   break;

            case SESSION_MANAGER_STORE_SESSION: {
                // save the session
                SessionManagerStatus status = SESSION_MANAGER_STATUS_SUCCESS;
                zframe_t *sessionFrame = zmsg_pop(msg);
                Session *session = (Session *) zframe_data(sessionFrame);
                // TODO : fix memleak

                if (!session) {
                    error("Cannot retrieve the session.");
                    status = SESSION_MANAGER_CANNOT_STORE;
                }

                // header : status
                if ((zmsg_addmem(msg, PACKET_HEADER(status), sizeof(PACKET_HEADER(status))) != 0)) {
                    error("Cannot add the packet header to the msg.");
                    break;
                }

                // store data only in case of success
                if (status == SESSION_MANAGER_STATUS_SUCCESS) {
                    if (!(sessionManagerStoreSession(self, sessionKey, session))) {
                        error("Cannot store the session.");
                        break;
                    }
                }

                // send packet
                if (zmsg_send(&msg, self->endpoint) != 0) {
                    error("Cannot send the reply packet.");
                    break;
                }

            }   break;
        }

        zframe_destroy(&packetTypeFrame);
        zframe_destroy(&sessionKeyFrame);
    }

    info("SessionManager %d stopped working.", self->info.routerId);
    return NULL;
}

bool sessionManagerStart(SessionManager *self) {

    if ((zsock_bind(self->endpoint, SESSION_MANAGER_ENDPOINT, self->info.routerId)) != 0) {
        error("Cannot bind the session manager for routerId = %d", self->info.routerId);
        return false;
    }

    zthread_new(sessionManagerMainLoop, self);

    return true;
}

bool sessionManagerStoreSession(SessionManager *self, uint8_t *sessionKey, Session *session) {
    Session *hashtableSession = NULL;

    if (!(hashtableSession = zhash_lookup(self->sessions, sessionKey))) {
        error("Cannot store session : Session '%s' does not exist.", sessionKey);
        return false;
    }
    else {
        // Session already exists : update it
        memcpy(hashtableSession, session, sizeof(*hashtableSession));
    }

    return true;
}

bool sessionManagerLoadSession(SessionManager *self, uint8_t *sessionKey, Session *session) {
    Session *hashtableSession = NULL;

    if (!(hashtableSession = zhash_lookup(self->sessions, sessionKey))) {

        // Session doesn't already exist : create it
        if (!(hashtableSession = malloc(sizeof(Session)))) {
            error("Cannot allocate a new Session for session hashtable.");
            return false;
        }

        if (!(sessionInit(hashtableSession, self->info.routerId, sessionKey))) {
            error("Cannot initialize a new session.");
            return false;
        }

        if (zhash_insert(self->sessions, sessionKey, hashtableSession) != 0) {
            error("Cannot insert the new session in the hashtable.");
            return false;
        }
    }

    memcpy(session, hashtableSession, sizeof(*session));

    return true;
}

void sessionManagerFree(SessionManager *self) {
    zhash_destroy(&self->sessions);
    free(self);
}

void sessionManagerDestroy(SessionManager **_self) {
    SessionManager *self = *_self;

    if (_self && self) {
        sessionManagerFree(self);
        *_self = NULL;
    }
}
