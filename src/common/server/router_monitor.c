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
#include "common/server/event_server.h"

// ------ Structure declaration -------
/**
 * @brief RouterMonitor contains needed information about client connection
 */
struct RouterMonitor {
    RouterMonitorInfo info;

    /** FD => Socket Id hashtable */
    zhash_t *connected;

    /** The EventServer socket */
    zsock_t *eventServer;

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
static int routerMonitorProcess(zloop_t *loop, zsock_t *monitor, void *_self);

/**
 * @brief Router Subscriber handler
 * @param loop The reactor handler
 * @param frontend The monitor socket
 * @param self The RouterMonitor
 * @return 0 on success, -1 on error
 */
static int routerMonitorSubscribe(zloop_t *loop, zsock_t *monitor, void *_self);

/**
 * @brief Disconnect a client successfully
 **/
static bool routerMonitorDisconnectClient(RouterMonitor *self, uint8_t *fdClientKey, uint8_t *sessionKeyStr);


// ------ Extern functions implementation -------
RouterMonitor *routerMonitorNew(RouterMonitorInfo *info) {

    RouterMonitor *self;

    if ((self = malloc(sizeof(RouterMonitor))) == NULL) {
        return NULL;
    }

    if (!routerMonitorInit (self, info)) {
        routerMonitorDestroy (&self);
        error("RouterMonitor failed to initialize.");
        return NULL;
    }

    return self;
}

bool routerMonitorInit (RouterMonitor *self, RouterMonitorInfo *info) {

    memset(self, 0, sizeof(*self));

    routerMonitorInfoInit (&self->info,
        info->frontend,
        info->routerId,
        &info->redisInfo,
        &info->sqlInfo,
        info->disconnectHandler
    );
    routerMonitorInfoDestroy (&info);

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

    // Create and bind a publisher to send messages to the Event Server
    if (!(self->eventServer = zsock_new (ZMQ_PUB))
    ||  zsock_bind(self->eventServer, EVENT_SERVER_MONITOR_ENDPOINT, self->info.routerId) == -1
    ) {
        error("[routerId=%d] cannot bind to the monitor subscriber endpoint.", self->info.routerId);
        return false;
    }
    info ("RouterMonitor binded %s.", zsys_sprintf(EVENT_SERVER_MONITOR_ENDPOINT, self->info.routerId));

    return true;
}

RouterMonitorInfo *routerMonitorInfoNew(
    zsock_t *frontend,
    RouterId_t routerId,
    RedisInfo *redisInfo,
    MySQLInfo *sqlInfo,
    DisconnectEventHandler disconnectHandler)
{
    RouterMonitorInfo *self;

    if ((self = calloc(1, sizeof(RouterMonitorInfo))) == NULL) {
        return NULL;
    }

    if (!routerMonitorInfoInit (self, frontend, routerId, redisInfo, sqlInfo, disconnectHandler)) {
        routerMonitorInfoDestroy (&self);
        error("RouterMonitorInfo failed to initialize.");
        return NULL;
    }

    return self;
}

bool routerMonitorInfoInit (
    RouterMonitorInfo *self,
    zsock_t *frontend,
    RouterId_t routerId,
    RedisInfo *redisInfo,
    MySQLInfo *sqlInfo,
    DisconnectEventHandler disconnectHandler)
{
    self->frontend = frontend;
    self->routerId = routerId;
    self->disconnectHandler = disconnectHandler;

    if (!(redisInfoInit(&self->redisInfo, redisInfo->hostname, redisInfo->port))) {
        error("Cannot initialize Redis Start up info.");
        return false;
    }

    if (!(mySqlInfoInit(&self->sqlInfo,
        sqlInfo->hostname, sqlInfo->user, sqlInfo->password, sqlInfo->database))) {
        error("Cannot initialize MySQL start up info.");
        return false;
    }

    return true;
}


static int
routerMonitorProcess (
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
    if (!(header = zmsg_first(msg))) {
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
            // everything is okay here, disconnect gracefully the client

            // generate a session key string
            uint8_t sessionKeyStr [SOCKET_SESSION_ID_SIZE];
            socketSessionGenSessionKey (zframe_data(clientFrame), sessionKeyStr);

            // call the custom disconnect handler
            if (!self->info.disconnectHandler) {
                warning("No custom disconnection server handler has been registred.");
            } else {
                if (!(self->info.disconnectHandler(
                        self->eventServer,
                        self->redis,
                        self->sql,
                        self->info.routerId,
                        sessionKeyStr)))
                {
                    warning("Custom disconnect handler failed.");
                }
            }
            // call router monitor disconnect handler
            routerMonitorDisconnectClient(self, fdClientKey, sessionKeyStr);
            zframe_destroy(&clientFrame);
            info("%s session successfully flushed !", sessionKeyStr);
        }
    }

cleanup:
    // Cleanup
    zmsg_destroy(&msg);

    return result;
}

static bool routerMonitorDisconnectClient(RouterMonitor *self, uint8_t *fdClientKey, uint8_t *sessionKeyStr) {

    // Remove the key from the "connected" hashtable
    zhash_delete (self->connected, fdClientKey);

    return true;
}

static int
routerMonitorSubscribe (
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
    if (!(header = zmsg_first(msg))) {
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
                if (zhash_insert(self->connected, fdClientKey, clientFrame) != 0) {
                    error ("Cannot insert client frame in client connected hashtable.");
                    result = -1;
                    goto cleanup;
                }
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
    RouterMonitor self;
    routerMonitorInit(&self, info);

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
    if (!(servermon = zactor_new(zmonitor, self.info.frontend))) {
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
    if (zloop_reader(reactor, (zsock_t *) servermon, routerMonitorProcess, &self) == -1
    ||  zloop_reader(reactor, requests, routerMonitorSubscribe, &self) == -1
    ) {
        error("Cannot register the sockets with the reactor.");
        goto cleanup;
    }

    info("Router Monitor is ready and running.");
    // Signal to the parent thread that the monitor is ready
    zsock_signal (pipe, 0);
    zframe_send ((zframe_t *[]) {zframe_new(PACKET_HEADER (ROUTER_MONITOR_READY), sizeof(ROUTER_MONITOR_READY))}, pipe, 0);

    if (zloop_start(reactor) != 0) {
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
    redisDestroy(&self->redis);
    mySqlDestroy(&self->sql);
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
RouterMonitorInfo_free(
    RouterMonitorInfo *self
) {
    redisInfoFree(&self->redisInfo);
    mySqlInfoFree(&self->sqlInfo);
}

void
routerMonitorInfoDestroy (
    RouterMonitorInfo **_self
) {
    RouterMonitorInfo *self = *_self;

    if (self) {
        RouterMonitorInfo_free(self);
        free(self);
    }

    *_self = NULL;
}
