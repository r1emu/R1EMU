/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file event_server.h
 * @brief EventServer broadcast the game events to the concerned clients
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#pragma once

#include "R1EMU.h"
#include "common/graph/graph.h"
#include "common/session/session.h"
#include "common/server/router.h"

#define EVENT_SERVER_EXECUTABLE_NAME             "EventServer"
#define EVENT_SERVER_SUBSCRIBER_ENDPOINT         "inproc://eventServerWorkersSubscriber-%d-%d"

/** Enumeration of all the packets headers that the EventServer handles */
// we want to differentiate the headers being received from the the ones being send, but we also want to keep a list
// with uniques header IDs. So, let's declare all the IDs here, and distribute them afterward
typedef enum EventServerHeader {
    _EVENT_SERVER_PING,     // ping message
    _EVENT_SERVER_PONG,     // pong message
    _EVENT_SERVER_EVENT     // event message
}   EventServerHeader;

// macro helper for the distribution
#define DECL_EVENT_SERVER_HEADER(x) \
    x = _##x

/** Enumeration of all the packets header that the EventServer accepts */
typedef enum EventServerRecvHeader {
    DECL_EVENT_SERVER_HEADER(EVENT_SERVER_PING),
    DECL_EVENT_SERVER_HEADER(EVENT_SERVER_EVENT),
} EventServerRecvHeader;

/** Enumeration of all the packets header that the EventServer sends */
typedef enum EventServerSendHeader {
    DECL_EVENT_SERVER_HEADER(EVENT_SERVER_PONG),
} EventServerSendHeader;

#undef DECL_EVENT_SERVER_HEADER

typedef enum EventServerType {
    EVENT_SERVER_TYPE_COMMANDER_MOVE,
    EVENT_SERVER_TYPE_REST_SIT,
    EVENT_SERVER_TYPE_JUMP,
    EVENT_SERVER_TYPE_MOVE_STOP,
    EVENT_SERVER_TYPE_ENTER_PC,
    EVENT_SERVER_TYPE_CHAT,
    EVENT_SERVER_TYPE_HEAD_ROTATE
} EventServerType;

typedef struct {
    uint16_t mapId;
    uint8_t sessionKey[SOCKET_SESSION_ID_SIZE];
    CommanderInfo commanderInfo;
} GameEventUpdatePosition;

typedef struct {
    uint16_t routerId;
    uint16_t workersCount;
    RedisStartupInfo redisInfo;
} EventServerStartupInfo;

typedef struct {
    bool around;
} GraphNodeClient;

typedef struct EventServer EventServer;

/**
 * @brief Allocate a new EventServer structure.
 * @param info An allocated EventServerStartupInfo containing the information for starting up the event server.
 * @return A pointer to an allocated EventServer.
 */
EventServer *eventServerNew(EventServerStartupInfo *info);

/**
 * @brief Initialize an allocated EventServer structure.
 * @param self An allocated EventServer to initialize.
 * @param info An allocated EventServerStartupInfo containing the information for starting up the event server.
 * @return true on success, false otherwise.
 */
bool eventServerInit(EventServer *self, EventServerStartupInfo *info);

/**
 * @brief Allocate a new GraphNodeClient structure.
 * @return A pointer to an allocated GraphNodeClient.
 */
GraphNodeClient *graphNodeClientNew(void);

/**
 * @brief Initialize an allocated GraphNodeClient structure.
 * @param self An allocated GraphNodeClient to initialize.
 * @return true on success, false otherwise.
 */
bool graphNodeClientInit(GraphNodeClient *self);

/**
 * @brief Free an allocated GraphNodeClient structure.
 * @param self A pointer to an allocated GraphNodeClient.
 */
void graphNodeClientFree(GraphNodeClient *self);

/**
 * @brief Free an allocated GraphNodeClient structure and nullify the content of the pointer.
 * @param self A pointer to an allocated GraphNodeClient.
 */
void graphNodeClientDestroy(GraphNodeClient **self);

/**
 * @brief Free an allocated EventServer structure.
 * @param self A pointer to an allocated EventServer.
 */
void eventServerFree(EventServer *self);

/**
 * @brief Free an allocated EventServer structure and nullify the content of the pointer.
 * @param self A pointer to an allocated EventServer.
 */
void eventServerDestroy(EventServer **self);

/**
 * @brief Update the position of a client within the event server structures
 * @param self A pointer to an allocated EventServer.
 * @param updatePosEvent Contains needed information for updating a commander position
 * @param newPosition The new position of the client
 * @param[out] redisClientsAround The list of other clients around the client at its new position
 * @return true on success, false otherwise
 */
bool eventServerUpdateClientPosition(
    EventServer *self,
    GameEventUpdatePosition *updatePosEvent,
    PositionXYZ *newPosition,
    zlist_t **_redisClientsAround);

/**
 * @brief Link 2 clients together.
 * @param self A pointer to an allocated EventServer.
 * @param node1 A first client node
 * @param node2 A second client node
 * @return true on success, false otherwise
 */
bool eventServerLinkClients(EventServer *self, GraphNode *node1, GraphNode *node2);

/**
 * @brief Unlink 2 clients connected.
 * @param self A pointer to an allocated EventServer.
 * @param node1 A first client node
 * @param node2 A second client node
 * @return true on success, false otherwise
 */
bool eventServerUnlinkClients(EventServer *self, GraphNode *node1, GraphNode *node2);

/**
 * @brief Initialize an allocated EventServerStartupInfo structure.
 * @param self An allocated EventServerStartupInfo to initialize.
 * @param routerId The routerID
 * @param workersCount The workers count
 * @return true on success, false otherwise.
 */
bool eventServerStartupInfoInit(
    EventServerStartupInfo *self,
    uint16_t routerId,
    uint16_t workersCount,
    char *redisHostname,
    int redisPort);

/**
 * @brief Return a list of clients around a given player
 * @param self An allocated EventServer
 * @param sessionKey The player sessionKey
 * @param[out] _clients The result list of clients socketIds
 * @return true on success, false otherwise
 */
bool eventServerGetClientsAround(
    EventServer *self,
    uint8_t *sessionKey,
    zlist_t **_clients);

/**
 * @brief Send the same packet to multiple clients
 * @param self An allocated EventServer
 * @param clients A list of clients socket identity
 * @param msg the message to broadcast
 * @return true on success, false otherwise
 */
bool eventServerSendToClients(
    EventServer *self,
    zlist_t *clients,
    zmsg_t *msg);

/**
 * @brief Send a packet to one client
 * @param self An allocated EventServer
 * @param identityKey A socket identity
 * @param packet The packet to send
 * @param packetLen the length of the packet
 * @return true on success, false otherwise
 */
bool eventServerSendToClient(
    EventServer *self,
    uint8_t *identityKey,
    uint8_t *packet,
    size_t packetLen);

/**
 * @brief : Router ID Accessor
 */
uint16_t eventServerGetRouterId(EventServer *self);

/**
 * @brief : GameSessionBySocketId by Redis module Accessor
 */
bool eventServerGetGameSessionBySocketId(
    EventServer *self,
    uint16_t routerId,
    uint8_t *socketId,
    GameSession *gameSession);

/**
 * @brief Start the Zone Server main loop.
 * @param self An allocated EventServer
 * @return true on success, false otherwise
 */
bool eventServerStart(EventServer *self);

/**
 * Get a client node based on its key
 */
GraphNode *eventServerGetClientNode(EventServer *self, uint8_t *socketId);
