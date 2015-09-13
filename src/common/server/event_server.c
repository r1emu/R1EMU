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

#include "event_server.h"
#include "event_handler.h"
#include "common/graph/graph.h"
#include "common/redis/redis.h"
#include "common/packet/packet.h"
#include "common/redis/fields/redis_game_session.h"
#include "common/redis/fields/redis_session.h"
#include "barrack_server/barrack_event_server.h"
#include "zone_server/zone_event_server.h"

/**
 * @brief EventServer communicates with the Router and broadcast packets to the clients
 */
struct EventServer
{
    // socket connected to the workers
    zsock_t *eventsInput;

    // frontend connected to the router
    zsock_t *router;

    // Redis connection
    Redis *redis;

    // graph of interconnected clients
    Graph *clientsGraph;

    // EventServer information
    EventServerInfo info;

    // EventServer process routine
    bool (*eventServerProcess)(EventServer *self, EventType type, void *eventData);
};

static int eventServerSubscribe(EventServer *self);

EventServer *eventServerNew(EventServerInfo *info, ServerType serverType) {
    EventServer *self;

    if ((self = calloc(1, sizeof(EventServer))) == NULL) {
        return NULL;
    }

    if (!eventServerInit(self, info, serverType)) {
        eventServerDestroy(&self);
        error("EventServer failed to initialize.");
        return NULL;
    }

    return self;
}

bool eventServerInit(EventServer *self, EventServerInfo *info, ServerType serverType) {
    memcpy(&self->info, info, sizeof(self->info));

    // create a unique publisher for the EventServer
    if (!(self->eventsInput = zsock_new(ZMQ_SUB))) {
        error("Cannot allocate a new Server SUBSCRIBER");
        return false;
    }

    // create a connection to the router
    if (!(self->router = zsock_new(ZMQ_PUB))) {
        error("Cannot create zsock to the router.");
        return false;
    }

    // initialize Redis connection
    if (!(self->redis = redisNew(&info->redisInfo))) {
        error("Cannot initialize a new Redis connection.");
        return false;
    }

    // initialize hashtable of clients around
    if (!(self->clientsGraph = graphNew())) {
        error("Cannot allocate a new clients Graph.");
        return false;
    }

    switch (serverType)
    {
        case SERVER_TYPE_BARRACK:
            self->eventServerProcess = barrackEventServerProcess;
            break;
        case SERVER_TYPE_SOCIAL:
            // self->eventServerProcess = socialEventServerProcess;
            break;
        case SERVER_TYPE_ZONE:
            self->eventServerProcess = zoneEventServerProcess;
            break;
        default:
            // No EventServer registered for this server
        break;
    }

    return true;
}

GraphNodeClient *graphNodeClientNew (void) {
    GraphNodeClient *self;

    if ((self = calloc(1, sizeof(GraphNodeClient))) == NULL) {
        return NULL;
    }

    if (!graphNodeClientInit (self)) {
        graphNodeClientDestroy (&self);
        error("GraphNodeClient failed to initialize.");
        return NULL;
    }

    return self;
}

bool eventServerFlushSession(EventServer *self, uint8_t *sessionKey) {

    // Flush the redis session here
    RedisSessionKey redisSessionKey = {
        .socketKey = {
            .routerId = self->info.routerId,
            .sessionKey = sessionKey
        }
    };
    if (!(redisFlushSession (self->redis, &redisSessionKey))) {
        error ("Cannot flush the redis session '%s'", sessionKey);
        return false;
    }

    return true;
}

bool graphNodeClientInit (GraphNodeClient *self) {

    return true;
}

bool eventServerInfoInit (EventServerInfo *self,
    uint16_t routerId,
    uint16_t workersCount,
    char *redisHostname,
    int redisPort
) {
    memset(self, 0, sizeof(EventServerInfo));

    self->routerId = routerId;
    self->workersCount = workersCount;

    if (!(redisInfoInit(&self->redisInfo, redisHostname, redisPort))) {
        error("Cannot initialize Redis startup.");
        return false;
    }

    return true;
}

static bool
EventServer_handleEvent (
    EventServer *self,
    zmsg_t *msg
) {
    zframe_t *eventTypeFrame;

    // Get the event type frame
    if (!(eventTypeFrame = zmsg_first (msg))) {
        error("Event type cannot be retrieved.");
        return false;
    }

    // Convert the event type frame to a EventType
    EventType eventType = *((EventType *) zframe_data(eventTypeFrame));

    // Get the event data
    void *eventData = zframe_data(zmsg_next(msg));

    if (!self->eventServerProcess) {
        error("The EventServer received an event, but no EventServerProcess has been defined.");
        error("The event has been ignored.");
        return false;
    }

    if (!self->eventServerProcess(self, eventType, eventData)) {
        error("Event type = %d failed.", eventType);
        return false;
    }

    return true;
}

bool
eventServerGetGameSessionBySocketId (
    EventServer *self,
    uint16_t routerId,
    uint8_t *socketId,
    GameSession *gameSession
) {
    return redisGetGameSessionBySocketId (self->redis, routerId, socketId, gameSession);
}

static int
eventServerSubscribe (
    EventServer *self
) {
    zmsg_t *msg;
    zframe_t *header;

    // Receive the message from the publisher socket
    if (!(msg = zmsg_recv(self->eventsInput))) {
        // Interrupt
        return -1;
    }

    // Get the header frame of the message
    if (!(header = zmsg_pop (msg))) {
        error("Frame header cannot be retrieved.");
        return -1;
    }

    // Convert the header frame to a EventServerHeader
    EventServerHeader packetHeader = *((EventServerHeader *) zframe_data(header));
    zframe_destroy (&header);

    switch (packetHeader)
    {
        case EVENT_SERVER_EVENT :
            if (!(EventServer_handleEvent (self, msg))) {
                error("Cannot handle the event properly.");
                return 0;
            }
        break;

        default:
            warning("Server subscriber received an unknown header : %x", packetHeader);
        break;
    }

    // Cleanup
    zmsg_destroy(&msg);

    return 1;
}

bool
eventServerSendToClients (
    EventServer *self,
    zlist_t *clients,
    zmsg_t *broadcastMsg
) {
    bool result = true;
    zmsg_t *msg = NULL;
    zframe_t *frame = zmsg_first (broadcastMsg);
    uint8_t *packet = zframe_data(frame);
    size_t packetLen = zframe_size (frame);

    if ((!(msg = zmsg_new ()))
    ||  zmsg_addmem (msg, PACKET_HEADER (ROUTER_WORKER_MULTICAST), sizeof(ROUTER_WORKER_MULTICAST)) != 0
    ||  zmsg_addmem (msg, packet, packetLen) != 0
    ) {
        error("Cannot build the multicast packet.");
        result = false;
        goto cleanup;
    }

    // [1 frame data] + [1 frame identity] + [1 frame identity] + ...
    char *identityKey;
    for (identityKey = zlist_first (clients); identityKey != NULL; identityKey = zlist_next (clients)) {
        // Add all the clients to the packet
        uint8_t identityBytes[5];
        socketSessionGenId (identityKey, identityBytes);
        if (zmsg_addmem (msg, identityBytes, sizeof(identityBytes)) != 0) {
            error("Cannot add the identity in the message.");
            result = false;
            goto cleanup;
        }
    }

    if (zmsg_send (&msg, self->router) != 0) {
        error("Cannot send the multicast packet to the Router.");
        result = false;
        goto cleanup;
    }

cleanup:
    zmsg_destroy(&msg);
    return result;
}

bool eventServerDispatchEvent(
    zsock_t *eventServer,
    uint8_t *emitterSk,
    EventType eventType,
    void *event,
    size_t eventSize)
{
    bool result = true;
    zmsg_t *msg = NULL;

    GameEvent gameEvent = {
        .emitterSk = SOCKET_ID_ARRAY(emitterSk),
        .type = eventType
    };

    memcpy (&gameEvent.data, event, eventSize);
    size_t gameEventSize = sizeof(GameEvent) - sizeof (EventDataCategories) + eventSize;

    if ((!(msg = zmsg_new ()))
    ||  zmsg_addmem (msg, PACKET_HEADER (EVENT_SERVER_EVENT), sizeof(EVENT_SERVER_EVENT)) != 0
    ||  zmsg_addmem (msg, PACKET_HEADER (eventType), sizeof(eventType)) != 0
    ||  zmsg_addmem (msg, &gameEvent, gameEventSize) != 0
    ) {
        error("Cannot build the event message.");
        result = false;
        goto cleanup;
    }

    if (zmsg_send (&msg, eventServer) != 0) {
        error("Cannot send the event packet.");
        result = false;
        goto cleanup;
    }

cleanup:
    zmsg_destroy(&msg);
    return result;
}

bool
eventServerSendToClient (
    EventServer *self,
    uint8_t *identityKey,
    uint8_t *packet,
    size_t packetLen
) {
    bool result = true;
    zmsg_t *msg = NULL;

    if ((!(msg = zmsg_new ()))
    ||  zmsg_addmem (msg, PACKET_HEADER (ROUTER_WORKER_MULTICAST), sizeof(ROUTER_WORKER_MULTICAST)) != 0
    ||  zmsg_addmem (msg, packet, packetLen) != 0
    ) {
        error("Cannot build the multicast packet.");
        result = false;
        goto cleanup;
    }

    // Add the client identity to the packet
    uint8_t identityBytes[5];
    socketSessionGenId (identityKey, identityBytes);
    if (zmsg_addmem (msg, identityBytes, sizeof(identityBytes)) != 0) {
        error("Cannot add the identity in the message.");
        result = false;
        goto cleanup;
    }

    if (zmsg_send (&msg, self->router) != 0) {
        error("Cannot send the multicast packet to the Router.");
        result = false;
        goto cleanup;
    }

cleanup:
    zmsg_destroy(&msg);
    return result;
}

uint16_t
eventServerGetRouterId (
    EventServer *self
) {
    return self->info.routerId;
}

zlist_t *
eventServerRedisGetClientsWithinRange (
    EventServer *self,
    uint16_t mapId,
    uint8_t *ignoredSessionKey,
    PositionXZ *position,
    float range
) {
    return redisGetClientsWithinDistance (self->redis, self->info.routerId, mapId, position, range, ignoredSessionKey);
}

bool eventServerRemoveClient(EventServer *self, uint8_t *sessionKey)
{
    GraphNode *node;

    // Get the node associated with the sessionKey
    if (!(node = eventServerGetClientNode (self, sessionKey))) {
        error("Cannot get the node %s.", sessionKey);
        return false;
    }

    if (!(graphRemoveNode(self->clientsGraph, node))) {
        error ("Cannot remove a client '%s' from the graph.", sessionKey);
        return false;
    }

    return true;
}

bool
eventServerGetClientsAround (
    EventServer *self,
    uint8_t *sessionKey,
    zlist_t **_clients
) {
    bool status = true;
    GraphNode *node;
    zlist_t *clients = NULL;

    if (!(clients = zlist_new())) {
        error("Cannot allocate a new clients list.");
        status = false;
        goto cleanup;
    }
    *_clients = clients;

    // Get the node associated with the sessionKey
    if (!(node = eventServerGetClientNode (self, sessionKey))) {
        error("Cannot get the node %s.", sessionKey);
        return NULL;
    }

    // Add the neighbors keys to the clients list
    for (GraphArc *arc = zlist_first (node->arcs); arc != NULL; arc = zlist_next (node->arcs)) {
        GraphNode *nodeAround = arc->to;
        zlist_append(clients, nodeAround->key);
    }

cleanup:
    return status;
}

bool
eventServerStart (
    EventServer *self
) {
    // Start Redis
    if (!(redisConnection (self->redis))) {
        error("Cannot connect to the Redis Server.");
        return false;
    }

    // Bind the connection to the router
    if (zsock_bind(self->router, ROUTER_SUBSCRIBER_ENDPOINT, self->info.routerId) != 0) {
        error("Failed to bind to the subscriber endpoint.");
        return false;
    }

    // Initialize worker event subscribers
    for (int workerId = 0; workerId < self->info.workersCount; workerId++) {
        if (zsock_connect (self->eventsInput, EVENT_SERVER_SUBSCRIBER_ENDPOINT, self->info.routerId, workerId) != 0) {
            error("Failed to connect to the worker subscriber endpoint %d:%d.", self->info.routerId, workerId);
            return false;
        }
        info("EventServer subscribed to %s",
             zsys_sprintf(EVENT_SERVER_SUBSCRIBER_ENDPOINT, self->info.routerId, workerId));
    }

    // Initialize router monitor event subscriber
    if (zsock_connect (self->eventsInput, EVENT_SERVER_MONITOR_ENDPOINT, self->info.routerId) != 0) {
        error("Failed to connect to the router monitor subscriber endpoint %d.", self->info.routerId);
        return false;
    }
    info("EventServer subscribed to %s", zsys_sprintf(EVENT_SERVER_MONITOR_ENDPOINT, self->info.routerId));

    // Subscribe to all messages, without any filter
    zsock_set_subscribe(self->eventsInput, "");

    // Listen to the subscriber socket
    while (eventServerSubscribe (self) != -1) {
        // Things to do between two messages ?
    }

    info("EventServer exits...");
    return true;
}

bool
eventServerLinkClients (
    EventServer *self,
    GraphNode *node1,
    GraphNode *node2
) {
    return graphLink (self->clientsGraph, node1, node2);
}

bool
eventServerUnlinkClients (
    EventServer *self,
    GraphNode *node1,
    GraphNode *node2
) {
    return graphUnlink (self->clientsGraph, node1, node2);
}

GraphNode *
eventServerGetClientNode (
    EventServer *self,
    uint8_t *sessionKey
) {
    GraphNode *clientNode;

    if (!(clientNode = graphGetNode (self->clientsGraph, sessionKey))) {
        // Client node doesn't exist, create it
        info("Add client %s in the graph", sessionKey);
        if (!(clientNode = graphNodeNew (sessionKey, NULL))) {
            error("Cannot allocate a new client node.");
            return NULL;
        }

        clientNode->user_data = graphNodeClientNew ();
        // Add it to the hashtable
        if (!(graphInsertNode (self->clientsGraph, clientNode))) {
            error("Cannot insert a new client node.");
            return NULL;
        }
    }

    return clientNode;
}

void
eventServerFree (
    EventServer *self
) {
    // TODO
}

void
eventServerDestroy (
    EventServer **_self
) {
    EventServer *self = *_self;

    if (self) {
        eventServerFree (self);
        free(self);
    }

    *_self = NULL;
}

void
graphNodeClientFree (
    GraphNodeClient *self
) {
    // TODO
}

void
graphNodeClientDestroy (
    GraphNodeClient **_self
) {
    GraphNodeClient *self = *_self;

    if (self) {
        graphNodeClientFree (self);
        free(self);
    }

    *_self = NULL;
}
