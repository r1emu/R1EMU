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

#include "npc.h"

Npc *npcNew(Actor *actor) {
    Npc *self;

    if ((self = malloc(sizeof(Npc))) == NULL) {
        return NULL;
    }

    if (!npcInit(self, actor)) {
        npcDestroy(&self);
        error("Npc failed to initialize.");
        return NULL;
    }

    return self;
}

bool npcInit(Npc *self, Actor *actor) {
    memset(self, 0, sizeof(*self));

    self->actor = *actor;

    return true;
}

void npcFree(Npc *self) {
    // TODO
}

void npcDestroy(Npc **_self) {
    Npc *self = *_self;

    if (_self && self) {
        npcFree(self);
        free(self);
        *_self = NULL;
    }
}
