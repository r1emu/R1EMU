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

#include "zone_event_server.h"
#include "zone_handler/zone_builder.h"
#include "zone_handler/zone_event_handler.h"
#include "common/server/event_handler.h"
#include "common/redis/fields/redis_session.h"
#include "common/redis/fields/redis_game_session.h"
#include "common/mysql/fields/mysql_commander_session.h"

bool zoneEventServerProcess(EventServer *self, EventType type, void *eventData) {

    EventHandlerFunction handler;

    if (type <= EVENT_TYPE_ZONE_START || type >= EVENT_TYPE_ZONE_END) {
        error("Wrong event received from ZoneEventServer : %d", type);
        return false;
    }

    // Test if a handler is associated with the packet type requested.
    if (!(handler = zoneEventHandlers [type].handler)) {
        error("Cannot find handler for the requested packet type : %s",
            (type <= EVENT_TYPE_ZONE_START || type >= EVENT_TYPE_ZONE_END) ?
               "UNKNOWN" : zoneEventHandlers[type].eventName
        );
        return false;
    }

    return handler(self, eventData);
}

bool zoneEventServerOnDisconnect (
    zsock_t *eventServer,
    DbClient *dbSession,
    Redis *redis,
    MySQL *mysql,
    uint16_t routerId,
    uint8_t *sessionKeyStr
) {
    // Get the current game session
    DbObject *dbSessionObject = NULL;
    if (!(dbClientGetObjectSync(dbSession, sessionKeyStr, &dbSessionObject))) {
        error("Cannot get the session '%s'.", sessionKeyStr);
        return false;
    }

    Session *session = dbSessionObject->data;
    GameSession *gameSession = &session->game;

    // Send a EVENT_TYPE_LEAVE packet to the event server
    GameEventLeave event = {
        .pcId = gameSession->commanderSession.currentCommander->pcId
    };
    eventServerDispatchEvent(eventServer, sessionKeyStr, EVENT_TYPE_LEAVE, &event, sizeof(event));

    // Transfer the Redis session to SQL
    if (!(mySqlCommanderSessionFlush(mysql, &gameSession->commanderSession))) {
        error ("Cannot flush the redis session '%s' to the SQL.", sessionKeyStr);
        // return false;
        // TODO : Should we flush the Redis data in that case ?
    }

    // Flush the Redis session of the client
    RedisSessionKey sessionKey = {
        .socketSessionKey = {
            .routerId = routerId,
            .sessionKey = sessionKeyStr
        }
    };

    if (!(redisDeleteSession(redis, &sessionKey))) {
        error ("Cannot flush the redis session '%s'", sessionKeyStr);
        return false;
    }

    return true;
}

bool zoneEventServerUpdateClientPosition(
    EventServer *self,
    GameEvent *event,
    GameEventUpdatePosition *updatePosEvent,
    zlist_t **_redisClientsAround)
{
    bool status = true;

    zmsg_t *pcEnterMsg = NULL;
    zmsg_t *curPcEnterMsg = NULL;
    zmsg_t *pcLeaveMsg = NULL;
    zmsg_t *curPcLeaveMsg = NULL;
    zlist_t *pcEnterList = NULL;
    zlist_t *pcLeaveList = NULL;
    zlist_t *redisClientsAround = NULL;

    uint8_t *emitterSk = event->emitterSk;
    uint16_t mapId = updatePosEvent->mapId;
    Commander *commander = &updatePosEvent->commander;

    // Get the clients around
    if (!(redisClientsAround = eventServerRedisGetClientsWithinRange(
        self, mapId, emitterSk, &PositionXYZToXZ (&updatePosEvent->newPosition),
        COMMANDER_VIEW_RANGE
    ))) {
        error("Cannot get clients within range");
        status = false;
        goto cleanup;
    }
    *_redisClientsAround = redisClientsAround;

    // Get the node of the current client
    GraphNode *nodeCurrentClient = eventServerGetClientNode (self, emitterSk);

    // Mark the neighbours nodes as unvisited
    for (GraphArc *neighbourArc = zlist_first(nodeCurrentClient->arcs);
        neighbourArc != NULL;
        neighbourArc = zlist_next(nodeCurrentClient->arcs)
    ) {
        GraphNode *neighbourNode = neighbourArc->to;
        GraphNodeClient *neighbourClient = neighbourNode->user_data;
        neighbourClient->around = false;
    }

    if (zlist_size(redisClientsAround) > 0)
    {
        pcEnterList = zlist_new();

        // Compare the list from Redis to the list of neighbors from the proximity graph
        for (uint8_t *redisSocketIdClientAround = zlist_first(redisClientsAround);
             redisSocketIdClientAround != NULL;
             redisSocketIdClientAround = zlist_next(redisClientsAround)
        ) {
            GraphNode *graphClientAroundNode;
            if (!(graphClientAroundNode = eventServerGetClientNode (self, redisSocketIdClientAround))) {
                error("Cannot get the neighbour node %s.", redisSocketIdClientAround);
                status = false;
                goto cleanup;
            }

            if (!(graphNodeGetArc (nodeCurrentClient, graphClientAroundNode))) {
                // nodeCurrentClient isn't linked yet with its neighbour
                // It means that the current client has just entered in the neighbour client zone !
                // Mutually link them together in the graph, and warn the game clients of the arrival of a new client
                eventServerLinkClients (self, nodeCurrentClient, graphClientAroundNode);
                zlist_append(pcEnterList, redisSocketIdClientAround);
            }

            GraphNodeClient *graphNeighbourClient = graphClientAroundNode->user_data;
            graphNeighbourClient->around = true;
        }

        // Send the ZC_PC_ENTER to clients who now sees the current client
        if (zlist_size(pcEnterList) > 0) {
            pcEnterMsg = zmsg_new();
            zoneBuilderEnterPc(commander, pcEnterMsg);
            if (!(eventServerSendToClients(self, redisClientsAround, pcEnterMsg))) {
                error("Failed to send the packet to the clients.");
                status = false;
                goto cleanup;
            }
        }

        // Also, send to the current player the list of entered players
        for (uint8_t *enterPcSessionKey = zlist_first(pcEnterList);
             enterPcSessionKey != NULL;
             enterPcSessionKey = zlist_next(pcEnterList)
        ) {
            if (strcmp (enterPcSessionKey, emitterSk) == 0) {
                // Doesn't send again the packet - the client sees himself
                continue;
            }

            GameSession gameSession;
            curPcEnterMsg = zmsg_new();
            if (!(eventServerGetGameSessionBySocketId(
                self, eventServerGetRouterId(self), enterPcSessionKey, &gameSession)))
            {
                error("Cannot get game session from %s.", enterPcSessionKey);
                status = false;
                goto cleanup;
            }

            zoneBuilderEnterPc(gameSession.commanderSession.currentCommander, curPcEnterMsg);
            zframe_t *pcEnterFrame = zmsg_first(curPcEnterMsg);
            if (!(eventServerSendToClient (self, emitterSk, zframe_data(pcEnterFrame), zframe_size(pcEnterFrame)))) {
                error("Failed to send the packet to the clients.");
                status = false;
                goto cleanup;
            }

            zmsg_destroy(&curPcEnterMsg);
        }
    }

    // Check for ZC_LEAVE and build a list of clients involved
    pcLeaveList = zlist_new();
    for (GraphArc *neighbourArc = zlist_first(nodeCurrentClient->arcs);
        neighbourArc != NULL;
        neighbourArc = zlist_next(nodeCurrentClient->arcs)
    ) {
        GraphNode *neighbourNode = neighbourArc->to;
        GraphNodeClient *neighbourClient = neighbourNode->user_data;
        if (neighbourClient->around == false) {
            // The neighbour is still marked as "not around" : The current commander left its screen zone
            eventServerUnlinkClients (self, nodeCurrentClient, neighbourNode);
            zlist_append(pcLeaveList, neighbourNode->key);
        }
    }

    // Send the ZC_LEAVE to the players in the list
    if (zlist_size(pcLeaveList) > 0)
    {
        pcLeaveMsg = zmsg_new();
        zoneBuilderLeave(commander->pcId, pcLeaveMsg);

        // Also, send to the current player the list of left players
        if (!(eventServerSendToClients(self, pcLeaveList, pcLeaveMsg))) {
            error("Failed to send the packet to the clients.");
            status = false;
            goto cleanup;
        }

        // Also, send to the current player the list of left players
        for (uint8_t *leftPcSocketId = zlist_first(pcLeaveList);
             leftPcSocketId != NULL;
             leftPcSocketId = zlist_next(pcLeaveList)
        ) {
            GameSession gameSession;
            curPcLeaveMsg = zmsg_new();
            if (!(eventServerGetGameSessionBySocketId(
                self, eventServerGetRouterId(self), leftPcSocketId, &gameSession)))
            {
                error("Cannot get game session from %s.", leftPcSocketId);
                status = false;
                goto cleanup;
            }

            zoneBuilderLeave(gameSession.commanderSession.currentCommander->pcId, curPcLeaveMsg);
            zframe_t *pcLeaveFrame = zmsg_first(curPcLeaveMsg);
            if (!(eventServerSendToClient (self, emitterSk, zframe_data(pcLeaveFrame), zframe_size(pcLeaveFrame)))) {
                error("Failed to send the packet to the clients.");
                status = false;
                goto cleanup;
            }

            zmsg_destroy(&curPcLeaveMsg);
        }
    }

cleanup:
    zlist_destroy (&pcLeaveList);
    zlist_destroy (&pcEnterList);
    zmsg_destroy(&pcLeaveMsg);
    zmsg_destroy(&curPcLeaveMsg);
    zmsg_destroy(&pcEnterMsg);
    zmsg_destroy(&curPcEnterMsg);
    return status;
}
