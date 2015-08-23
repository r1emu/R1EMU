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
#include "EventHandler.h"
#include "ZoneServer/ZoneHandler/ZoneBuilder.h"

// ------ Structure declaration -------


// ------ Static declaration -------


// ------ Extern function implementation -------

bool
EventHandler_enterPc (
    EventServer *self,
    GameEventPcEnter *event
) {
    bool status = true;
    zlist_t *clientsAround = NULL;

    // Update client position
    if (!(EventServer_updateClientPosition (self, &event->updatePosEvent, &event->updatePosEvent.commanderInfo.pos, &clientsAround))) {
        error ("Cannot update player %s position.", event->updatePosEvent.sessionKey);
        status = false;
        goto cleanup;
    }

cleanup:
    zlist_destroy (&clientsAround);
    return status;
}


bool
EventHandler_commanderMove (
    EventServer *self,
    GameEventCommanderMove *event
) {
    bool status = true;
    zlist_t *clientsAround = NULL;
    zmsg_t *msg = NULL;

    // Update client position and get the clients around
    if (!(EventServer_updateClientPosition (self, &event->updatePosEvent, &event->position, &clientsAround))) {
        error ("Cannot update player %s position.", event->updatePosEvent.sessionKey);
        status = false;
        goto cleanup;
    }

    if (zlist_size (clientsAround) > 0)
    {
        // Build the packet for the clients around
        msg = zmsg_new ();
        ZoneBuilder_moveDir (
            event->updatePosEvent.commanderInfo.pcId,
            &event->position,
            &event->direction,
            event->timestamp,
            msg
        );

        // Send the packet
        zframe_t *frame = zmsg_first (msg);
        if (!(EventServer_sendToClients (self, clientsAround, zframe_data (frame), zframe_size (frame)))) {
            error ("Failed to send the packet to the clients.");
            status = false;
            goto cleanup;
        }
    }

cleanup:
    zlist_destroy (&clientsAround);
    zmsg_destroy (&msg);
    return status;
}

bool
EventHandler_moveStop (
    EventServer *self,
    GameEventMoveStop *event
) {
    bool status = true;
    zmsg_t *msg = NULL;
    zlist_t *clientsAround = NULL;

    // Update client position and get the clients around
    if (!(EventServer_updateClientPosition (self, &event->updatePosEvent, &event->position, &clientsAround))) {
        error ("Cannot update player %s position.", event->updatePosEvent.sessionKey);
        status = false;
        goto cleanup;
    }

    if (zlist_size (clientsAround) > 0)
    {
        // Build the packet for the clients around
        msg = zmsg_new ();
        ZoneBuilder_pcMoveStop (
            event->updatePosEvent.commanderInfo.pcId,
            &event->position,
            &event->direction,
            event->timestamp,
            msg
        );

        // Send the packet
        zframe_t *frame = zmsg_first (msg);
        if (!(EventServer_sendToClients (self, clientsAround, zframe_data (frame), zframe_size (frame)))) {
            error ("Failed to send the packet to the clients.");
            status = false;
            goto cleanup;
        }
    }

cleanup:
    zlist_destroy (&clientsAround);
    zmsg_destroy (&msg);
    return status;
}


bool
EventHandler_jump (
    EventServer *self,
    GameEventJump *event
) {
    bool status = true;
    zmsg_t *msg = NULL;
    zlist_t *clientsAround = NULL;

    // Update client position and get the clients around
    if (!(EventServer_updateClientPosition (self, &event->updatePosEvent, &event->updatePosEvent.commanderInfo.pos, &clientsAround))) {
        error ("Cannot update player %s position.", event->updatePosEvent.sessionKey);
        status = false;
        goto cleanup;
    }

    // Add itself in the clients list
    zlist_append (clientsAround, event->updatePosEvent.sessionKey);

    // Build the packet for the clients around
    msg = zmsg_new ();
    ZoneBuilder_jump (
        event->updatePosEvent.commanderInfo.pcId,
        event->height,
        msg
    );

    // Send the packet
    zframe_t *frame = zmsg_first (msg);
    if (!(EventServer_sendToClients (self, clientsAround, zframe_data (frame), zframe_size (frame)))) {
        error ("Failed to send the packet to the clients.");
        status = false;
        goto cleanup;
    }

cleanup:
    zlist_destroy (&clientsAround);
    zmsg_destroy (&msg);
    return status;
}

bool
EventHandler_chat (
    EventServer *self,
    GameEventChat *event
) {
    bool status = true;
    zmsg_t *msg = NULL;
    zlist_t *clientsAround = NULL;

    // Get the clients around
    if (!EventServer_getClientsAround (self, event->sessionKey, &clientsAround)) {
        error ("Cannot get clients within range");
        status = false;
        goto cleanup;
    }

    // Add itself in the clients list
    zlist_append (clientsAround, event->sessionKey);

    // Build the packet for the clients around
    msg = zmsg_new ();
    ZoneBuilder_chat (
        event->pcId,
        event->familyName,
        event->commanderName,
        event->chatText,
        msg
    );
    // Send the packet
    zframe_t *frame = zmsg_first (msg);
    if (!(EventServer_sendToClients (self, clientsAround, zframe_data (frame), zframe_size (frame)))) {
        error ("Failed to send the packet to the clients.");
        status = false;
        goto cleanup;
    }

cleanup:
    zlist_destroy (&clientsAround);
    zmsg_destroy (&msg);
    return status;
}

bool
EventHandler_restSit (
    EventServer *self,
    GameEventRestSit *event
) {
    bool status = true;
    zmsg_t *msg = NULL;
    zlist_t *clientsAround = NULL;

    // Get the clients around
    if (!EventServer_getClientsAround (self, event->sessionKey, &clientsAround)) {
        error ("Cannot get clients within range");
        status = false;
        goto cleanup;
    }

    // Add itself in the clients list
    zlist_append (clientsAround, event->sessionKey);

    // Build the packet for the clients around
    msg = zmsg_new ();
    ZoneBuilder_restSit (
        event->pcId,
        msg
    );

    // Send the packet
    zframe_t *frame = zmsg_first (msg);
    if (!(EventServer_sendToClients (self, clientsAround, zframe_data (frame), zframe_size (frame)))) {
        error ("Failed to send the packet to the clients.");
        status = false;
        goto cleanup;
    }

cleanup:
    zlist_destroy (&clientsAround);
    zmsg_destroy (&msg);
    return status;
}
