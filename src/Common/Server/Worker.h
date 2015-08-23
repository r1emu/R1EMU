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
 * @brief Worker is the component in the network architecture that processes the packet received from the Router.
 *
 * Here are the general steps that a worker always do :
 * - It receives a request from the Router
 * - It extracts the entity ID from the packet
 * - It searches for the session associated with the entity ID in the Redis server
 * - If it exists, return the session associated with the entity ID
 * - If it doesn't, create the session
 *
 * @license <license placeholder>
 */

#pragma once

// ---------- Includes ------------
#include "R1EMU.h"
#include "EventServer.h"
#include "Common/MySQL/MySQL.h"
#include "Common/Redis/Redis.h"
#include "Common/Session/Session.h"

// ---------- Defines -------------

// ------ Structure declaration -------
typedef struct _PacketHandler PacketHandler;
typedef struct _WorkerStartupInfo WorkerStartupInfo;
typedef struct _Worker Worker;
typedef enum _PacketHandlerState PacketHandlerState;

/**
 * @brief PacketHandlerFunction is the generic function prototype that a Worker is going to call when it receives a packet.
 * It takes the packet, calls packet builders based on the packet type, and reply to it using the \reply argument.
 */
typedef PacketHandlerState (*PacketHandlerFunction) (
    /** A pointer to the current Worker */
    Worker *self,
    /** Session of the current player */
    Session *session,
    /** Packet sent by the client */
    uint8_t *packet,
    /** Size of the packet sent by the client */
    size_t packetSize,
    /** Reply buffer. If you need to send multiple replies, add multiple frames to this zmsg_t */
    zmsg_t *reply
);

/**
 * @brief WorkerStartupInfo contains all the information needed for a worker to start.
 */
struct _WorkerStartupInfo
{
    /** The worker ID */
    uint16_t workerId;

    /** The Server ID having authority on this worker */
    uint32_t routerId;

    /** The server type of the current Worker */
    ServerType serverType;

    /** IP of the global server */
    char *globalServerIp;

    /** Private port with the global server */
    int globalServerPort;

    /** MySQL startup information */
    MySQLStartupInfo sqlInfo;

    /** Redis startup information */
    RedisStartupInfo redisInfo;

    /** Array of packet handlers */
    const PacketHandler *packetHandlers;

    /** Packet handlers entries count */
    int packetHandlersCount;

};

/**
 * @brief Worker is the main component of the network architecture, as it processes the packets received
 * It connects to the database and fullfil requests based on its packetHandlers table.
 */
struct _Worker
{
    /** Start up information */
    WorkerStartupInfo info;

    /** The publisher socket to send asynchronous messages to the Event Server */
    zsock_t *eventServer;

    /** Seed for the random generator */
    uint32_t seed;

    // === Database ===
    /** The MySQL session */
    MySQL *sqlConn;

    /** The Redis session */
    Redis *redis;
};

/**
 * @brief PacketHandler contains a function handler and its name
 */
struct _PacketHandler {
    /** Function handler */
    PacketHandlerFunction handler;
    /** The packet name  */
    char *packetName;
};

/**
 * @brief Return state of the packet handlers.
 */
enum _PacketHandlerState {
    PACKET_HANDLER_ERROR          = -1,
    PACKET_HANDLER_OK             = 0,
    PACKET_HANDLER_UPDATE_SESSION = 1,
    PACKET_HANDLER_DELETE_SESSION = 2
};


// ----------- Functions ------------

/**
 * @brief Allocate a new Worker structure.
 * @param info An initialized WorkerStartupInfo structure.
 * @return A pointer to an allocated Worker.
 */
Worker *
Worker_new (
    WorkerStartupInfo *info
);

/**
 * @brief Initialize an allocated Worker structure.
 * @param self An allocated Worker to initialize.
 * @param info An initialized WorkerStartupInfo structure.
 * @return true on success, false otherwise.
 */
bool
Worker_init (
    Worker *self,
    WorkerStartupInfo *info
);

/**
 * @brief Initialize an allocated WorkerStartupInfo structure.
 * @param self An allocated WorkerStartupInfo to initialize.
 * @param workerId The worker ID.
 * @param routerId The Server ID
 * @param serverType The Server type having the responsibility of the worker
 * @param globalServerIp The IP of the global server
 * @param globalServerPort The private port exposed to the global server
 * @param sqlHost The information about the SQL Database
 * @param redisInfo The information about the Redis Database
 * @param packetHandlers Array of packet handlers
 * @param packetHandlersCount Packet handlers entries count
 * @return true on success, false otherwise.
 */
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
);


/**
 * @brief Start a new Worker
 * @param self An allocated Worker to start
 * @return true on success, false otherwise.
 */
bool
Worker_start (
    Worker *self
);


/**
 * @brief Worker routine for the zone server.
 * @param arg A Worker
 * @return Always NULL
 */
void *
Worker_mainLoop (
    void *arg
);


bool
Worker_dispatchEvent (
    Worker *self,
    EventServerType eventType,
    void *event,
    size_t eventSize
);

/**
 * @brief Free an allocated Worker structure.
 * @param self A pointer to an allocated Worker.
 */
void
Worker_free (
    Worker *self
);

/**
 * @brief Free an allocated Worker structure and nullify the content of the pointer.
 * @param self A pointer to an allocated Worker.
 */
void
Worker_destroy (
    Worker **self
);

