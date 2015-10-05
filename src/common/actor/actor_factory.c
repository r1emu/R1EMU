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

#include "actor_factory.h"

static struct ActorFactory {
    uint64_t id;
} self = {
    .id = 0
};

Actor *actorFactoryCreate() {

    Actor *actor = NULL;

    if (!(actor = malloc(sizeof(Actor)))) {
        error("Cannot allocate a new actor.");
        return NULL;
    }

    if (!(actorFactoryInit(actor))) {
        error("Cannot initialize a new actor.");
        free(actor);
        return NULL;
    }

    return actor;
}

bool actorFactoryInit(Actor *actor) {

    if (!(actorInit(actor, self.id))) {
        error("Cannot initialize a new actor.");
        return false;
    }

    self.id++;
    return true;
}
