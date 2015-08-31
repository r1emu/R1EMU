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
#include "zone_event_handler.h"
#include "zone_builder.h"
#include "zone_event.h"
#include "common/server/game_event.h"
#include "zone_server/zone_event_server.h"

static bool zoneEventHandlerCommanderMove(EventServer *self, GameEvent *event);
static bool zoneEventHandlerMoveStop(EventServer *self, GameEvent *event);
static bool zoneEventHandlerChat(EventServer *self, GameEvent *event);
static bool zoneEventHandlerRestSit(EventServer *self, GameEvent *event);
static bool zoneEventHandlerEnterPc(EventServer *self, GameEvent *event);
static bool zoneEventHandlerJump(EventServer *self, GameEvent *event);
static bool zoneEventHandlerHeadRotate(EventServer *self, GameEvent *event);
static bool zoneEventHandlerRotate(EventServer *self, GameEvent *event);
static bool zoneEventHandlerLeave(EventServer *self, GameEvent *event);

/**
 * @brief zoneEventHandlers is a global table containing all the zone event handlers.
 */
const EventHandler zoneEventHandlers[EVENT_TYPE_ZONE_END] = {
    #define REGISTER_PACKET_HANDLER(packetName, _handler) \
       [packetName] = {.handler = (void *) _handler, .eventName = STRINGIFY(packetName)}

    REGISTER_PACKET_HANDLER(EVENT_TYPE_COMMANDER_MOVE, zoneEventHandlerCommanderMove),
    REGISTER_PACKET_HANDLER(EVENT_TYPE_MOVE_STOP, zoneEventHandlerMoveStop),
    REGISTER_PACKET_HANDLER(EVENT_TYPE_REST_SIT, zoneEventHandlerRestSit),
    REGISTER_PACKET_HANDLER(EVENT_TYPE_CHAT, zoneEventHandlerChat),
    REGISTER_PACKET_HANDLER(EVENT_TYPE_ENTER_PC, zoneEventHandlerEnterPc),
    REGISTER_PACKET_HANDLER(EVENT_TYPE_JUMP, zoneEventHandlerJump),
    REGISTER_PACKET_HANDLER(EVENT_TYPE_HEAD_ROTATE, zoneEventHandlerHeadRotate),
    REGISTER_PACKET_HANDLER(EVENT_TYPE_ROTATE, zoneEventHandlerRotate),
    REGISTER_PACKET_HANDLER(EVENT_TYPE_LEAVE, zoneEventHandlerLeave),

    #undef REGISTER_PACKET_HANDLER
};

bool zoneEventHandlerEnterPc(EventServer *self, GameEvent *event) {

    bool status = true;
    zlist_t *clientsAround = NULL;
    GameEventEnterPc *enterPcEvent = &event->data.zone.enterPc;

    // update client position
    if (!(zoneEventServerUpdateClientPosition(self, event, &enterPcEvent->updatePosEvent, &clientsAround))) {
        error("Cannot update player %s position.", event->emitterSk);
        status = false;
        goto cleanup;
    }

cleanup:
    zlist_destroy(&clientsAround);
    return status;
}

bool zoneEventHandlerCommanderMove(EventServer *self, GameEvent *event) {

    bool status = true;
    zlist_t *clientsAround = NULL;
    zmsg_t *msg = NULL;
    GameEventCommanderMove *moveEvent = &event->data.zone.move;

    // update client position and get the clients around
    if (!(zoneEventServerUpdateClientPosition(self, event, &moveEvent->updatePosEvent, &clientsAround))) {
        error("Cannot update player %s position.", event->emitterSk);
        status = false;
        goto cleanup;
    }

    if (zlist_size(clientsAround) > 0) {
        // build the packet for the clients around
        msg = zmsg_new();

        zoneBuilderMoveDir(
            moveEvent->updatePosEvent.commanderInfo.pcId,
            &moveEvent->position,
            &moveEvent->direction,
            moveEvent->timestamp,
            msg
        );

        // send the packet
        if (!(eventServerSendToClients(self, clientsAround, msg))) {
            error("Failed to send the packet to the clients.");
            status = false;
            goto cleanup;
        }
    }

cleanup:
    zlist_destroy(&clientsAround);
    zmsg_destroy(&msg);
    return status;
}

bool zoneEventHandlerMoveStop(EventServer *self, GameEvent *event) {

    bool status = true;
    zmsg_t *msg = NULL;
    zlist_t *clientsAround = NULL;
    GameEventMoveStop *moveStopEvent = &event->data.zone.moveStop;

    // update client position and get the clients around
    if (!(zoneEventServerUpdateClientPosition(self, event, &moveStopEvent->updatePosEvent, &clientsAround))) {
        error("Cannot update player %s position.", event->emitterSk);
        status = false;
        goto cleanup;
    }

    if (zlist_size(clientsAround) > 0) {
        // build the packet for the clients around
        msg = zmsg_new ();

        zoneBuilderPcMoveStop(
            moveStopEvent->updatePosEvent.commanderInfo.pcId,
            &moveStopEvent->position,
            &moveStopEvent->direction,
            moveStopEvent->timestamp,
            msg
        );

        // send the packet
        if (!(eventServerSendToClients(self, clientsAround, msg))) {
            error("Failed to send the packet to the clients.");
            status = false;
            goto cleanup;
        }
    }

cleanup:
    zlist_destroy(&clientsAround);
    zmsg_destroy(&msg);
    return status;
}


bool zoneEventHandlerJump(EventServer *self, GameEvent *event) {

    bool status = true;
    zmsg_t *msg = NULL;
    zlist_t *clientsAround = NULL;
    GameEventJump *jumpEvent = &event->data.zone.jump;

    // update client position and get the clients around
    if (!(zoneEventServerUpdateClientPosition(self, event, &jumpEvent->updatePosEvent, &clientsAround))) {
        error("Cannot update player %s position.", event->emitterSk);
        status = false;
        goto cleanup;
    }

    // add itself in the clients list
    zlist_append(clientsAround, event->emitterSk);

    // build the packet for the clients around
    msg = zmsg_new();

    zoneBuilderJump(jumpEvent->updatePosEvent.commanderInfo.pcId, jumpEvent->height, msg);

    // send the packet
    if (!(eventServerSendToClients(self, clientsAround, msg))) {
        error("Failed to send the packet to the clients.");
        status = false;
        goto cleanup;
    }

cleanup:
    zlist_destroy(&clientsAround);
    zmsg_destroy(&msg);
    return status;
}

bool zoneEventHandlerChat(EventServer *self, GameEvent *event) {
    bool status = true;
    zmsg_t *msg = NULL;
    zlist_t *clientsAround = NULL;
    GameEventChat *chatEvent = &event->data.zone.chat;

    // get the clients around
    if (!eventServerGetClientsAround(self, event->emitterSk, &clientsAround)) {
        error("Cannot get clients within range");
        status = false;
        goto cleanup;
    }

    // add itself in the clients list
    zlist_append(clientsAround, event->emitterSk);

    // build the packet for the clients around
    msg = zmsg_new();

    zoneBuilderChat(
        &chatEvent->commander,
        chatEvent->chatText,
        msg
    );

    // send the packet
    if (!(eventServerSendToClients(self, clientsAround, msg))) {
        error("Failed to send the packet to the clients.");
        status = false;
        goto cleanup;
    }

cleanup:
    zlist_destroy(&clientsAround);
    zmsg_destroy(&msg);
    return status;
}

bool zoneEventHandlerRestSit(EventServer *self, GameEvent *event) {
    bool status = true;
    zmsg_t *msg = NULL;
    zlist_t *clientsAround = NULL;
    GameEventRestSit *restSitEvent = &event->data.zone.restSit;

    // Get the clients around
    if (!eventServerGetClientsAround(self, event->emitterSk, &clientsAround)) {
        error("Cannot get clients within range");
        status = false;
        goto cleanup;
    }

    // add itself in the clients list
    zlist_append(clientsAround, event->emitterSk);

    // build the packet for the clients around
    msg = zmsg_new();

    zoneBuilderRestSit(
        restSitEvent->pcId,
        msg
    );

    // send the packet
    if (!(eventServerSendToClients(self, clientsAround, msg))) {
        error("Failed to send the packet to the clients.");
        status = false;
        goto cleanup;
    }

cleanup:
    zlist_destroy(&clientsAround);
    zmsg_destroy(&msg);
    return status;
}

bool zoneEventHandlerHeadRotate(EventServer *self, GameEvent *event) {
    bool status = true;
    zmsg_t *msg = NULL;
    zlist_t *clientsAround = NULL;
    GameEventHeadRotate *headRotateEvent = &event->data.zone.headRotate;

    // Get the clients around
    if (!eventServerGetClientsAround(self, event->emitterSk, &clientsAround)) {
        error("Cannot get clients within range");
        status = false;
        goto cleanup;
    }

    // add itself in the clients list
    zlist_append(clientsAround, event->emitterSk);

    // build the packet for the clients around
    msg = zmsg_new();

    zoneBuilderRotateHead(
        headRotateEvent->pcId,
        &headRotateEvent->direction,
        msg
    );

    // send the packet
    if (!(eventServerSendToClients(self, clientsAround, msg))) {
        error("Failed to send the packet to the clients.");
        status = false;
        goto cleanup;
    }

cleanup:
    zlist_destroy(&clientsAround);
    zmsg_destroy(&msg);
    return status;
}

bool zoneEventHandlerRotate(EventServer *self, GameEvent *event) {
    bool status = true;
    zmsg_t *msg = NULL;
    zlist_t *clientsAround = NULL;
    GameEventRotate *rotateEvent = &event->data.zone.rotate;

    // Get the clients around
    if (!eventServerGetClientsAround(self, event->emitterSk, &clientsAround)) {
        error("Cannot get clients within range");
        status = false;
        goto cleanup;
    }

    // add itself in the clients list
    zlist_append(clientsAround, event->emitterSk);

    // build the packet for the clients around
    msg = zmsg_new();

    zoneBuilderRotate(
        rotateEvent->pcId,
        &rotateEvent->direction,
        msg
    );

    // send the packet
    if (!(eventServerSendToClients(self, clientsAround, msg))) {
        error("Failed to send the packet to the clients.");
        status = false;
        goto cleanup;
    }

cleanup:
    zlist_destroy(&clientsAround);
    zmsg_destroy(&msg);
    return status;
}

bool zoneEventHandlerLeave(EventServer *self, GameEvent *event) {
    bool status = true;
    zmsg_t *msg = NULL;
    zlist_t *clientsAround = NULL;
    GameEventLeave *leaveEvent = &event->data.zone.leave;

    // Get the clients around
    if (!eventServerGetClientsAround(self, event->emitterSk, &clientsAround)) {
        error("Cannot get clients within range");
        status = false;
        goto cleanup;
    }

    // add itself in the clients list
    zlist_append(clientsAround, event->emitterSk);

    // Remove the node from the proximity graph
    eventServerRemoveClient(self, event->emitterSk);

    // build the packet for the clients around
    msg = zmsg_new();

    zoneBuilderLeave(
        leaveEvent->pcId,
        msg
    );

    // send the packet
    if (!(eventServerSendToClients(self, clientsAround, msg))) {
        error("Failed to send the packet to the clients.");
        status = false;
        goto cleanup;
    }

cleanup:
    zlist_destroy(&clientsAround);
    zmsg_destroy(&msg);
    return status;
}
