/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file GameSession.h
 * @brief
 *
 *
 *
 * @license <license placeholder>
 */

// ---------- Includes ------------
#include "GameSession.h"

GameSession *
GameSession_new (
    CommanderInfo *commanderInfo
) {
    GameSession *self;

    if ((self = calloc (1, sizeof (GameSession))) == NULL) {
        return NULL;
    }

    if (!GameSession_init (self, commanderInfo)) {
        GameSession_destroy (&self);
        error ("GameSession failed to initialize.");
        return NULL;
    }

    return self;
}

bool
GameSession_init (
    GameSession *self,
    CommanderInfo *commanderInfo
) {
    memset (self, 0, sizeof (GameSession));

    CommanderSession_init (&self->commanderSession, commanderInfo);
    BarrackSession_init (&self->barrackSession);

    return true;
}

void
GameSession_print (
    GameSession *self
) {
    dbg ("==== GameSession %p ====", self);
    BarrackSession_print (&self->barrackSession);
    CommanderSession_print (&self->commanderSession);
}

void
GameSession_destroy (
    GameSession **_self
) {
    GameSession_free (*_self);
    *_self = NULL;
}

void
GameSession_free (
    GameSession *self
) {
    free (self);
}
