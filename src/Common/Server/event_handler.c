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

#include "event_handler.h"
#include "zone_server/zone_handler/zone_builder.h"

bool eventHandlerEnterPc(EventServer *self, GameEventPcEnter *event) {
    bool status = true;
    zlist_t *clientsAround = NULL;

    // update client position
    if (!(eventServerUpdateClientPosition(self, &event->updatePosEvent, &event->updatePosEvent.commanderInfo.pos, &clientsAround))) {
        error("Cannot update player %s position.", event->updatePosEvent.sessionKey);
        status = false;
        goto cleanup;
    }

cleanup:
    zlist_destroy(&clientsAround);
    return status;
}


bool eventHandlerCommanderMove(EventServer *self, GameEventCommanderMove *event) {
    bool status = true;
    zlist_t *clientsAround = NULL;
    zmsg_t *msg = NULL;

    // update client position and get the clients around
    if (!(eventServerUpdateClientPosition(self, &event->updatePosEvent, &event->position, &clientsAround))) {
        error("Cannot update player %s position.", event->updatePosEvent.sessionKey);
        status = false;
        goto cleanup;
    }

    if (zlist_size(clientsAround) > 0) {
        // build the packet for the clients around
        msg = zmsg_new();

        zoneBuilderMoveDir(
            event->updatePosEvent.commanderInfo.pcId,
            &event->position,
            &event->direction,
            event->timestamp,
            msg
        );

        // send the packet
        zframe_t *frame = zmsg_first(msg);

        if (!(eventServerSendToClients(self, clientsAround, zframe_data(frame), zframe_size(frame)))) {
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

bool eventHandlerMoveStop(EventServer *self, GameEventMoveStop *event) {
    bool status = true;
    zmsg_t *msg = NULL;
    zlist_t *clientsAround = NULL;

    // update client position and get the clients around
    if (!(eventServerUpdateClientPosition(self, &event->updatePosEvent, &event->position, &clientsAround))) {
        error("Cannot update player %s position.", event->updatePosEvent.sessionKey);
        status = false;
        goto cleanup;
    }

    if (zlist_size(clientsAround) > 0) {
        // build the packet for the clients around
        msg = zmsg_new ();

        zoneBuilderPcMoveStop(
            event->updatePosEvent.commanderInfo.pcId,
            &event->position,
            &event->direction,
            event->timestamp,
            msg
        );

        // send the packet
        zframe_t *frame = zmsg_first(msg);

        if (!(eventServerSendToClients(self, clientsAround, zframe_data(frame), zframe_size(frame)))) {
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


bool eventHandlerJump(EventServer *self, GameEventJump *event) {
    bool status = true;
    zmsg_t *msg = NULL;
    zlist_t *clientsAround = NULL;

    // update client position and get the clients around
    if (!(eventServerUpdateClientPosition(self, &event->updatePosEvent, &event->updatePosEvent.commanderInfo.pos, &clientsAround))) {
        error("Cannot update player %s position.", event->updatePosEvent.sessionKey);
        status = false;
        goto cleanup;
    }

    // add itself in the clients list
    zlist_append(clientsAround, event->updatePosEvent.sessionKey);

    // build the packet for the clients around
    msg = zmsg_new();

    zoneBuilderJump(
        event->updatePosEvent.commanderInfo.pcId,
        event->height,
        msg
    );

    // send the packet
    zframe_t *frame = zmsg_first(msg);

    if (!(eventServerSendToClients(self, clientsAround, zframe_data(frame), zframe_size(frame)))) {
        error("Failed to send the packet to the clients.");
        status = false;
        goto cleanup;
    }

cleanup:
    zlist_destroy(&clientsAround);
    zmsg_destroy(&msg);
    return status;
}

bool eventHandlerChat(EventServer *self, GameEventChat *event) {
    bool status = true;
    zmsg_t *msg = NULL;
    zlist_t *clientsAround = NULL;

    // get the clients around
    if (!eventServerGetClientsAround(self, event->sessionKey, &clientsAround)) {
        error("Cannot get clients within range");
        status = false;
        goto cleanup;
    }

    // add itself in the clients list
    zlist_append(clientsAround, event->sessionKey);

    // build the packet for the clients around
    msg = zmsg_new();

    zoneBuilderChat(
        event->pcId,
        event->familyName,
        event->commanderName,
        event->chatText,
        msg
    );
    // Send the packet
    zframe_t *frame = zmsg_first (msg);

    if (!(eventServerSendToClients (self, clientsAround, zframe_data(frame), zframe_size (frame)))) {
        error ("Failed to send the packet to the clients.");
        status = false;
        goto cleanup;
    }

cleanup:
    zlist_destroy(&clientsAround);
    zmsg_destroy(&msg);
    return status;
}

bool eventHandlerRestSit(EventServer *self, GameEventRestSit *event) {
    bool status = true;
    zmsg_t *msg = NULL;
    zlist_t *clientsAround = NULL;

    // Get the clients around
    if (!eventServerGetClientsAround(self, event->sessionKey, &clientsAround)) {
        error("Cannot get clients within range");
        status = false;
        goto cleanup;
    }

    // add itself in the clients list
    zlist_append(clientsAround, event->sessionKey);

    // build the packet for the clients around
    msg = zmsg_new();

    zoneBuilderRestSit(
        event->pcId,
        msg
    );

    // send the packet
    zframe_t *frame = zmsg_first (msg);

    if (!(eventServerSendToClients (self, clientsAround, zframe_data(frame), zframe_size (frame)))) {
        error("Failed to send the packet to the clients.");
        status = false;
        goto cleanup;
    }

cleanup:
    zlist_destroy(&clientsAround);
    zmsg_destroy(&msg);
    return status;
}
