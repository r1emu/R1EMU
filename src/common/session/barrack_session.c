/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file BarrackSession.h
 * @brief
 *
 *
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

// ---------- Includes ------------
#include "barrack_session.h"

BarrackSession *
barrackSessionNew (

) {
    BarrackSession *self;

    if ((self = calloc (1, sizeof (BarrackSession))) == NULL) {
        return NULL;
    }

    if (!barrackSessionInit (self)) {
        barrackSessionDestroy (&self);
        error ("BarrackSession failed to initialize.");
        return NULL;
    }

    return self;
}

bool
barrackSessionInit (
    BarrackSession *self
) {
    memset (self, 0, sizeof (BarrackSession));

    self->charactersCreatedCount = 0;

    return true;
}

void
barrackSessionPrint (
    BarrackSession *self
) {
    dbg ("==== BarrackSession %p ====", self);
}

void
barrackSessionDestroy (
    BarrackSession **_self
) {
    barrackSessionFree (*_self);
    *_self = NULL;
}

void
barrackSessionFree (
    BarrackSession *self
) {
    free (self);
}
