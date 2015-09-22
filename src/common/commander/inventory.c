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


// ---------- Includes ------------
#include "inventory.h"


// ------ Structure declaration -------

// ------ Static declaration -------


// ------ Extern function implementation -------

Inventory* inventoryNew(void) {
    Inventory *self;

    if ((self = malloc(sizeof(Inventory))) == NULL) {
        return NULL;
    }

    if (!inventoryInit(self)) {
        inventoryDestroy(&self);
        error("Inventory failed to initialize.");
        return NULL;
    }

    return self;
}

bool inventoryInit(Inventory *self) {
    memset(self, 0, sizeof(Inventory));

    // Initialize hash for items storage
    if (!(self->items = zhash_new())) {
        error("Cannot allocate hashtable for items.");
        return false;
    }

    return true;
}

void inventoryFree(Inventory *self) {
    // Destroy hashtable of items
    zhash_destroy (&self->items);
}

void inventoryDestroy(Inventory **_self) {
    Inventory *self = *_self;

    if (_self && self) {
        inventoryFree(self);
        free(self);
        *_self = NULL;
    }
}

bool inventoryAddItem(Inventory *self, Item *itemToAdd) {

    dbg("itemIdKey: %d", itemToAdd->itemId);

    char itemIdKey[17];
    itemGenKey(itemToAdd, itemIdKey);

    dbg("itemIdKey: %s", itemIdKey);

    if (self->items == NULL) {
        self->items = zhash_new();
    }

    int result;
    result = zhash_insert(self->items, itemIdKey, itemToAdd);
    if (result != 0) {
        error("Cannot insert the item in the hashtable. Error: %d", result);
        return false;
    }

    return true;
}

bool inventoryRemoveItem(Inventory *self, Item *itemToRemove) {

    char itemIdKey[17];
    itemGenKey(itemToAdd, itemIdKey);

    zhash_delete(self->items, itemIdKey);

    return true;
}

bool inventoryGetItemByItemId(Inventory *self, uint64_t itemId, Item **_item) {

    Item *item = NULL;

    char itemIdKey[17];
    itemGenKey(itemToAdd, itemIdKey);

    *_item = NULL;

    if (!(item = zhash_lookup(self->items, itemIdKey))) {
        error("Cannot find the item '%s' in the inventory.", itemIdKey);
        return false;
    }

    *_item = item;

    return true;
}

size_t inventoryGetItemsCount(Inventory *self) {
    return zhash_size(self->items);
}

bool inventoryUnequipItem(Inventory *self, EquipmentSlot eqSlot) {

    Item *itemToUnequip = self->equippedItems[eqSlot];

    if (itemToUnequip == NULL) {
        dbg("No item to unequip. Slot is free.");
        return true; // Should I return false? Function didn't success to unequip.
    }

    if (!inventoryAddItem(self, itemToUnequip)) {
        dbg("Item not unequipped, since it can't be added to Inventory.");
        return false;
    }

    // Set slot as free
    self->equippedItems[eqSlot] = NULL;

    // Process unequip events
    /// TODO


    return true;
}

bool inventoryEquipItem(Inventory *self, uint64_t itemId, EquipmentSlot eqSlot) {

    Item *itemToEquip;

    // Get item from inventory
    if (!inventoryGetItemByItemId(self, itemId, &itemToEquip)) {
        dbg("Can not find item in inventory. ItemId: %d", itemId);
        return false;
    }

    // Check if eqSlot is right for the item we want to equip.
    /// TODO

    // Check if there is the slot already, if so, unequip.
    if (self->equippedItems[eqSlot] != NULL) {
        // Unequip Item
        /// TODO
        if (!inventoryUnequipItem(self, eqSlot)) {
            dbg("No possible to unequip item from slot %d", eqSlot);
            return false;
        }
    }

    // Now we have the slot free to equip this item, and item is not in inventory.
    self->equippedItems[eqSlot] = itemToEquip;

    // Remove from inventory
    if (!inventoryRemoveItem(self, itemToEquip)) {
        dbg("Can't remove item from inventory. Equip Item failed.");
        // Roll back equipped item
        self->equippedItems[eqSlot] = NULL;
        return false;
    }

    // Process Equipment events
    /// TODO

    return true;

}

Item *inventoryGetEquipment(Inventory *self) {
    return (Item*) self->equippedItems;
}

bool inventoryGetEquipmentEmptySlot(EquipmentSlot slot, uint32_t *value) {

    switch (slot) {
        case EQSLOT_HEAD_TOP: *value = EMPTYEQSLOT_NoHat; break;
        case EQSLOT_HEAD_MIDDLE: *value = EMPTYEQSLOT_NoHat; break;
        case EQSLOT_UNKOWN1: *value = EMPTYEQSLOT_NoOuter; break;
        case EQSLOT_BODY_ARMOR: *value = EMPTYEQSLOT_NoHat; break;
        case EQSLOT_GLOVES: *value = EMPTYEQSLOT_NoGloves; break;
        case EQSLOT_BOOTS: *value = EMPTYEQSLOT_NoBoots; break;
        case EQSLOT_HELMET: *value = EMPTYEQSLOT_NoHelmet; break;
        case EQSLOT_BRACELET: *value = EMPTYEQSLOT_NoArmband; break;
        case EQSLOT_WEAPON: *value = EMPTYEQSLOT_NoWeapon; break;
        case EQSLOT_SHIELD: *value = EMPTYEQSLOT_NoWeapon; break;
        case EQSLOT_COSTUME: *value = EMPTYEQSLOT_NoOuter; break;
        case EQSLOT_UNKOWN3: *value = EMPTYEQSLOT_NoRing; break;
        case EQSLOT_UNKOWN4: *value = EMPTYEQSLOT_NoRing; break;
        case EQSLOT_UNKOWN5: *value = EMPTYEQSLOT_NoOuter; break;
        case EQSLOT_LEG_ARMOR: *value = EMPTYEQSLOT_NoShirt; break;
        case EQSLOT_UNKOWN6: *value = EMPTYEQSLOT_NoRing; break;
        case EQSLOT_UNKOWN7: *value = EMPTYEQSLOT_NoRing; break;
        case EQSLOT_RIGHT_LEFT: *value = EMPTYEQSLOT_NoRing; break;
        case EQSLOT_RIGHT_RIGHT: *value = EMPTYEQSLOT_NoRing; break;
        case EQSLOT_NECKLACE: *value = EMPTYEQSLOT_NoNeck; break;
        default: {
            *value = -1;
            return false;
        }
    }
    return true;
}


void inventoryPrintEquipment(Inventory *self) {
    dbg("head_top = %d (%x)", self->equippedItems[0] ? self->equippedItems[0]->itemType : 0, self->equippedItems[0]);
    dbg("head_middle = %d (%x)", self->equippedItems[1] ? self->equippedItems[1]->itemType : 0, self->equippedItems[1]);
    dbg("itemUnk1 = %d (%x)", self->equippedItems[2] ? self->equippedItems[2]->itemType : 0, self->equippedItems[2]);
    dbg("body_armor = %d (%x)", self->equippedItems[3] ? self->equippedItems[3]->itemType : 0, self->equippedItems[3]);
    dbg("gloves = %d (%x)", self->equippedItems[4] ? self->equippedItems[4]->itemType : 0, self->equippedItems[4]);
    dbg("boots = %d (%x)", self->equippedItems[5] ? self->equippedItems[5]->itemType : 0, self->equippedItems[5]);
    dbg("helmet = %d (%x)", self->equippedItems[6] ? self->equippedItems[6]->itemType : 0, self->equippedItems[6]);
    dbg("bracelet = %d (%x)", self->equippedItems[7] ? self->equippedItems[7]->itemType : 0, self->equippedItems[7]);
    dbg("weapon = %d (%x)", self->equippedItems[8] ? self->equippedItems[8]->itemType : 0, self->equippedItems[8]);
    dbg("shield = %d (%x)", self->equippedItems[9] ? self->equippedItems[9]->itemType : 0, self->equippedItems[9]);
    dbg("costume = %d (%x)", self->equippedItems[10] ? self->equippedItems[10]->itemType : 0, self->equippedItems[10]);
    dbg("itemUnk3 = %d (%x)", self->equippedItems[11] ? self->equippedItems[11]->itemType : 0, self->equippedItems[11]);
    dbg("itemUnk4 = %d (%x)", self->equippedItems[12] ? self->equippedItems[12]->itemType : 0, self->equippedItems[12]);
    dbg("itemUnk5 = %d (%x)", self->equippedItems[13] ? self->equippedItems[13]->itemType : 0, self->equippedItems[13]);
    dbg("leg_armor = %d (%x)", self->equippedItems[14] ? self->equippedItems[14]->itemType : 0, self->equippedItems[14]);
    dbg("itemUnk6 = %d (%x)", self->equippedItems[15] ? self->equippedItems[15]->itemType : 0, self->equippedItems[15]);
    dbg("itemUnk7 = %d (%x)", self->equippedItems[16] ? self->equippedItems[16]->itemType : 0, self->equippedItems[16]);
    dbg("ring_left = %d (%x)", self->equippedItems[17] ? self->equippedItems[17]->itemType : 0, self->equippedItems[17]);
    dbg("ring_right = %d (%x)", self->equippedItems[18] ? self->equippedItems[18]->itemType : 0, self->equippedItems[18]);
    dbg("necklace = %d (%x)", self->equippedItems[19] ? self->equippedItems[19]->itemType : 0, self->equippedItems[19]);
}
