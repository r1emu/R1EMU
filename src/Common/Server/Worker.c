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
 * @license <license placeholder>
 */

// ---------- Includes ------------
#include "Worker.h"
#include "Router.h"
#include "EventServer.h"
#include "Common/utils/random.h"
#include "Common/Redis/Fields/RedisSession.h"
#include "Common/Redis/Fields/RedisSocketSession.h"
#include "Common/Redis/Fields/RedisGameSession.h"
#include "Common/Crypto/Crypto.h"


// ------ Structure declaration -------

// ------ Static declaration -------
/**
 * @brief Handle a PING request from any entity.
 * @return a zframe_t containing the PONG. Never returns NULL.
 */
static zframe_t *
Worker_handlePingPacket (
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
Worker_processClientPacket (
    Worker *self,
    zmsg_t *msg
);

/**
 * @brief Handle a request from the public ports
 * @param self An allocated Worker structure
 * @param worker The socket listening on the public port
 * @return -2 on end of stream, -1 on error, 0 on success
*/
static int
Worker_handlePublicRequest (
    Worker *self,
    zsock_t *worker
);

/**
 * @brief Handle a request from the private ports (coming from global server)
 * @param self An allocated Worker structure
 * @param worker The socket listening on the public port
 * @return -2 on end of stream, -1 on error, 0 on success
*/
static int
Worker_handlePrivateRequest (
    Worker *self,
    zsock_t *global
);

/**
 * @brief Process a message from the the global server
 * @param self An allocated Worker structure
 * @param msg The message coming from the global server
 * @return true on success, false otherwise
*/
static bool
Worker_processGlobalPacket (
    Worker *self,
    zmsg_t *msg
);

/**
 * @brief Build a reply for a given packet
 * @param self A pointer to the current worker
 * @param[in] packetHandlers The packet handlers
 * @param[in] handlersCount The number of handlers in the handlers array
 * @param[in] session The game session associated with the packet
 * @param[in] packet The packet sent by the client
 * @param[in] packetSize The size of the packet
 * @param[out] reply The message for the reply. Each frame contains a reply to send in different packets.
 * @return PacketHandlerState
 */
static bool
Worker_buildReply (
    Worker *self,
    Session *session,
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
Worker_handlePacket (
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
 * @param[in] session The game session associated with the packet
 * @param[in] packet The packet sent by the client
 * @param[in] packetSize The size of the packet
 * @param[out] reply The message for the reply. Each frame contains a reply to send in different packets.
 * @param[in] headerAnswer The header of the answer message
 * @param[in] isCrypted Tells if the packet was crypted
 * @return true on success, false otherwise
 */
static bool
Worker_processOneRequest (
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
Worker_getCryptedPacketInfo (
    Worker *self,
    size_t packetSize,
    uint8_t *packet,
    CryptPacketHeader *cryptHeader
);


// ------ Extern function implementation -------

Worker *
Worker_new (
    WorkerStartupInfo *info
) {
    Worker *self;

    if ((self = calloc (1, sizeof (Worker))) == NULL) {
        return NULL;
    }

    if (!Worker_init (self, info)) {
        Worker_destroy (&self);
        error ("Worker failed to initialize.");
        return NULL;
    }

    return self;
}

bool
Worker_init (
    Worker *self,
    WorkerStartupInfo *info
) {
    // Make a private copy of the WorkerStartupInfo
    if (!(WorkerStartupInfo_init (
        &self->info, info->workerId, info->routerId, info->serverType,
        info->globalServerIp, info->globalServerPort,
        &info->sqlInfo, &info->redisInfo,
        info->packetHandlers, info->packetHandlersCount))
    ) {
        error ("Cannot initialize worker start up info.");
        return false;
    }

    // ===================================
    //          Initialize MySQL
    // ===================================
    if (!(self->sqlConn = MySQL_new (&info->sqlInfo))) {
        error ("Cannot initialize a new MySQL connection.");
        return false;
    }

    if (!(MySQL_connect (self->sqlConn))) {
        error ("Cannot connect to the MySQL server.");
        return false;
    }

    // ===================================
    //     Initialize Redis connection
    // ===================================
    if (!(self->redis = Redis_new (&info->redisInfo))) {
        error ("Cannot initialize a new Redis connection.");
        return false;
    }

    if (!(Redis_connect (self->redis))) {
        error ("Cannot connect to the Redis server.");
        return false;
    }

    // Initialize random seed
    self->seed = R1EMU_seed_random (self->info.routerId);

    return true;
}

bool
WorkerStartupInfo_init (
    WorkerStartupInfo *self,
    uint16_t workerId,
    uint16_t routerId,
    ServerType serverType,
    char *globalServerIp,
    int globalServerPort,
    MySQLStartupInfo *sqlInfo,
    RedisStartupInfo *redisInfo,
    const PacketHandler *packetHandlers,
    int packetHandlersCount
) {
    self->workerId = workerId;
    self->routerId = routerId;
    self->serverType = serverType;

    if (!(self->globalServerIp = strdup (globalServerIp))) {
        error ("Cannot allocated globalServerIp.");
        return false;
    }

    self->globalServerPort = globalServerPort;

    if (!(RedisStartupInfo_init (&self->redisInfo, redisInfo->hostname, redisInfo->port))) {
        error ("Cannot initialize Redis Start up info.");
        return false;
    }

    if (!(MySQLStartupInfo_init (&self->sqlInfo, sqlInfo->hostname, sqlInfo->login, sqlInfo->password, sqlInfo->database))) {
        error ("Cannot initialize MySQL start up info.");
        return false;
    }

    self->packetHandlers = packetHandlers;
    self->packetHandlersCount = packetHandlersCount;

    return true;
}

static zframe_t *
Worker_handlePingPacket (
    void
) {
    return zframe_new (PACKET_HEADER (ROUTER_PONG), sizeof (ROUTER_PONG));
}

static bool
Worker_processClientPacket (
    Worker *self,
    zmsg_t *msg
) {
    bool result = true;
    Session session = {{0}};
    zframe_t *headerAnswer = NULL;

    // Read the message
    zframe_t *sessionKeyFrame = zmsg_first (msg);
    zframe_t *packetFrame = zmsg_next (msg);
    // We don't need the client packet in the reply
    zmsg_remove (msg, packetFrame);

    // Convert the frame to socketId
    uint8_t sessionKeyStr [SOCKET_SESSION_ID_SIZE];

    // Generate the socketId key
    SocketSession_genSessionKey (zframe_data (sessionKeyFrame), sessionKeyStr);

    // Request the Session
    RedisSessionKey sessionKey = {
        .socketKey = {
            .routerId = self->info.routerId,
            .sessionKey = sessionKeyStr
        }
    };
    if (!(Redis_getSession (self->redis, &sessionKey, &session))) {
        error ("Cannot retrieve a Game Session.");
        result = false;
        goto cleanup;
    }

    // Consider the message as a "normal" message by default
    headerAnswer = zframe_new (PACKET_HEADER (ROUTER_WORKER_NORMAL), sizeof (ROUTER_WORKER_NORMAL));
    zmsg_push (msg, headerAnswer);

    // === Build the message reply ===
    uint8_t *packet = zframe_data (packetFrame);
    size_t packetSize = zframe_size (packetFrame);

    if (!(Worker_buildReply (self, &session, packet, packetSize, msg, headerAnswer))) {
        error ("Cannot build a reply for the following packet :");
        buffer_print (packet, packetSize, NULL);
        result = false;
        goto cleanup;
    }

cleanup:
    // Cleanup
    zframe_destroy (&packetFrame);
    return result;
}

static bool
Worker_buildReply (
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *msg,
    zframe_t *headerAnswer
) {
    CryptPacketHeader cryptHeader;

    size_t packetPos = 0;
    size_t packetSizeRemaining = packetSize;
    int isCrypted;

    while (packetSizeRemaining > 0)
    {
        // Get crypto packet info
        if ((isCrypted = Worker_getCryptedPacketInfo (self, packetSizeRemaining, &packet[packetPos], &cryptHeader)) == -1) {
            error ("Cannot get crypted packet info.");
            return false;
        }

        // Compute the subPacket size
        size_t subPacketSize = cryptHeader.plainSize;
        if (isCrypted) {
            subPacketSize += sizeof (CryptPacketHeader);
        }

        // Process the request
        if ((!(Worker_processOneRequest (self, session, &packet[packetPos], subPacketSize, msg, headerAnswer, isCrypted)))) {
            error ("Cannot process properly a reply.");
            return false;
        }

        // == Iterate to the next reply ==
        packetPos += subPacketSize;
        packetSizeRemaining -= subPacketSize;
    }

    return true;
}


static bool
Worker_processOneRequest (
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *msg,
    zframe_t *headerAnswer,
    bool isCrypted
) {
    // Decrypt the packet
    if (isCrypted) {
        if (!(Crypto_decryptPacket (&packet, &packetSize))) {
            error ("Cannot decrypt the client packet.");
            return false;
        }
    }

    // Answer
    switch (Worker_handlePacket (self, self->info.packetHandlers, self->info.packetHandlersCount, session, packet, packetSize, isCrypted, msg))
    {
        case PACKET_HANDLER_ERROR:
            zframe_reset (headerAnswer, PACKET_HEADER (ROUTER_WORKER_ERROR), sizeof (ROUTER_WORKER_ERROR));
            return false;
        break;

        case PACKET_HANDLER_OK:
        break;

        case PACKET_HANDLER_UPDATE_SESSION:
            if (!(Redis_updateSession (self->redis, session))) {
                error ("Cannot update the Session.");
                return false;
            }
        break;

        case PACKET_HANDLER_DELETE_SESSION: {
            RedisSessionKey sessionKey = {
                .socketKey = {
                    .routerId = session->socket.routerId,
                    .sessionKey = session->socket.sessionKey
                }
            };
            if (!(Redis_flushSession (self->redis, &sessionKey))) {
                error ("Cannot delete the Session.");
                return false;
            }
        }
        break;
    }

    return true;
}


static PacketHandlerState
Worker_handlePacket (
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
    ClientPacket_unwrapHeader (&packet, &packetSize, &header, isCrypted);

    // Get the corresponding packet handler
    if (header.type > handlersCount) {
        error ("Invalid packet type (%#x / %#x). Ignore request.", header.type, handlersCount);
        return PACKET_HANDLER_ERROR;
    }

    // Test if a handler is associated with the packet type requested.
    if (!(handler = packetHandlers [header.type].handler)) {
        error ("Cannot find handler for the requested packet type : %s",
            (header.type < PACKET_TYPES_MAX_INDEX) ?
               packetTypeInfo.packets[header.type].string : "UNKNOWN"
        );
        return PACKET_HANDLER_ERROR;
    }

    // Call the handler
    special ("Calling [%s] handler", packetTypeInfo.packets[header.type].string);
    return handler (self, session, packet, packetSize, reply);
}


static int
Worker_getCryptedPacketInfo (
    Worker *self,
    size_t packetSize,
    uint8_t *packet,
    CryptPacketHeader *cryptHeader
) {
    memset (cryptHeader, 0, sizeof (CryptPacketHeader));

    switch (self->info.serverType)
    {
        // Zone and Barrack server encrypts their packets
        case SERVER_TYPE_ZONE:
        case SERVER_TYPE_BARRACK:
            // Check if we can read a CryptPacketHeader
            if (packetSize < sizeof (CryptPacketHeader)) {
                error ("The packet received is too small to be read. Ignore request.");
                return -1;
            }

            // Unwrap the crypt packet header, and check the cryptHeader size
            CryptPacket_getHeader (packet, cryptHeader);
            if ((packetSize - sizeof (CryptPacketHeader)) < cryptHeader->plainSize) {
                error ("The real packet plainSize (0x%x) is inferior to the header plainSize (0x%x). Ignore request.",
                    packetSize - sizeof (CryptPacketHeader), cryptHeader->plainSize);
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
            error ("Wrong server type ! %d", self->info.serverType);
            return -1;
        break;
    }

    return -1;
}


bool
Worker_start (
    Worker *self
) {
    if (zthread_new (Worker_mainLoop, self) != 0) {
        error ("Cannot create Server worker ID = %d.", self->info.workerId);
        return false;
    }

    return true;
}


void *
Worker_mainLoop (
    void *arg
) {
    zpoller_t *poller = NULL;
    zsock_t *worker = NULL, *global = NULL;
    bool isRunning = true;

    Worker *self = (Worker *) arg;

    // ============================
    //    Initialize connections
    // ============================

    // Create and connect a socket to the backend
    if (!(worker = zsock_new (ZMQ_REQ))
    ||  zsock_connect (worker, ROUTER_BACKEND_ENDPOINT, self->info.routerId) == -1
    ) {
        error ("[routerId=%d][WorkerId=%d] cannot connect to the backend socket.", self->info.routerId, self->info.workerId);
        goto cleanup;
    }
    info ("[routerId=%d][WorkerId=%d] connected to the backend %s.",
          self->info.routerId, self->info.workerId, zsys_sprintf (ROUTER_BACKEND_ENDPOINT, self->info.routerId));

    // Create and connect a socket to the global server
    if (!(global = zsock_new (ZMQ_REQ))
    ||  zsock_connect (global, ROUTER_GLOBAL_ENDPOINT, self->info.globalServerIp, self->info.globalServerPort) == -1
    ) {
        error ("[routerId=%d][WorkerId=%d] cannot bind to the global server %s:%d.", self->info.routerId, self->info.workerId, self->info.globalServerIp, self->info.globalServerPort);
        goto cleanup;
    }
    info ("[routerId=%d][WorkerId=%d] connected to the global server %s.",
          self->info.routerId, self->info.workerId, zsys_sprintf (ROUTER_GLOBAL_ENDPOINT, self->info.globalServerIp, self->info.globalServerPort));

    // Create and bind a publisher to send messages to the Event Server
    if (!(self->eventServer = zsock_new (ZMQ_PUB))
    ||  zsock_bind (self->eventServer, EVENT_SERVER_SUBSCRIBER_ENDPOINT, self->info.routerId, self->info.workerId) == -1
    ) {
        error ("[routerId=%d][WorkerId=%d] cannot bind to the subscriber endpoint.", self->info.routerId, self->info.workerId);
        goto cleanup;
    }
    info ("[routerId=%d][WorkerId=%d] bind to the subscriber endpoint %s",
          self->info.routerId, self->info.workerId, zsys_sprintf (EVENT_SERVER_SUBSCRIBER_ENDPOINT, self->info.routerId, self->info.workerId));

    // Tell to the broker we're ready for work
    zmsg_t *readyMsg = zmsg_new ();
    if (zmsg_addmem (readyMsg, PACKET_HEADER (ROUTER_WORKER_READY), sizeof (ROUTER_WORKER_READY)) == -1
    ||  zmsg_addmem (readyMsg, PACKET_HEADER (self->info.workerId), sizeof (self->info.workerId)) == -1
    ||  zmsg_send (&readyMsg, worker) == -1
    ) {
        error ("[routerId=%d][WorkerId=%d] cannot send a correct ROUTER_WORKER_READY state.",
               self->info.routerId, self->info.workerId);
        goto cleanup;
    }

    // Define a poller with the global and the worker socket
    if (!(poller = zpoller_new (global, worker, NULL))) {
        error ("[routerId=%d][WorkerId=%d] cannot create a poller.", self->info.routerId, self->info.workerId);
        goto cleanup;
    }
    info ("[routerId=%d][WorkerId=%d] is running and waiting for messages.", self->info.routerId, self->info.workerId);

    // TODO : Refactor zpoller into zreactor ?
    while (isRunning) {

        zsock_t *actor = zpoller_wait (poller, -1);
        typedef int (*WorkerRequestHandler) (Worker *self, zsock_t *actor);
        WorkerRequestHandler handler;

        // Get the correct handler based on the actor
        if (actor == worker) {
            handler = Worker_handlePublicRequest;
        } else if (actor == global) {
            handler = Worker_handlePrivateRequest;
        }
        else {
            warning ("[routerId=%d][WorkerId=%d] received a message from an unknown actor. Maybe it is a SIGINT signal?", self->info.routerId, self->info.workerId);
            break;
        }

        switch (handler (self, actor)) {
            case -1: // ERROR
                error ("[routerId=%d][WorkerId=%d] encountered an error when handling a request.", self->info.routerId, self->info.workerId);
            break;

            case -2: // Connection stopped
                error ("[routerId=%d][WorkerId=%d] The worker has been requested to stop.", self->info.routerId, self->info.workerId);
                isRunning = false;
            break;

            case 0: /* OK */ break;
        }
    }

cleanup:
    // Cleanup
    zpoller_destroy (&poller);
    zsock_destroy (&worker);
    zsock_destroy (&global);

    dbg ("[routerId=%d][WorkerId=%d] exits.", self->info.routerId, self->info.workerId);
    return NULL;
}

static int
Worker_handlePrivateRequest (
    Worker *self,
    zsock_t *global
) {
    int result = 0;
    zmsg_t *msg = NULL;

    // Process messages as they arrive
    if (!(msg = zmsg_recv (global))) {
        error ("[routerId=%d][WorkerId=%d] stops working.", self->info.routerId, self->info.workerId);
        result = -2;
        goto cleanup;
    }

    if (!(Worker_processGlobalPacket (self, msg))) {
        error ("[routerId=%d][WorkerId=%d] Worker cannot process the global packet.", self->info.routerId, self->info.workerId);
        result = -2;
        goto cleanup;
    }

    // Reply back to the sender
    if (zmsg_send (&msg, global) != 0) {
        warning ("[routerId=%d][WorkerId=%d] failed to send a message to the backend.", self->info.routerId, self->info.workerId);
        result = -1;
        goto cleanup;
    }

cleanup:
    zmsg_destroy (&msg);
    return result;
}

static bool
Worker_processGlobalPacket (
    Worker *self,
    zmsg_t *msg
) {
    bool result = true;

    // Extract the request
    zframe_t *headerFrame = zmsg_pop (msg);
    zframe_t *requestAnswer = NULL;

    RouterRecvHeader header = *((RouterRecvHeader *) zframe_data (headerFrame));

    // Handle the request
    switch (header) {
        case ROUTER_PING:
            requestAnswer = Worker_handlePingPacket ();
        break;

        default:
            error ("[routerId=%d][WorkerId=%d] : Packet type %d not handled.", self->info.routerId, self->info.workerId, header);
            result = false;
            goto cleanup;
        break;
    }

    // Rebuild the answer message
    zmsg_add (msg, requestAnswer);

cleanup:
    // Clean up
    zframe_destroy (&headerFrame);
    return result;
}

static int
Worker_handlePublicRequest (
    Worker *self,
    zsock_t *worker
) {
    int result = 0;
    zmsg_t *msg = NULL;

    // Process messages as they arrive
    if (!(msg = zmsg_recv (worker))) {
        dbg ("[routerId=%d][WorkerId=%d] stops working.", self->info.routerId, self->info.workerId);
        result = -2;
        goto cleanup;
    }

    // No message should be with less than 3 frames
    // The first frame is the client identity
    // The second frame is the data of the packet
    if (zmsg_size (msg) != 2) {
        error ("[routerId=%d][WorkerId=%d] received a malformed message.", self->info.routerId, self->info.workerId);
        result = -1;
        goto cleanup;
    }

    if (!(Worker_processClientPacket (self, msg))) {
        error ("Cannot handle correctly the client packet.");
        result = -1;
        // Don't return, we want to send back an answer so the Worker doesn't quit
    }

    // Reply back to the sender
    if (zmsg_send (&msg, worker) != 0) {
        warning ("[routerId=%d][WorkerId=%d] failed to send a message to the backend.", self->info.routerId, self->info.workerId);
        result = -1;
        goto cleanup;
    }

cleanup:
    zmsg_destroy (&msg);
    return result;
}

bool
Worker_dispatchEvent (
    Worker *self,
    EventServerType eventType,
    void *event,
    size_t eventSize
) {
    bool result = true;
    zmsg_t *msg = NULL;

    if ((!(msg = zmsg_new ()))
    ||  zmsg_addmem (msg, PACKET_HEADER (EVENT_SERVER_EVENT), sizeof (EVENT_SERVER_EVENT)) != 0
    ||  zmsg_addmem (msg, PACKET_HEADER (eventType), sizeof (eventType)) != 0
    ||  zmsg_addmem (msg, event, eventSize) != 0
    ) {
        error ("Cannot build the event message.");
        result = false;
        goto cleanup;
    }

    if (zmsg_send (&msg, self->eventServer) != 0) {
        error ("Cannot send the event packet.");
        result = false;
        goto cleanup;
    }

cleanup:
    zmsg_destroy (&msg);
    return result;
}

void
WorkerStartupInfo_free (
    WorkerStartupInfo *self
) {
    MySQLStartupInfo_free (&self->sqlInfo);
    RedisStartupInfo_free (&self->redisInfo);
    free (self->globalServerIp);
}

void
Worker_free (
    Worker *self
) {
    Redis_destroy (&self->redis);
    MySQL_destroy (&self->sqlConn);
}

void
Worker_destroy (
    Worker **_self
) {
    Worker *self = *_self;

    if (self) {
        Worker_free (self);
        free (self);
    }

    *_self = NULL;
}
