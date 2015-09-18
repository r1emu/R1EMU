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

/**
 * @brief SessionManager contains a Db of Session *
 */
struct SessionManager
{
    Db *server;
    DbClient *client;
};

SessionManager *sessionManagerNew(DbInfo *startInfo) {
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

bool sessionManagerInit(SessionManager *self, DbInfo *startInfo) {
    memset(self, 0, sizeof(SessionManager));

    if (!(self->server = dbNew(startInfo))) {
        error("Cannot allocate a new db.");
        return false;
    }

    return true;
}

bool sessionManagerStart(SessionManager *self) {

    if (!(dbStart(self->server))) {
        error("Cannot start db.");
        return false;
    }

    return true;
}

bool sessionManagerStoreSession(SessionManager *self, uint8_t *sessionKey, Session *session) {

    return true;
}

bool sessionManagerLoadSession(SessionManager *self, uint8_t *sessionKey, Session *session) {
    return true;
}

void sessionManagerFree(SessionManager *self) {
}

void sessionManagerDestroy(SessionManager **_self) {
    SessionManager *self = *_self;

    if (_self && self) {
        sessionManagerFree(self);
        *_self = NULL;
    }
}
