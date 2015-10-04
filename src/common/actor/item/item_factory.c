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

#include "item_factory.h"
#include "common/actor/actor_factory.h"

struct ItemFactory {
} itemFactory = {
};

// TODO : ItemId_t should be enough to know the correct category
// Once StaticData is available, link them together
Item *itemFactoryCreate(ItemCategory category, ItemId_t id, ItemAmount_t amount) {

    Item *item = NULL;

    if (!(item = malloc(sizeof(Item)))) {
        error("Cannot allocate a new item.");
        return NULL;
    }

    if (!(itemFactoryInit(item, category, id, amount))) {
        error("Cannot initialize a new item.");
        free(item);
        return NULL;
    }

    return item;
}

bool itemFactoryInit(Item *item, ItemCategory category, ItemId_t id, ItemAmount_t amount) {

    Actor actor;

    if (!(actorFactoryInit(&actor))) {
        error("Cannot initialize a new actor.");
        return false;
    }

    if (!(itemInit(item, &actor, category, id, amount))) {
        error("Cannot create a new item '%d'.", id);
        return false;
    }

    return true;
}
