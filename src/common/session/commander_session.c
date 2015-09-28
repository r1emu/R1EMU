/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file CommanderSession.h
 * @brief
 *
 *
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

// ---------- Includes ------------
#include "commander_session.h"

CommanderSession *
commanderSessionNew (
    Commander *commander
) {
    CommanderSession *self;

    if ((self = calloc(1, sizeof(CommanderSession))) == NULL) {
        return NULL;
    }

    if (!commanderSessionInit (self, commander)) {
        commanderSessionDestroy (&self);
        error("CommanderSession failed to initialize.");
        return NULL;
    }

    return self;
}

bool
commanderSessionInit (
    CommanderSession *self,
    Commander *commander
) {
    memset(self, 0, sizeof(CommanderSession));

    memcpy(&self->currentCommander, commander, sizeof(self->currentCommander));

    return true;
}

void
commanderSessionPrint (
    CommanderSession *self
) {
    dbg("==== CommanderSession %p ====", self);
    commanderPrint (self->currentCommander);
}

void
commanderSessionDestroy (
    CommanderSession **_self
) {
    commanderSessionFree (*_self);
    free(*_self);
    *_self = NULL;
}

void
commanderSessionFree (
    CommanderSession *self
) {
}


size_t commanderSessionGetPacketSize(CommanderSession *self) {
    size_t packetSize = 0;

    packetSize += sizeof(CommanderSessionPacket);

    return packetSize;
}

void commanderSessionSPacket(CommanderSession *self, PacketStream *stream) {

}

