/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file Worker.h
 * @brief
 *
 *
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#include "worker.h"
#include "router.h"
#include "event_server.h"
#include "common/utils/random.h"
#include "common/redis/fields/redis_session.h"
#include "common/redis/fields/redis_socket_session.h"
#include "common/redis/fields/redis_game_session.h"
#include "common/crypto/crypto.h"
#include "common/packet/packet.h"
#include "common/server/game_event.h"


// ------ Structure declaration -------

// ------ Static declaration -------
/**
 * @brief Handle a PING request from any entity.
 * @return a zframe_t containing the PONG. Never returns NULL.
 */
static zframe_t *
workerHandlePingPacket (
    void
);

/**
 * @brief Handle a client request.
 *        The first frame contains client entity, the second frame contains packet data.
 * @param self An allocated Worker structure
 * @param msg The message of the client
 * @return true on success, false otherwise
 */
static bool
workerProcessClientPacket(
    Worker *self,
    zmsg_t *msg
);

/**
 * @brief Handle a request from the public ports
 * @param loop A pointer to the reactor
 * @param worker The socket listening on the public port
 * @param self An allocated Worker structure
 * @return -2 on end of stream, -1 on error, 0 on success
*/
static int
workerHandlePublicRequest (
    zloop_t *loop,
    zsock_t *worker,
    void *_self
);

/**
 * @brief Handle a request from the private ports (coming from global server)
 * @param loop A pointer to the reactor
 * @param worker The socket listening on the public port
 * @param self An allocated Worker structure
 * @return -2 on end of stream, -1 on error, 0 on success
*/
static int
workerHandlePrivateRequest (
    zloop_t *loop,
    zsock_t *global,
    void *_self
);

/**
 * @brief Process a message from the the global server
 * @param self An allocated Worker structure
 * @param msg The message coming from the global server
 * @return true on success, false otherwise
*/
static bool
workerProcessGlobalPacket (
    Worker *self,
    zmsg_t *msg
);

/**
 * @brief Build a reply for a given packet
 * @return PacketHandlerState
 */
static bool
workerBuildReply (
    Worker *self,
    uint8_t *sessionKey,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *msg,
    zframe_t *headerAnswer
);

/**
 * @brief Read the packet and reply based on the packet handler
 * @param self A pointer to the current worker
 * @param[in] packetHandlers The packet handlers
 * @param[in] handlersCount The number of handlers in the handlers array
 * @param[in] session The game session associated with the packet
 * @param[in] packet The packet sent by the client
 * @param[in] packetSize The size of the packet
 * @param[in] packetSize The size of the packet
 * @param[out] reply The message for the reply. Each frame contains a reply to send in different packets.
 * @return PacketHandlerState
 */
static PacketHandlerState
workerHandlePacket (
    Worker *self,
    const PacketHandler *packetHandlers,
    size_t handlersCount,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    bool isCrypted,
    zmsg_t *reply
);


/**
 * @brief Build a reply based on a single request
 * @param self A pointer to the current worker
 * @param[in] session The session of the client
 * @param[in] packet The packet sent by the client
 * @param[in] packetSize The size of the packet
 * @param[out] reply The message for the reply. Each frame contains a reply to send in different packets.
 * @param[in] headerAnswer The header of the answer message
 * @param[in] isCrypted Tells if the packet was crypted
 * @return true on success, false otherwise
 */
static bool
workerProcessOneRequest (
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *msg,
    zframe_t *headerAnswer,
    bool isCrypted
);

/**
 * @brief Retrieve and check the size of a packet based on the packet header
 * @param self A pointer to the current worker
 * @param packetSize The size of the packet
 * @param packet The packet
 * @param[out] cryptHeader an allocated CryptPacketHeader
 * @return -1 on error, 0 if the packet isn't crypted, 1 if the packet is crypted
 */
static int
workerGetCryptedPacketInfo (
    Worker *self,
    size_t packetSize,
    uint8_t *packet,
    CryptPacketHeader *cryptHeader
);


// ------ Extern function implementation -------

Worker *
workerNew (
    WorkerInfo *info
) {
    Worker *self;

    if ((self = calloc(1, sizeof(Worker))) == NULL) {
        return NULL;
    }

    if (!workerInit (self, info)) {
        workerDestroy (&self);
        error("Worker failed to initialize.");
        return NULL;
    }

    return self;
}

bool
workerInit (
    Worker *self,
    WorkerInfo *workerInfo
) {
    // Make a private copy of the WorkerInfo
    if (!(workerInfoInit (
        &self->info, workerInfo->workerId, workerInfo->routerId, workerInfo->serverType,
        workerInfo->globalServerIp, workerInfo->globalServerPort,
        &workerInfo->sqlInfo, &workerInfo->redisInfo,
        workerInfo->packetHandlers, workerInfo->packetHandlersCount,
        workerInfo->routerIp, workerInfo->routerPort))
    ) {
        error("Cannot initialize worker start up info.");
        return false;
    }

    // ===================================
    //          Initialize MySQL
    // ===================================
    if (!(self->sqlConn = mySqlNew (&workerInfo->sqlInfo))) {
        error("Cannot initialize a new MySQL connection.");
        return false;
    }

    if (!(mySqlConnect (self->sqlConn))) {
        error("Cannot connect to the MySQL server.");
        return false;
    }

    // ===================================
    //     Initialize Redis connection
    // ===================================
    if (!(self->redis = redisNew (&workerInfo->redisInfo))) {
        error("Cannot initialize a new Redis connection.");
        return false;
    }

    if (!(redisConnection (self->redis))) {
        error("Cannot connect to the Redis server.");
        return false;
    }

    // ===================================
    //     Initialize ZMQ connection
    // ===================================
    if (!(self->eventServer = zsock_new (ZMQ_PUB))) {
        error("Cannot allocate event server zsock.");
        return false;
    }

    DbClientInfo dbClientInfo;
    if (!(dbClientInfoInit(&dbClientInfo, "dbSession", self->info.routerId))) {
        error("Cannot initialize dbClientInfo.");
        return false;
    }

    if (!(self->dbSession = dbClientNew(&dbClientInfo))) {
        error("Cannot allocate session db client.");
        return false;
    }

    // Initialize random seed
    self->seed = r1emuSeedRandom (self->info.routerId);

    return true;
}

bool
workerInfoInit (
    WorkerInfo *self,
    uint16_t workerId,
    uint16_t routerId,
    ServerType serverType,
    char *globalServerIp,
    int globalServerPort,
    MySQLInfo *sqlInfo,
    RedisInfo *redisInfo,
    const PacketHandler *packetHandlers,
    int packetHandlersCount,
    char *routerIp, int routerPort
) {
    self->workerId = workerId;
    self->routerId = routerId;
    self->serverType = serverType;
    self->routerIp = strdup(routerIp);
    self->routerPort = routerPort;

    if (!(self->globalServerIp = strdup(globalServerIp))) {
        error("Cannot allocated globalServerIp.");
        return false;
    }

    self->globalServerPort = globalServerPort;

    if (!(redisInfoInit(&self->redisInfo, redisInfo->hostname, redisInfo->port))) {
        error("Cannot initialize Redis Start up info.");
        return false;
    }

    if (!(mySqlInfoInit(&self->sqlInfo,
        sqlInfo->hostname, sqlInfo->user, sqlInfo->password, sqlInfo->database)))
    {
        error("Cannot initialize MySQL start up info.");
        return false;
    }

    self->packetHandlers = packetHandlers;
    self->packetHandlersCount = packetHandlersCount;

    return true;
}

static zframe_t *
workerHandlePingPacket (
    void
) {
    return zframe_new (PACKET_HEADER (ROUTER_PONG), sizeof(ROUTER_PONG));
}

static bool
workerProcessClientPacket(
    Worker *self,
    zmsg_t *msg
) {
    bool result = false;
    zframe_t *headerAnswer = NULL;

    // Read the message
    zframe_t *sessionKeyFrame = zmsg_first (msg);
    zframe_t *packetFrame = zmsg_next(msg);
    // We don't need the client packet in the reply
    zmsg_remove (msg, packetFrame);

    // Convert the frame to socketId
    uint8_t sessionKeyStr [SOCKET_SESSION_ID_SIZE];

    // Generate the socketId key
    socketSessionGenSessionKey (zframe_data(sessionKeyFrame), sessionKeyStr);

    // Request the Session
    if (!(dbClientRequestObject(self->dbSession, sessionKeyStr))) {
        error ("Cannot request a Session.");
        goto cleanup;
    }

    // Consider the message as a "normal" message by default
    headerAnswer = zframe_new (PACKET_HEADER (ROUTER_WORKER_NORMAL), sizeof(ROUTER_WORKER_NORMAL));
    zmsg_push (msg, headerAnswer);

    // === Build the message reply ===
    uint8_t *packet = zframe_data(packetFrame);
    size_t packetSize = zframe_size (packetFrame);

    if (!(workerBuildReply (self, sessionKeyStr, packet, packetSize, msg, headerAnswer))) {
        error("Cannot build a reply for the following packet :");
        buffer_print(packet, packetSize, NULL);
        goto cleanup;
    }

    result = true;

cleanup:
    // Cleanup
    zframe_destroy (&packetFrame);
    return result;
}

static bool
workerGetOrCreateSessionObject(Worker *self, uint8_t *sessionKey, DbObject **_object) {

    bool status = false;
    DbObject *object = NULL;
    Session *session = NULL;

    if (!(dbClientGetObject(self->dbSession, &object))) {
        info("Cannot get the session object, create a new one.");
    }

    if (!object) {
        info("Welcome, %s!", sessionKey);
        // Session doesn't not exist yet, create it
        if (!(session = sessionNew(self->info.routerId, sessionKey))) {
            error("Cannot allocate a new session.");
            goto cleanup;
        }

        if (!(object = dbObjectNew(sizeof(*session), session, true))) {
            error("Cannot allocate a new dbObject.");
            goto cleanup;
        }

        if (!(dbClientUpdateObject(self->dbSession, sessionKey, object))) {
            error("Cannot update the session object.");
            goto cleanup;
        }
    }

    status = true;
    *_object = object;

cleanup:
    if (!status) {
        sessionDestroy(&session);
    }

    return status;
}

static bool
workerBuildReply (
    Worker *self,
    uint8_t *sessionKey,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *msg,
    zframe_t *headerAnswer
) {
    CryptPacketHeader cryptHeader;

    bool status = false;
    size_t packetPos = 0;
    size_t packetSizeRemaining = packetSize;
    int isCrypted;

    // Get the session
    DbObject *sessionObject = NULL;
    Session *session = NULL;
    if (!(workerGetOrCreateSessionObject(self, sessionKey, &sessionObject))) {
        error("Cannot get or create the session.");
        goto cleanup;
    }
    session = sessionObject->data;

    while (packetSizeRemaining > 0)
    {
        // Get crypto packet info
        if ((isCrypted = workerGetCryptedPacketInfo (self, packetSizeRemaining, &packet[packetPos], &cryptHeader)) == -1) {
            error("Cannot get crypted packet info.");
            goto cleanup;
        }

        // Compute the subPacket size
        size_t subPacketSize = cryptHeader.plainSize;
        if (isCrypted) {
            subPacketSize += sizeof(CryptPacketHeader);
        }

        // Process the request
        if ((!(workerProcessOneRequest (self, session, &packet[packetPos], subPacketSize, msg, headerAnswer, isCrypted)))) {
            error("Cannot process properly a reply.");
            goto cleanup;
        }

        // == Iterate to the next reply ==
        packetPos += subPacketSize;
        packetSizeRemaining -= subPacketSize;
    }

    status = true;

cleanup:
    dbObjectDestroy(&sessionObject);
    return status;
}

static bool
workerProcessOneRequest (
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *msg,
    zframe_t *headerAnswer,
    bool isCrypted
) {
    bool status = false;

    // Decrypt the packet
    if (isCrypted) {
        if (!(cryptoDecryptPacket (&packet, &packetSize))) {
            error("Cannot decrypt the client packet.");
            goto cleanup;
        }
    }

    // Answer
    switch (workerHandlePacket (self, self->info.packetHandlers, self->info.packetHandlersCount, session, packet, packetSize, isCrypted, msg))
    {
        case PACKET_HANDLER_ERROR:
            zframe_reset (headerAnswer, PACKET_HEADER (ROUTER_WORKER_ERROR), sizeof(ROUTER_WORKER_ERROR));
            goto cleanup;
        break;

        case PACKET_HANDLER_OK:
        break;

        case PACKET_HANDLER_UPDATE_SESSION: {
            DbObject object;
            if (!(dbObjectInit(&object, sizeof(*session), session, true))) {
                error("Cannot initialize dbObject.");
                goto cleanup;
            }

            if (!(dbClientUpdateObject(self->dbSession, session->socket.sessionKey, &object))) {
                error("Cannot update the memory session.");
                goto cleanup;
            }

            if (!(redisUpdateSession (self->redis, session))) {
                error("Cannot update the Redis session.");
                goto cleanup;
            }
        }   break;

        case PACKET_HANDLER_DELETE_SESSION: {
            RedisSessionKey sessionKey = {
                .socketKey = {
                    .routerId = session->socket.routerId,
                    .sessionKey = session->socket.sessionKey
                }
            };
            if (!(redisFlushSession (self->redis, &sessionKey))) {
                error("Cannot delete the Session.");
                goto cleanup;
            }
        }
        break;
    }

    status = true;

cleanup:

    return status;
}

static PacketHandlerState
workerHandlePacket (
    Worker *self,
    const PacketHandler *packetHandlers,
    size_t handlersCount,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    bool isCrypted,
    zmsg_t *reply
) {
    PacketHandlerFunction handler;

    // Read the packet
    ClientPacketHeader header;
    clientPacketUnwrapHeader (&packet, &packetSize, &header, isCrypted);

    // Get the corresponding packet handler
    if (header.type > handlersCount) {
        error("Invalid packet type (%#x / %#x). Ignore request.", header.type, handlersCount);
        return PACKET_HANDLER_ERROR;
    }

    // Test if a handler is associated with the packet type requested.
    if (!(handler = packetHandlers [header.type].handler)) {
        error("Cannot find handler for the requested packet type : %s",
            (header.type < PACKET_TYPES_MAX_INDEX) ?
               packetTypeInfo.packets[header.type].string : "UNKNOWN"
        );
        return PACKET_HANDLER_ERROR;
    }

    // Call the handler
    special("Calling [%s] handler", packetTypeInfo.packets[header.type].string);
    return handler (self, session, packet, packetSize, reply);
}

static int workerGetCryptedPacketInfo (Worker *self, size_t packetSize, uint8_t *packet, CryptPacketHeader *cryptHeader) {
    memset(cryptHeader, 0, sizeof(CryptPacketHeader));

    switch (self->info.serverType)
    {
        // Zone and Barrack server encrypts their packets
        case SERVER_TYPE_ZONE:
        case SERVER_TYPE_BARRACK:
            // Check if we can read a CryptPacketHeader
            if (packetSize < sizeof(CryptPacketHeader)) {
                error("The packet received is too small to be read. Ignore request.");
                return -1;
            }

            // Unwrap the crypt packet header, and check the cryptHeader size
            cryptPacketGetHeader (packet, cryptHeader);
            if ((packetSize - sizeof(CryptPacketHeader)) < cryptHeader->plainSize) {
                error("The real packet plainSize (0x%x) is inferior to the header plainSize (0x%x). Ignore request.",
                    packetSize - sizeof(CryptPacketHeader), cryptHeader->plainSize);
                return -1;
            }

            return 1;
        break;

        // Social Server don't encrypt packet
        case SERVER_TYPE_SOCIAL:
            cryptHeader->plainSize = packetSize;
            return 0;
        break;

        default :
            error("Wrong server type ! %d", self->info.serverType);
            return -1;
        break;
    }

    return -1;
}

bool workerStart (Worker *self) {

    // ============================
    //    Initialize connections
    // ============================

    // Create and bind a publisher to send messages to the Event Server
    if (zsock_bind(self->eventServer,
                   EVENT_SERVER_SUBSCRIBER_ENDPOINT, self->info.routerId, self->info.workerId) != 0) {
        error("[routerId=%d][WorkerId=%d] cannot bind to the subscriber endpoint.",
              self->info.routerId, self->info.workerId);
        return false;
    }

    if (!(dbClientStart(self->dbSession))) {
        error("[routerId=%d][WorkerId=%d] Cannot connect to session manager.",
              self->info.routerId, self->info.workerId);
        return false;
    }

    if (zthread_new (workerMainLoop, self) != 0) {
        error("Cannot create Server worker ID = %d.", self->info.workerId);
        return false;
    }

    return true;
}

void *workerMainLoop (void *arg) {
    zsock_t *worker = NULL, *global = NULL;
    zloop_t *reactor;

    Worker *self = (Worker *) arg;

    // Create and connect a socket to the backend
    if (!(worker = zsock_new (ZMQ_REQ))
    ||  zsock_connect (worker, ROUTER_BACKEND_ENDPOINT, self->info.routerId) == -1
    ) {
        error("[routerId=%d][WorkerId=%d] cannot connect to the backend socket.", self->info.routerId, self->info.workerId);
        goto cleanup;
    }

    // Create and connect a socket to the global server
    if (!(global = zsock_new (ZMQ_REQ))
    ||  zsock_connect (global, ROUTER_GLOBAL_ENDPOINT, self->info.globalServerIp, self->info.globalServerPort) == -1
    ) {
        error("[routerId=%d][WorkerId=%d] cannot bind to the global server %s:%d.",
              self->info.routerId, self->info.workerId, self->info.globalServerIp, self->info.globalServerPort);
        goto cleanup;
    }


    // Tell to the broker we're ready for work
    zmsg_t *readyMsg = zmsg_new ();
    if (zmsg_addmem (readyMsg, PACKET_HEADER (ROUTER_WORKER_READY), sizeof(ROUTER_WORKER_READY)) == -1
    ||  zmsg_addmem (readyMsg, PACKET_HEADER (self->info.workerId), sizeof(self->info.workerId)) == -1
    ||  zmsg_send (&readyMsg, worker) == -1
    ) {
        error("[routerId=%d][WorkerId=%d] cannot send a correct ROUTER_WORKER_READY state.",
               self->info.routerId, self->info.workerId);
        goto cleanup;
    }

    if (!(reactor = zloop_new())) {
        error("Cannot allocate a new reactor.");
    }

    if (zloop_reader(reactor, worker,  workerHandlePublicRequest,  self) == -1
    ||  zloop_reader(reactor, global, workerHandlePrivateRequest, self) == -1
    ) {
        error("Cannot register the sockets with the reactor.");
    }
    if (zloop_start (reactor) != 0) {
        error("An error occurred in the reactor.");
    }

cleanup:
    // Cleanup
    zloop_destroy (&reactor);
    zsock_destroy (&worker);
    zsock_destroy (&global);

    dbg("[routerId=%d][WorkerId=%d] exits.", self->info.routerId, self->info.workerId);
    return NULL;
}

static int workerHandlePrivateRequest (zloop_t *loop, zsock_t *global, void *_self) {
    int result = 0;
    zmsg_t *msg = NULL;
    Worker *self = (Worker *) _self;

    // Process messages as they arrive
    if (!(msg = zmsg_recv(global))) {
        error("[routerId=%d][WorkerId=%d] stops working.", self->info.routerId, self->info.workerId);
        result = -1;
        goto cleanup;
    }

    if (!(workerProcessGlobalPacket (self, msg))) {
        error("[routerId=%d][WorkerId=%d] Worker cannot process the global packet.", self->info.routerId, self->info.workerId);
        result = -1;
        goto cleanup;
    }

    // Reply back to the sender
    if (zmsg_send (&msg, global) != 0) {
        warning("[routerId=%d][WorkerId=%d] failed to send a message to the backend.", self->info.routerId, self->info.workerId);
        result = -1;
        goto cleanup;
    }

cleanup:
    zmsg_destroy(&msg);
    return result;
}

static bool workerProcessGlobalPacket(Worker *self, zmsg_t *msg) {

    bool result = true;

    // Extract the request
    zframe_t *headerFrame = zmsg_pop (msg);
    zframe_t *requestAnswer = NULL;

    RouterRecvHeader header = *((RouterRecvHeader *) zframe_data(headerFrame));

    // Handle the request
    switch (header) {
        case ROUTER_PING:
            requestAnswer = workerHandlePingPacket ();
        break;

        default:
            error("[routerId=%d][WorkerId=%d] : Packet type %d not handled.", self->info.routerId, self->info.workerId, header);
            result = false;
            goto cleanup;
        break;
    }

    // Rebuild the answer message
    zmsg_add(msg, requestAnswer);

cleanup:
    // Clean up
    zframe_destroy (&headerFrame);
    return result;
}

static int workerHandlePublicRequest(zloop_t *loop, zsock_t *worker, void *_self) {
    int result = 0;
    zmsg_t *msg = NULL;
    Worker *self = (Worker *) _self;

    // Process messages as they arrive
    if (!(msg = zmsg_recv(worker))) {
        dbg("[routerId=%d][WorkerId=%d] stops working.", self->info.routerId, self->info.workerId);
        result = -1;
        goto cleanup;
    }

    // No message should be with less than 3 frames
    // The first frame is the client identity
    // The second frame is the data of the packet
    if (zmsg_size(msg) != 2) {
        error("[routerId=%d][WorkerId=%d] received a malformed message.", self->info.routerId, self->info.workerId);
        result = -1;
        goto cleanup;
    }

    if (!(workerProcessClientPacket(self, msg))) {
        error("Cannot handle correctly the client packet.");
        result = -1;
        // Don't return, we want to send back an answer so the Worker doesn't quit
    }

    // Reply back to the sender
    if (zmsg_send (&msg, worker) != 0) {
        warning("[routerId=%d][WorkerId=%d] failed to send a message to the backend.",
                self->info.routerId, self->info.workerId);
        result = -1;
        goto cleanup;
    }

cleanup:
    zmsg_destroy(&msg);

    return result;
}

bool workerDispatchEvent (Worker *self, uint8_t *emitterSk, EventType eventType, void *event, size_t eventSize)
{
    return eventServerDispatchEvent (self->eventServer, emitterSk, eventType, event, eventSize);
}

void workerInfoFree(WorkerInfo *self) {
    mySqlInfoFree (&self->sqlInfo);
    redisInfoFree (&self->redisInfo);
    free(self->globalServerIp);
}

void workerFree(Worker *self) {
    redisDestroy(&self->redis);
    mySqlDestroy(&self->sqlConn);
}

void workerDestroy(Worker **_self) {
    Worker *self = *_self;

    if (self) {
        workerFree(self);
        free(self);
    }

    *_self = NULL;
}
