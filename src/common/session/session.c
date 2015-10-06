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

#include "session.h"

Session *sessionNew (RouterId_t routerId, uint8_t *sessionKey) {
    Session *self;

    if ((self = calloc(1, sizeof(Session))) == NULL) {
        return NULL;
    }

    if (!sessionInit(self, routerId, sessionKey)) {
        sessionDestroy(&self);
        error("Session failed to initialize.");
        return NULL;
    }

    return self;
}

bool sessionInit(Session *self, RouterId_t routerId, uint8_t *sessionKey) {

    // Define a valid socket session
    socketSessionInit(&self->socket,
        SOCKET_SESSION_UNDEFINED_ACCOUNT,
        routerId,
        SOCKET_SESSION_UNDEFINED_MAP,
        sessionKey,
        false);

    // Initialize a dummy commander info
    Commander commander;
    commanderInit(&commander);
    gameSessionInit(&self->game, &commander);

    return true;
}

void sessionDestroy(Session **_self) {
    Session *self = *_self;

    free(self);
    *_self = NULL;
}
