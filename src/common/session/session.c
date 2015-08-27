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

Session *sessionNew (void) {
    Session *self;

    if ((self = calloc(1, sizeof(Session))) == NULL) {
        return NULL;
    }

    if (!sessionInit(self)) {
        sessionDestroy(&self);
        error("Session failed to initialize.");
        return NULL;
    }

    return self;
}

bool sessionInit(Session *self) {
    return true;
}

void sessionDestroy(Session **_self) {
    Session *self = *_self;

    free(self);
    *_self = NULL;
}
