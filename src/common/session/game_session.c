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
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

// ---------- Includes ------------
#include "game_session.h"

GameSession *
gameSessionNew (
    CommanderInfo *commander
) {
    GameSession *self;

    if ((self = calloc(1, sizeof(GameSession))) == NULL) {
        return NULL;
    }

    if (!gameSessionInit (self, commander)) {
        gameSessionDestroy (&self);
        error("GameSession failed to initialize.");
        return NULL;
    }

    return self;
}

bool
gameSessionInit (
    GameSession *self,
    CommanderInfo *commander
) {
    memset(self, 0, sizeof(GameSession));

    commanderSessionInit (&self->commanderSession, commander);
    barrackSessionInit (&self->barrackSession);

    return true;
}

void
gameSessionPrint (
    GameSession *self
) {
    dbg("==== GameSession %p ====", self);
    barrackSessionPrint (&self->barrackSession);
    commanderSessionPrint (&self->commanderSession);
}

void
gameSessionDestroy (
    GameSession **_self
) {
    gameSessionFree (*_self);
    *_self = NULL;
}

void
gameSessionFree (
    GameSession *self
) {
    free(self);
}
