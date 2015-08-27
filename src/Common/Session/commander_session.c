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
    CommanderInfo *commanderInfo
) {
    CommanderSession *self;

    if ((self = calloc (1, sizeof (CommanderSession))) == NULL) {
        return NULL;
    }

    if (!commanderSessionInit (self, commanderInfo)) {
        commanderSessionDestroy (&self);
        error ("CommanderSession failed to initialize.");
        return NULL;
    }

    return self;
}

bool
commanderSessionInit (
    CommanderSession *self,
    CommanderInfo *commanderInfo
) {
    memset (self, 0, sizeof (CommanderSession));

    memcpy (&self->currentCommander, commanderInfo, sizeof (CommanderInfo));

    return true;
}

void
commanderSessionPrint (
    CommanderSession *self
) {
    dbg ("==== CommanderSession %p ====", self);
    commanderInfoPrint (&self->currentCommander);
}

void
commanderSessionDestroy (
    CommanderSession **_self
) {
    commanderSessionFree (*_self);
    *_self = NULL;
}

void
commanderSessionFree (
    CommanderSession *self
) {
    free (self);
}
