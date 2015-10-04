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

#include "actor.h"

// Inlined functions
extern inline ActorId_t actorGetUId(void *self);

Actor *actorNew(ActorId_t uid) {
    Actor *self;

    if ((self = malloc(sizeof(Actor))) == NULL) {
        return NULL;
    }

    if (!actorInit(self, uid)) {
        actorDestroy(&self);
        error("Actor failed to initialize.");
        return NULL;
    }

    return self;
}

void actorFree(Actor *self) {
    // TODO
}

void actorDestroy(Actor **_self) {
    Actor *self = *_self;

    if (_self && self) {
        actorFree(self);
        free(self);
        *_self = NULL;
    }
}

bool actorInit(Actor *self, ActorId_t uid) {
    self->uid = uid;
    return true;
}

void actorGenKey(ActorId_t uid, ActorKey key) {
    snprintf(key, ACTOR_KEY_SIZE, "%I64x", uid);
}
