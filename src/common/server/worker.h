/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file worker.h
 * @brief Worker is the component in the network architecture that processes the packet received from the Router.
 *
 * Here are the general steps that a worker always do :
 * - It receives a request from the Router
 * - It extracts the entity ID from the packet
 * - It searches for the session associated with the entity ID in the Redis server
 * - If it exists, return the session associated with the entity ID
 * - If it doesn't, create the session
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#pragma once

#include "R1EMU.h"
#include "event_server.h"
#include "common/mysql/mysql.h"
#include "common/redis/redis.h"
#include "common/session/session.h"

typedef struct _PacketHandler PacketHandler;
typedef struct _WorkerStartupInfo WorkerStartupInfo;
typedef struct _Worker Worker;
typedef enum _PacketHandlerState PacketHandlerState;

/**
 * @brief PacketHandlerFunction is the generic function prototype that a Worker is going to call when it receives a packet.
 * It takes the packet, calls packet builders based on the packet type, and reply to it using the \reply argument.
 */
typedef PacketHandlerState(*PacketHandlerFunction)(
    // a pointer to the current Worker
    Worker *self,

    // session of the current player
    Session *session,

    // packet sent by the client
    uint8_t *packet,

    // size of the packet sent by the client
    size_t packetSize,

    // reply buffer. If you need to send multiple replies, add multiple frames to this zmsg_t
    zmsg_t *reply);

/**
 * @brief WorkerStartupInfo contains all the information needed for a worker to start.
 */
struct _WorkerStartupInfo {
    // the worker ID
    uint16_t workerId;

    // the Server ID having authority on this worker
    uint32_t routerId;

    // the server type of the current Worker
    ServerType serverType;

    // IP of the global server
    char *globalServerIp;

    // private port with the global server
    int globalServerPort;

    // MySQL startup information
    MySQLStartupInfo sqlInfo;

    // Redis startup information
    RedisStartupInfo redisInfo;

    // array of packet handlers
    const PacketHandler *packetHandlers;

    // packet handlers entries count
    int packetHandlersCount;

    // Router information
    char *routerIp;
    int routerPort;
};

/**
 * @brief Worker is the main component of the network architecture, as it processes the packets received
 * It connects to the database and fullfil requests based on its packetHandlers table.
 */
struct _Worker {
    // start up information
    WorkerStartupInfo info;

    // the publisher socket to send asynchronous messages to the Event Server
    zsock_t *eventServer;

    // the connection to the session manager
    zsock_t *sessionManager;

    // seed for the random generator
    uint32_t seed;

    // the MySQL session
    MySQL *sqlConn;

    // the Redis session
    Redis *redis;
};

/**
 * @brief PacketHandler contains a function handler and its name
 */
struct _PacketHandler {
    // Function handler
    PacketHandlerFunction handler;

    // The packet name
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

/**
 * @brief Allocate a new Worker structure.
 * @param info An initialized WorkerStartupInfo structure.
 * @return A pointer to an allocated Worker.
 */
Worker *workerNew(WorkerStartupInfo *info);

/**
 * @brief Initialize an allocated Worker structure.
 * @param self An allocated Worker to initialize.
 * @param info An initialized WorkerStartupInfo structure.
 * @return true on success, false otherwise.
 */
bool workerInit(Worker *self, WorkerStartupInfo *info);

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
 * @param routerIp, routerPort IP and port of the router responsable of the worker
 * @return true on success, false otherwise.
 */
bool workerStartupInfoInit(
    WorkerStartupInfo *self,
    uint16_t workerId,
    uint16_t routerId,
    ServerType serverType,
    char *globalServerIp,
    int globalServerPort,
    MySQLStartupInfo *sqlInfo,
    RedisStartupInfo *redisInfo,
    const PacketHandler *packetHandlers,
    int packetHandlersCount,
    char *routerIp, int routerPort);

/**
 * @brief Start a new Worker
 * @param self An allocated Worker to start
 * @return true on success, false otherwise.
 */
bool workerStart(Worker *self);

/**
 * @brief Worker routine for the zone server.
 * @param arg A Worker
 * @return Always NULL
 */
void *workerMainLoop(void *arg);

/**
 * @brief Send an event to the attached EventServer
 * @param self An allocated Worker
 * @param emitterSk The session key of the event emitter
 * @param eventType The type of the event
 * @param event The event data
 * @param eventSize The event size
 */
bool workerDispatchEvent(Worker *self, uint8_t *emitterSk, EventType eventType, void *event, size_t eventSize);

/**
 * @brief Free an allocated Worker structure.
 * @param self A pointer to an allocated Worker.
 */
void workerFree(Worker *self);

/**
 * @brief Free an allocated Worker structure and nullify the content of the pointer.
 * @param self A pointer to an allocated Worker.
 */
void workerDestroy(Worker **self);
