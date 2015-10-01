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

#include "equipable_item.h"

// Inlined functions
extern inline ItemId_t equipableItemGetId(EquipableItem *self);
extern inline ItemId_t equipableItemGetUId(EquipableItem *self);
extern inline int equipableItemGetSlot(EquipableItem *self);

EquipableItem *equipableItemNew(void) {
    EquipableItem *self;

    if ((self = malloc(sizeof(EquipableItem))) == NULL) {
        return NULL;
    }

    if (!equipableItemInit(self)) {
        equipableItemDestroy(&self);
        error("EquipableItem failed to initialize.");
        return NULL;
    }

    return self;
}

bool equipableItemInit(EquipableItem *self) {
    memset(self, 0, sizeof(*self));

    return true;
}

void equipableItemFree(EquipableItem *self) {
    // TODO
}

void equipableItemDestroy(EquipableItem **_self) {
    EquipableItem *self = *_self;

    if (_self && self) {
        equipableItemFree(self);
        free(self);
        *_self = NULL;
    }
}
