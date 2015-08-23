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
#include "CommanderSession.h"

CommanderSession *
CommanderSession_new (
    CommanderInfo *commanderInfo
) {
    CommanderSession *self;

    if ((self = calloc (1, sizeof (CommanderSession))) == NULL) {
        return NULL;
    }

    if (!CommanderSession_init (self, commanderInfo)) {
        CommanderSession_destroy (&self);
        error ("CommanderSession failed to initialize.");
        return NULL;
    }

    return self;
}

bool
CommanderSession_init (
    CommanderSession *self,
    CommanderInfo *commanderInfo
) {
    memset (self, 0, sizeof (CommanderSession));

    memcpy (&self->currentCommander, commanderInfo, sizeof (CommanderInfo));

    return true;
}

void
CommanderSession_print (
    CommanderSession *self
) {
    dbg ("==== CommanderSession %p ====", self);
    CommanderInfo_print (&self->currentCommander);
}

void
CommanderSession_destroy (
    CommanderSession **_self
) {
    CommanderSession_free (*_self);
    *_self = NULL;
}

void
CommanderSession_free (
    CommanderSession *self
) {
    free (self);
}
