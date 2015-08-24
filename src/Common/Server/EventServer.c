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

// ---------- Includes ------------
#include "EventServer.h"
#include "EventHandler.h"
#include "Common/Graph/Graph.h"
#include "Common/Redis/Redis.h"
#include "Common/Redis/Fields/RedisGameSession.h"
#include "ZoneServer/ZoneHandler/ZoneBuilder.h"
#include "Common/Packet/Packet.h"

// ------ Structure declaration -------
/**
 * @brief EventServer communicates with the Router and broadcast packets to the clients
 */
struct EventServer
{
    /** Socket connected to the workers */
    zsock_t *workers;

    /** Frontend connected to the router */
    zsock_t *router;

    /** Redis connection **/
    Redis *redis;

    /** Graph of interconnected clients **/
    Graph *clientsGraph;

    /** EventServer information */
    EventServerStartupInfo info;
};


// ------ Static declaration ------
static bool
EventServer_subscribe (
    EventServer *self
);


/**
 * @brief Return a list of clients into an area according to the Redis database
 * @param self An allocated EventServer
 * @param mapId : The mapId of the target zone
 * @param ignoredSessionKey A socketID to ignore. NULL don't ignore anybody.
 * @param center The position of the center of the circle
 * @param range Radius of the circle
 * @return a zlist_t of identity keys
 */
static zlist_t *
EventServer_redisGetClientsWithinRange (
    EventServer *self,
    uint16_t mapId,
    uint8_t *ignoredSessionKey,
    PositionXZ *center,
    float range
);

// ------ Extern function implementation ------

EventServer *
EventServer_new (
    EventServerStartupInfo *info
) {
    EventServer *self;

    if ((self = calloc (1, sizeof (EventServer))) == NULL) {
        return NULL;
    }

    if (!EventServer_init (self, info)) {
        EventServer_destroy (&self);
        error ("EventServer failed to initialize.");
        return NULL;
    }

    return self;
}


bool
EventServer_init (
    EventServer *self,
    EventServerStartupInfo *info
) {
    memcpy (&self->info, info, sizeof (self->info));

    // Create a unique publisher for the EventServer
    if (!(self->workers = zsock_new (ZMQ_SUB))) {
        error ("Cannot allocate a new Server SUBSCRIBER");
        return false;
    }

    // Create a connection to the router
    if (!(self->router = zsock_new (ZMQ_PUB))) {
        error ("Cannot create zsock to the router.");
        return false;
    }

    // Initialize Redis connection
    if (!(self->redis = Redis_new (&info->redisInfo))) {
        error ("Cannot initialize a new Redis connection.");
        return false;
    }

    // Initialize hashtable of clients around
    if (!(self->clientsGraph = Graph_new ())) {
        error ("Cannot allocate a new clients Graph.");
        return false;
    }

    return true;
}

GraphNodeClient *
GraphNodeClient_new (
    void
) {
    GraphNodeClient *self;

    if ((self = calloc (1, sizeof (GraphNodeClient))) == NULL) {
        return NULL;
    }

    if (!GraphNodeClient_init (self)) {
        GraphNodeClient_destroy (&self);
        error ("GraphNodeClient failed to initialize.");
        return NULL;
    }

    return self;
}


bool
GraphNodeClient_init (
    GraphNodeClient *self
) {
    return true;
}

bool
EventServerStartupInfo_init (
    EventServerStartupInfo *self,
    uint16_t routerId,
    uint16_t workersCount,
    char *redisHostname,
    int redisPort
) {
    memset (self, 0, sizeof (EventServerStartupInfo));

    self->routerId = routerId;
    self->workersCount = workersCount;

    if (!(RedisStartupInfo_init (&self->redisInfo, redisHostname, redisPort))) {
        error ("Cannot initialize Redis startup.");
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
        error ("Event type cannot be retrieved.");
        return false;
    }

    // Convert the event type frame to a EventServerType
    EventServerType eventType = *((EventServerType *) zframe_data (eventTypeFrame));

    // Get the event data
    void *eventData = zframe_data (zmsg_next (msg));
    bool result = false;

    switch (eventType)
    {
        case EVENT_SERVER_TYPE_COMMANDER_MOVE : {
            result = EventHandler_commanderMove (self, eventData);
        } break;

        case EVENT_SERVER_TYPE_MOVE_STOP : {
            result = EventHandler_moveStop (self, eventData);
        } break;

        case EVENT_SERVER_TYPE_REST_SIT : {
            result = EventHandler_restSit (self, eventData);
        } break;

        case EVENT_SERVER_TYPE_CHAT : {
            result = EventHandler_chat (self, eventData);
        } break;

        case EVENT_SERVER_TYPE_ENTER_PC : {
            result = EventHandler_enterPc (self, eventData);
        } break;

        case EVENT_SERVER_TYPE_JUMP : {
            result = EventHandler_jump (self, eventData);
        } break;

        default :
            error ("Unknown event type received : %d", eventType);
            result = false;
        break;
    }

    if (!result) {
        error ("Event type = %d failed.", eventType);
        return false;
    }

    return true;
}

bool
EventServer_getGameSessionBySocketId (
    EventServer *self,
    uint16_t routerId,
    uint8_t *socketId,
    GameSession *gameSession
) {
    return Redis_getGameSessionBySocketId (self->redis, routerId, socketId, gameSession);
}

static bool
EventServer_subscribe (
    EventServer *self
) {
    zmsg_t *msg;
    zframe_t *header;

    // Receive the message from the publisher socket
    if (!(msg = zmsg_recv (self->workers))) {
        // Interrupt
        return false;
    }

    // Get the header frame of the message
    if (!(header = zmsg_pop (msg))) {
        error ("Frame header cannot be retrieved.");
        return false;
    }

    // Convert the header frame to a EventServerHeader
    EventServerHeader packetHeader = *((EventServerHeader *) zframe_data (header));
    zframe_destroy (&header);

    switch (packetHeader)
    {
        case EVENT_SERVER_EVENT :
            if (!(EventServer_handleEvent (self, msg))) {
                error ("Cannot handle the event properly.");
                return false;
            }
        break;

        default:
            warning ("Server subscriber received an unknown header : %x", packetHeader);
        break;
    }

    // Cleanup
    zmsg_destroy (&msg);

    return true;
}

bool
EventServer_sendToClients (
    EventServer *self,
    zlist_t *clients,
    uint8_t *packet,
    size_t packetLen
) {
    bool result = true;
    zmsg_t *msg = NULL;

    if ((!(msg = zmsg_new ()))
    ||  zmsg_addmem (msg, PACKET_HEADER (ROUTER_WORKER_MULTICAST), sizeof (ROUTER_WORKER_MULTICAST)) != 0
    ||  zmsg_addmem (msg, packet, packetLen) != 0
    ) {
        error ("Cannot build the multicast packet.");
        result = false;
        goto cleanup;
    }

    // [1 frame data] + [1 frame identity] + [1 frame identity] + ...
    char *identityKey;
    for (identityKey = zlist_first (clients); identityKey != NULL; identityKey = zlist_next (clients)) {
        // Add all the clients to the packet
        uint8_t identityBytes[5];
        SocketSession_genId (identityKey, identityBytes);
        if (zmsg_addmem (msg, identityBytes, sizeof (identityBytes)) != 0) {
            error ("Cannot add the identity in the message.");
            result = false;
            goto cleanup;
        }
    }

    if (zmsg_send (&msg, self->router) != 0) {
        error ("Cannot send the multicast packet to the Router.");
        result = false;
        goto cleanup;
    }

cleanup:
    zmsg_destroy (&msg);
    return result;
}

bool
EventServer_updateClientPosition (
    EventServer *self,
    GameEventUpdatePosition *updatePosEvent,
    PositionXYZ *newPosition,
    zlist_t **_redisClientsAround
) {
    bool status = true;

    zmsg_t *pcEnterMsg = NULL;
    zmsg_t *curPcEnterMsg = NULL;
    zmsg_t *pcLeaveMsg = NULL;
    zmsg_t *curPcLeaveMsg = NULL;
    zlist_t *pcEnterList = NULL;
    zlist_t *pcLeaveList = NULL;
    zlist_t *redisClientsAround = NULL;

    uint8_t *sessionKey = updatePosEvent->sessionKey;
    uint16_t mapId = updatePosEvent->mapId;
    CommanderInfo *commanderInfo = &updatePosEvent->commanderInfo;

    // Get the clients around
    if (!(redisClientsAround = EventServer_redisGetClientsWithinRange (
        self, mapId, sessionKey, &PositionXYZToXZ (newPosition),
        COMMANDER_RANGE_AROUND
    ))) {
        error ("Cannot get clients within range");
        status = false;
        goto cleanup;
    }
    *_redisClientsAround = redisClientsAround;

    // Get the node of the current client
    GraphNode *nodeCurrentClient = EventServer_getClientNode (self, sessionKey);

    // Mark the neighbours nodes as unvisited
    for (GraphArc *neighbourArc = zlist_first (nodeCurrentClient->arcs);
        neighbourArc != NULL;
        neighbourArc = zlist_next (nodeCurrentClient->arcs)
    ) {
        GraphNode *neighbourNode = neighbourArc->to;
        GraphNodeClient *neighbourClient = neighbourNode->user_data;
        neighbourClient->around = false;
    }

    if (zlist_size (redisClientsAround) > 0)
    {
        pcEnterList = zlist_new ();

        // Compare the list from Redis to the list of neighbors from the proximity graph
        for (uint8_t *redisSocketIdClientAround = zlist_first (redisClientsAround);
             redisSocketIdClientAround != NULL;
             redisSocketIdClientAround = zlist_next (redisClientsAround)
        ) {
            GraphNode *graphClientAroundNode;
            if (!(graphClientAroundNode = EventServer_getClientNode (self, redisSocketIdClientAround))) {
                error ("Cannot get the neighbour node %s.", redisSocketIdClientAround);
                status = false;
                goto cleanup;
            }

            if (!(GraphNode_getArc (nodeCurrentClient, graphClientAroundNode))) {
                // nodeCurrentClient isn't linked yet with its neighbour
                // It means that the current client has just entered in the neighbour client zone !
                // Mutually link them together in the graph, and warn the game clients of the arrival of a new client
                EventServer_linkClients (self, nodeCurrentClient, graphClientAroundNode);
                zlist_append (pcEnterList, redisSocketIdClientAround);
            }

            GraphNodeClient *graphNeighbourClient = graphClientAroundNode->user_data;
            graphNeighbourClient->around = true;
        }

        // Send the ZC_PC_ENTER to clients who now sees the current client
        if (zlist_size (pcEnterList) > 0) {
            pcEnterMsg = zmsg_new ();
            ZoneBuilder_enterPc (commanderInfo, pcEnterMsg);
            zframe_t *pcEnterFrame = zmsg_first (pcEnterMsg);
            if (!(EventServer_sendToClients (self, redisClientsAround, zframe_data (pcEnterFrame), zframe_size (pcEnterFrame)))) {
                error ("Failed to send the packet to the clients.");
                status = false;
                goto cleanup;
            }
        }

        // Also, send to the current player the list of entered players
        for (uint8_t *enterPcSocketId = zlist_first (pcEnterList);
             enterPcSocketId != NULL;
             enterPcSocketId = zlist_next (pcEnterList)
        ) {
            if (strcmp (enterPcSocketId, sessionKey) == 0) {
                // Doesn't send again the packet if the client sees himself
                continue;
            }

            GameSession gameSession;
            curPcEnterMsg = zmsg_new ();
            if (!(EventServer_getGameSessionBySocketId (self, self->info.routerId, enterPcSocketId, &gameSession))) {
                error ("Cannot get game session from %s.", enterPcSocketId);
                status = false;
                goto cleanup;
            }

            ZoneBuilder_enterPc (&gameSession.commanderSession.currentCommander, curPcEnterMsg);
            zframe_t *pcEnterFrame = zmsg_first (curPcEnterMsg);
            if (!(EventServer_sendToClient (self, sessionKey, zframe_data (pcEnterFrame), zframe_size (pcEnterFrame)))) {
                error ("Failed to send the packet to the clients.");
                status = false;
                goto cleanup;
            }

            zmsg_destroy (&curPcEnterMsg);
        }
    }

    // Check for ZC_LEAVE and build a list of clients involved
    pcLeaveList = zlist_new ();
    for (GraphArc *neighbourArc = zlist_first (nodeCurrentClient->arcs);
        neighbourArc != NULL;
        neighbourArc = zlist_next (nodeCurrentClient->arcs)
    ) {
        GraphNode *neighbourNode = neighbourArc->to;
        GraphNodeClient *neighbourClient = neighbourNode->user_data;
        if (neighbourClient->around == false) {
            // The neighbour is still marked as "not around" : The current commander left its screen zone
            EventServer_unlinkClients (self, nodeCurrentClient, neighbourNode);
            zlist_append (pcLeaveList, neighbourNode->key);
        }
    }

    // Send the ZC_LEAVE to the players in the list
    if (zlist_size (pcLeaveList) > 0)
    {
        pcLeaveMsg = zmsg_new ();
        ZoneBuilder_leave (commanderInfo->pcId, pcLeaveMsg);
        zframe_t *pcLeaveFrame = zmsg_first (pcLeaveMsg);

        // Also, send to the current player the list of left players
        if (!(EventServer_sendToClients (self, pcLeaveList, zframe_data (pcLeaveFrame), zframe_size (pcLeaveFrame)))) {
            error ("Failed to send the packet to the clients.");
            status = false;
            goto cleanup;
        }

        // Also, send to the current player the list of left players
        for (uint8_t *leftPcSocketId = zlist_first (pcLeaveList);
             leftPcSocketId != NULL;
             leftPcSocketId = zlist_next (pcLeaveList)
        ) {
            GameSession gameSession;
            curPcLeaveMsg = zmsg_new ();
            if (!(EventServer_getGameSessionBySocketId (self, self->info.routerId, leftPcSocketId, &gameSession))) {
                error ("Cannot get game session from %s.", leftPcSocketId);
                status = false;
                goto cleanup;
            }

            ZoneBuilder_leave (gameSession.commanderSession.currentCommander.pcId, curPcLeaveMsg);
            zframe_t *pcLeaveFrame = zmsg_first (curPcLeaveMsg);
            if (!(EventServer_sendToClient (self, sessionKey, zframe_data (pcLeaveFrame), zframe_size (pcLeaveFrame)))) {
                error ("Failed to send the packet to the clients.");
                status = false;
                goto cleanup;
            }

            zmsg_destroy (&curPcLeaveMsg);
        }
    }

cleanup:
    zlist_destroy (&pcLeaveList);
    zlist_destroy (&pcEnterList);
    zmsg_destroy (&pcLeaveMsg);
    zmsg_destroy (&curPcLeaveMsg);
    zmsg_destroy (&pcEnterMsg);
    zmsg_destroy (&curPcEnterMsg);
    return status;
}

bool
EventServer_sendToClient (
    EventServer *self,
    uint8_t *identityKey,
    uint8_t *packet,
    size_t packetLen
) {
    bool result = true;
    zmsg_t *msg = NULL;

    if ((!(msg = zmsg_new ()))
    ||  zmsg_addmem (msg, PACKET_HEADER (ROUTER_WORKER_MULTICAST), sizeof (ROUTER_WORKER_MULTICAST)) != 0
    ||  zmsg_addmem (msg, packet, packetLen) != 0
    ) {
        error ("Cannot build the multicast packet.");
        result = false;
        goto cleanup;
    }

    // Add the client identity to the packet
    uint8_t identityBytes[5];
    SocketSession_genId (identityKey, identityBytes);
    if (zmsg_addmem (msg, identityBytes, sizeof (identityBytes)) != 0) {
        error ("Cannot add the identity in the message.");
        result = false;
        goto cleanup;
    }

    if (zmsg_send (&msg, self->router) != 0) {
        error ("Cannot send the multicast packet to the Router.");
        result = false;
        goto cleanup;
    }

cleanup:
    zmsg_destroy (&msg);
    return result;
}

uint16_t
EventServer_getRouterId (
    EventServer *self
) {
    return self->info.routerId;
}

static zlist_t *
EventServer_redisGetClientsWithinRange (
    EventServer *self,
    uint16_t mapId,
    uint8_t *ignoredSessionKey,
    PositionXZ *position,
    float range
) {
    return Redis_getClientsWithinDistance (self->redis, self->info.routerId, mapId, position, range, ignoredSessionKey);
}

bool
EventServer_getClientsAround (
    EventServer *self,
    uint8_t *sessionKey,
    zlist_t **_clients
) {
    bool status = true;
    GraphNode *node;
    zlist_t *clients = NULL;

    if (!(clients = zlist_new ())) {
        error ("Cannot allocate a new clients list.");
        status = false;
        goto cleanup;
    }
    *_clients = clients;

    // Get the node associated with the sessionKey
    if (!(node = EventServer_getClientNode (self, sessionKey))) {
        error ("Cannot get the node %s.", sessionKey);
        return NULL;
    }

    // Add the neighbors keys to the clients list
    for (GraphArc *arc = zlist_first (node->arcs); arc != NULL; arc = zlist_next (node->arcs)) {
        GraphNode *nodeAround = arc->to;
        zlist_append (clients, nodeAround->key);
    }

cleanup:
    return status;
}

bool
EventServer_start (
    EventServer *self
) {
    // Start Redis
    if (!(Redis_connect (self->redis))) {
        error ("Cannot connect to the Redis Server.");
        return false;
    }

    // Bind the connection to the router
    if (zsock_bind (self->router, ROUTER_SUBSCRIBER_ENDPOINT, self->info.routerId) != 0) {
        error ("Failed to bind to the subscriber endpoint.");
        return false;
    }

    // Initialize subscribers
    for (int workerId = 0; workerId < self->info.workersCount; workerId++) {
        if (zsock_connect (self->workers, EVENT_SERVER_SUBSCRIBER_ENDPOINT, self->info.routerId, workerId) != 0) {
            error ("Failed to connect to the subscriber endpoint %d:%d.", self->info.routerId, workerId);
            return false;
        }
        info ("EventServer subscribed to %s", zsys_sprintf (EVENT_SERVER_SUBSCRIBER_ENDPOINT, self->info.routerId, workerId));
        // Subscribe to all messages, without any filter
        zsock_set_subscribe (self->workers, "");
    }

    // Listen to the subscriber socket
    while (EventServer_subscribe (self)) {
        // Things to do between two messages ?
    }

    info ("EventServer exits...");
    return true;
}

bool
EventServer_linkClients (
    EventServer *self,
    GraphNode *node1,
    GraphNode *node2
) {
    return Graph_link (self->clientsGraph, node1, node2);
}

bool
EventServer_unlinkClients (
    EventServer *self,
    GraphNode *node1,
    GraphNode *node2
) {
    return Graph_unlink (self->clientsGraph, node1, node2);
}

GraphNode *
EventServer_getClientNode (
    EventServer *self,
    uint8_t *sessionKey
) {
    GraphNode *clientNode;

    if (!(clientNode = Graph_getNode (self->clientsGraph, sessionKey))) {
        // Client node doesn't exist, create it
        info ("Add client %s in the graph", sessionKey);
        if (!(clientNode = GraphNode_new (sessionKey, NULL))) {
            error ("Cannot allocate a new client node.");
            return NULL;
        }

        clientNode->user_data = GraphNodeClient_new ();
        // Add it to the hashtable
        if (!(Graph_insertNode (self->clientsGraph, clientNode))) {
            error ("Cannot insert a new client node.");
            return NULL;
        }
    }

    return clientNode;
}

void
EventServer_free (
    EventServer *self
) {
    // TODO
}

void
EventServer_destroy (
    EventServer **_self
) {
    EventServer *self = *_self;

    if (self) {
        EventServer_free (self);
        free (self);
    }

    *_self = NULL;
}

void
GraphNodeClient_free (
    GraphNodeClient *self
) {
    // TODO
}

void
GraphNodeClient_destroy (
    GraphNodeClient **_self
) {
    GraphNodeClient *self = *_self;

    if (self) {
        GraphNodeClient_free (self);
        free (self);
    }

    *_self = NULL;
}
