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
extern inline int itemEquipableGetSlot(ItemEquipable *self);

ItemEquipable *itemEquipableNew(Item *item) {
    ItemEquipable *self;

    if ((self = malloc(sizeof(ItemEquipable))) == NULL) {
        return NULL;
    }

    if (!itemEquipableInit(self, item)) {
        itemEquipableDestroy(&self);
        error("ItemEquipable failed to initialize.");
        return NULL;
    }

    return self;
}

bool itemEquipableInit(ItemEquipable *self, Item *item) {
    memset(self, 0, sizeof(*self));

    self->item = *item;
    self->slot = EQSLOT_NOSLOT;

    return true;
}

void itemEquipableFree(ItemEquipable *self) {
    itemFree(&self->item);
}

void itemEquipableDestroy(ItemEquipable **_self) {
    ItemEquipable *self = *_self;

    if (_self && self) {
        itemEquipableFree(self);
        free(self);
        *_self = NULL;
    }
}
