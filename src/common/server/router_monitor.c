/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file RouterMonitor.c
 * @brief
 *
 *
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

// ------ Includes -------
#include "router_monitor.h"
#include "router.h"
#include "common/session/session.h"
#include "common/redis/fields/redis_session.h"
#include "common/packet/packet.h"

// ------ Structure declaration -------
/**
 * @brief RouterMonitor contains needed information about client connection
 */
struct RouterMonitor {
    RouterMonitorStartupInfo info;

    /** FD => Socket Id hashtable */
    zhash_t *connected;

    /** Database connection */
    Redis *redis;
    MySQL *sql;
};

// ------ Static declaration -------
/**
 * @brief Router Monitor handler
 * @param loop The reactor handler
 * @param frontend The monitor socket
 * @param self The RouterMonitor
 * @return 0 on success, -1 on error
 */
static int RouterMonitor_monitor(zloop_t *loop, zsock_t *monitor, void *_self);

/**
 * @brief Router Subscriber handler
 * @param loop The reactor handler
 * @param frontend The monitor socket
 * @param self The RouterMonitor
 * @return 0 on success, -1 on error
 */
static int RouterMonitor_subscribe(zloop_t *loop, zsock_t *monitor, void *_self);

// ------ Extern functions implementation -------
RouterMonitor *routerMonitorNew(RouterMonitorStartupInfo *info) {

    RouterMonitor *self;

    if ((self = calloc(1, sizeof(RouterMonitor))) == NULL) {
        return NULL;
    }

    if (!routerMonitorInit (self, info)) {
        routerMonitorDestroy (&self);
        error("RouterMonitor failed to initialize.");
        return NULL;
    }

    return self;
}

bool routerMonitorInit (RouterMonitor *self, RouterMonitorStartupInfo *info) {

    routerMonitorStartupInfoInit (&self->info, info->frontend, info->routerId, &info->redisInfo, &info->sqlInfo);
    routerMonitorStartupInfoDestroy (&info);

    // Allocate the connected clients hashtable
    if (!(self->connected = zhash_new())) {
        error("Cannot allocate a new connected clients hashtable.");
        return false;
    }

    if (!(self->redis = redisNew (&self->info.redisInfo))) {
        error("Cannot initialize the Redis connection.");
        return false;
    }

    if (!(self->sql = mySqlNew (&self->info.sqlInfo))) {
        error("Cannot initialize the MySQL connection.");
        return false;
    }

    return true;
}

RouterMonitorStartupInfo *routerMonitorStartupInfoNew(
    zsock_t *frontend,
    uint16_t routerId,
    RedisStartupInfo *redisInfo,
    MySQLStartupInfo *sqlInfo)
{
    RouterMonitorStartupInfo *self;

    if ((self = calloc(1, sizeof(RouterMonitorStartupInfo))) == NULL) {
        return NULL;
    }

    if (!routerMonitorStartupInfoInit (self, frontend, routerId, redisInfo, sqlInfo)) {
        routerMonitorStartupInfoDestroy (&self);
        error("RouterMonitorStartupInfo failed to initialize.");
        return NULL;
    }

    return self;
}

bool routerMonitorStartupInfoInit (
    RouterMonitorStartupInfo *self,
    zsock_t *frontend,
    uint16_t routerId,
    RedisStartupInfo *redisInfo,
    MySQLStartupInfo *sqlInfo)
{
    self->frontend = frontend;
    self->routerId = routerId;

    if (!(redisStartupInfoInit(&self->redisInfo, redisInfo->hostname, redisInfo->port))) {
        error("Cannot initialize Redis Start up info.");
        return false;
    }

    if (!(mySqlStartupInfoInit(&self->sqlInfo,
        sqlInfo->hostname, sqlInfo->user, sqlInfo->password, sqlInfo->database))) {
        error("Cannot initialize MySQL start up info.");
        return false;
    }

    return true;
}


static int
RouterMonitor_monitor (
    zloop_t *loop,
    zsock_t *monitor,
    void *_self
) {
    int result = 0;
    RouterMonitor *self = _self;
    zmsg_t *msg;
    zframe_t *header;

    // Receive the message from the Router
    if (!(msg = zmsg_recv(monitor))) {
        // Interrupt
        result = 0;
        goto cleanup;
    }

    // Get the frame header of the message
    if (!(header = zmsg_first (msg))) {
        error("Frame header cannot be retrieved.");
        result = -1;
        goto cleanup;
    }

    if (zframe_streq (header, "ACCEPTED")) {
        // A client just connected to the Router.

        // Get the socket file descriptor
        zframe_t *fdFrame = zmsg_next(msg);
        uint64_t fdClient = strtoull(zframe_data(fdFrame), NULL, 10);

        // Check if this file descriptor is still used
        uint8_t fdClientKey [ROUTER_MONITOR_FDKEY_SIZE];
        routerMonitorGenKey (fdClient, fdClientKey);

        zframe_t *clientFrame;
        // Check if it already exists in the table
        if ((clientFrame = zhash_lookup(self->connected, fdClientKey)) != NULL) {
            uint8_t sessionKey [ROUTER_MONITOR_FDKEY_SIZE];
            socketSessionGenSessionKey (zframe_data(clientFrame), sessionKey);
            error("The client FD=%d just connected, but another client has still this FD (previously : %s)",
                   fdClient, sessionKey);
            // TODO : Decide what to do in this case
            result = 0;
            goto cleanup;
        }
    }

    else if (zframe_streq (header, "DISCONNECTED")) {
        // A client just disconnected from the Router.

        // Get the socket file descriptor
        zframe_t *fdFrame = zmsg_next(msg);
        uint64_t fdClient = strtoull(zframe_data(fdFrame), NULL, 10);

        // Check if this file descriptor is already used
        uint8_t fdClientKey [ROUTER_MONITOR_FDKEY_SIZE];
        routerMonitorGenKey (fdClient, fdClientKey);

        zframe_t *clientFrame;
        if ((clientFrame = zhash_lookup(self->connected, fdClientKey)) == NULL) {
            // The client just disconnected, but no client has been registred using this fd
            // It happens when the client connects but send no data to the server
            // TODO : Decide what to do in this case, probably nothing
            warning("Cannot find the clientFrame when disconnecting.");
            dbg("DISCONNECTED : Key = %s", fdClientKey);
        }
        else {
            // Everything is okay here, disconnect gracefully the client
            uint8_t sessionKeyStr [SOCKET_SESSION_ID_SIZE];
            socketSessionGenSessionKey (zframe_data(clientFrame), sessionKeyStr);

            // Flush the session here
            RedisSessionKey sessionKey = {
                .socketKey = {
                    .routerId = self->info.routerId,
                    .sessionKey = sessionKeyStr
                }
            };
            redisFlushSession (self->redis, &sessionKey);

            // Remove the key from the "connected" hashtable
            zhash_delete (self->connected, fdClientKey);
            zframe_destroy (&clientFrame);

            info("%s session successfully flushed !", sessionKeyStr);
        }
    }

cleanup:
    // Cleanup
    zmsg_destroy(&msg);

    return result;
}


static int
RouterMonitor_subscribe (
    zloop_t *loop,
    zsock_t *monitor,
    void *_self
) {
    RouterMonitor *self = _self;
    zmsg_t *msg;
    zframe_t *header;
    int result = 0;

    // Receive the message from the Router
    if (!(msg = zmsg_recv(monitor))) {
        // Interrupt
        result = 0;
        goto cleanup;
    }

    // Get the frame header of the message
    if (!(header = zmsg_first (msg))) {
        error("Frame header cannot be retrieved.");
        result = -1;
        goto cleanup;
    }

    // Convert the frame to a RouterMonitorHeader
    RouterMonitorHeader packetHeader = *((RouterMonitorHeader *) zframe_data(header));

    switch (packetHeader) {

        case ROUTER_MONITOR_ADD_FD: {
            zframe_t *fdFrame = zmsg_next(msg);

            // Get the socket file descriptor
            uint64_t fdClient = *((uint64_t *) (zframe_data(fdFrame)));

            // Check if this file descriptor is still used
            uint8_t fdClientKey [ROUTER_MONITOR_FDKEY_SIZE];
            routerMonitorGenKey (fdClient, fdClientKey);

            zframe_t *clientFrame;
            if ((clientFrame = zhash_lookup(self->connected, fdClientKey)) == NULL) {
                // The client just sent its first message, add the identity frame to the hashtable
                clientFrame = zframe_dup (zmsg_next(msg));
                zhash_insert(self->connected, fdClientKey, clientFrame);
            } else {
                // Check if the fd <=> identity association is always the same
                if (!zframe_eq (clientFrame, zmsg_next(msg))) {
                    // Huho.
                    // TODO : Decide what to do
                    error("==================================================================");
                    error("The registred fd changed identity. That's really not a good thing.");
                    error("==================================================================");
                    result = -1;
                    goto cleanup;
                } else {
                    // It's alright, the client kept the same identity
                }
            }
        } break;

        default:
            warning("Server subscriber received an unknown header : %x", packetHeader);
        break;
    }

cleanup:
    zmsg_destroy(&msg);

    return result;
}


void
routerMonitorStart (
    zsock_t *pipe,
    void *info
) {
    RouterMonitor self = {{0}};
    routerMonitorInit (&self, info);

    zactor_t *servermon = NULL;
    zsock_t *requests = NULL;
    zloop_t *reactor = NULL;

    // Connect to the Redis database
    if (!(redisConnection (self.redis))) {
        error("Cannot connect to Redis.");
        goto cleanup;
    }

    // Connect to the MySQL database
    if (!(mySqlConnect (self.sql))) {
        error("Cannot connect to MySQL.");
        goto cleanup;
    }

    // Set up the Server Monitor Actor
    if (!(servermon = zactor_new (zmonitor, self.info.frontend))) {
        error("Cannot allocate a new server monitor actor.");
        goto cleanup;
    }
    zstr_sendx (servermon, "LISTEN", "ACCEPTED", "DISCONNECTED", NULL);
    zstr_sendx (servermon, "START", NULL);
    zsock_wait (servermon);

    // Set up the Server Monitor Subscriber that will receive request from the Router
    if (!(requests = zsock_new (ZMQ_SUB))) {
        error("Cannot allocate a request subscriber socket.");
        goto cleanup;
    }

    if (zsock_connect (requests, ROUTER_MONITOR_SUBSCRIBER_ENDPOINT, self.info.routerId) != 0) {
        error("Failed to connect to the monitor subscriber endpoint %d.", self.info.routerId);
        goto cleanup;
    }
    info("Monitor Subscriber connected to %s", zsys_sprintf(ROUTER_MONITOR_SUBSCRIBER_ENDPOINT, self.info.routerId));
    zsock_set_subscribe(requests, "");

    // ====================================
    //   Prepare a reactor and fire it up
    // ====================================
    if (!(reactor = zloop_new())) {
        error("Cannot allocate a new reactor.");
        goto cleanup;
    }

    // Attach a callback to frontend and backend sockets
    if (zloop_reader(reactor, (zsock_t *) servermon, RouterMonitor_monitor, &self) == -1
    ||  zloop_reader(reactor, requests, RouterMonitor_subscribe, &self) == -1
    ) {
        error("Cannot register the sockets with the reactor.");
        goto cleanup;
    }

    info("Router Monitor is ready and running.");
    // Signal to the parent thread that the monitor is ready
    zsock_signal (pipe, 0);
    zframe_send ((zframe_t *[]) {zframe_new (PACKET_HEADER (ROUTER_MONITOR_READY), sizeof(ROUTER_MONITOR_READY))}, pipe, 0);

    if (zloop_start (reactor) != 0) {
        error("An error occurred in the reactor.");
        goto cleanup;
    }

cleanup:
    routerMonitorFree (&self);
    zactor_destroy (&servermon);
    zsock_destroy (&requests);
    zloop_destroy (&reactor);
}

void
routerMonitorGenKey (
    uint64_t fd,
    uint8_t fdKey[ROUTER_MONITOR_FDKEY_SIZE]
) {
    // Format the fdKey from the fd
    snprintf (fdKey, ROUTER_MONITOR_FDKEY_SIZE, "%I64d", fd);
}

void
routerMonitorFree (
    RouterMonitor *self
) {
    zhash_destroy (&self->connected);
    redisDestroy (&self->redis);
    mySqlDestroy (&self->sql);
}

void
routerMonitorDestroy (
    RouterMonitor **_self
) {
    RouterMonitor *self = *_self;

    if (self) {
        routerMonitorFree (self);
        free(self);
    }

    *_self = NULL;
}

void
RouterMonitorStartupInfo_free(
    RouterMonitorStartupInfo *self
) {
    redisStartupInfoFree (&self->redisInfo);
    mySqlStartupInfoFree (&self->sqlInfo);
}

void
routerMonitorStartupInfoDestroy (
    RouterMonitorStartupInfo **_self
) {
    RouterMonitorStartupInfo *self = *_self;

    if (self) {
        RouterMonitorStartupInfo_free(self);
        free(self);
    }

    *_self = NULL;
}
