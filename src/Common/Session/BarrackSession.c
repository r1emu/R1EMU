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
#include "BarrackSession.h"

BarrackSession *
BarrackSession_new (

) {
    BarrackSession *self;

    if ((self = calloc (1, sizeof (BarrackSession))) == NULL) {
        return NULL;
    }

    if (!BarrackSession_init (self)) {
        BarrackSession_destroy (&self);
        error ("BarrackSession failed to initialize.");
        return NULL;
    }

    return self;
}

bool
BarrackSession_init (
    BarrackSession *self
) {
    memset (self, 0, sizeof (BarrackSession));

    self->charactersCreatedCount = 0;

    return true;
}

void
BarrackSession_print (
    BarrackSession *self
) {
    dbg ("==== BarrackSession %p ====", self);
}

void
BarrackSession_destroy (
    BarrackSession **_self
) {
    BarrackSession_free (*_self);
    *_self = NULL;
}

void
BarrackSession_free (
    BarrackSession *self
) {
    free (self);
}
