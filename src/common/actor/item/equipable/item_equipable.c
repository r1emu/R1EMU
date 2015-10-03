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

#include "item_equipable.h"

// Inlined functions
extern inline int equipableItemGetSlot(ItemEquipable *self);

ItemEquipable *equipableItemNew(void) {
    ItemEquipable *self;

    if ((self = malloc(sizeof(ItemEquipable))) == NULL) {
        return NULL;
    }

    if (!equipableItemInit(self)) {
        equipableItemDestroy(&self);
        error("ItemEquipable failed to initialize.");
        return NULL;
    }

    return self;
}

bool equipableItemInit(ItemEquipable *self) {
    memset(self, 0, sizeof(*self));

    return true;
}

void equipableItemFree(ItemEquipable *self) {
    // TODO
}

void equipableItemDestroy(ItemEquipable **_self) {
    ItemEquipable *self = *_self;

    if (_self && self) {
        equipableItemFree(self);
        free(self);
        *_self = NULL;
    }
}
