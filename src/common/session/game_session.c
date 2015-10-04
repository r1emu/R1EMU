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

GameSession *gameSessionNew(Commander *commander) {

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

bool gameSessionInit(GameSession *self, Commander *commander) {

    memset(self, 0, sizeof(GameSession));

    if (!(accountSessionInit(&self->accountSession, "undefined", "undefined", ACCOUNT_SESSION_PRIVILEGES_PLAYER))) {
        error("Cannot initialize the account session.");
        return false;
    }

    if (!(commanderSessionInit(&self->commanderSession, commander))) {
        error("Cannot initialize the commander session.");
        return false;
    }

    return true;
}

void gameSessionPrint(GameSession *self) {

    dbg("==== GameSession %p ====", self);
    accountSessionPrint(&self->accountSession);
    commanderSessionPrint (&self->commanderSession);
}

void gameSessionDestroy(GameSession **_self) {
    gameSessionFree (*_self);
    *_self = NULL;
}

void gameSessionFree(GameSession *self) {
    free(self);
}
