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

    for (int i = 0; i < INVENTORY_CAT_Count; i++) {
        if (!(self->bags[i] = zlist_new())) {
            error("Cannot allocate list for bag [%d]", i);
            return false;
        }
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

    if (!itemToAdd->itemCategory) {
        error("Item has no category");
        return false;
    }

    if (itemToAdd->itemCategory > INVENTORY_CAT_Count) {
        error("Item has invalid category");
        return false;
    }

    uint32_t itemId = itemGetItemId(itemToAdd);
    uint32_t itemCategory = itemGetCategory(itemToAdd);

    dbg("itemIdKey: %d", itemId);

    ItemKey itemKey;
    itemGenKey(itemId, itemKey);

    dbg("itemKey: %s", itemKey);

    if (zhash_insert(self->items, itemKey, itemToAdd) != 0) {
        error("Cannot insert the item '%s' in the hashtable.", itemKey);
        return false;
    }

    int result = zlist_append(self->bags[itemCategory], itemToAdd);
    if (result == -1) {
        error("Cannot push item into the category bag in inventory");
        return false;
    }

    return true;
}

bool inventoryRemoveItem(Inventory *self, Item *itemToRemove) {

    uint32_t itemId = itemGetItemId(itemToRemove);
    uint32_t itemCategory = itemGetCategory(itemToRemove);

    ItemKey itemKey;
    itemGenKey(itemId, itemKey);

    //if (zlist_exists(self->bags[itemToRemove->itemCategory], itemToRemove)) {
        zlist_remove(self->bags[itemCategory], itemToRemove);
    //} else {
    //    error("Item was not found in bag [%d] of inventory", itemToRemove->itemCategory);
    //    return false;
    //}



    zhash_delete(self->items, itemKey);

    return true;
}

bool inventoryGetItemByItemId(Inventory *self, uint64_t itemId, Item **_item) {

    Item *item = NULL;

    ItemKey itemKey;
    itemGenKey(itemId, itemKey);

    *_item = NULL;

    if (!(item = zhash_lookup(self->items, itemKey))) {
        error("Cannot find the item '%s' in the inventory.", itemKey);
        return false;
    }

    *_item = item;

    return true;
}

size_t inventoryGetItemsCount(Inventory *self) {
    return zhash_size(self->items);
}

Item *inventoryGetFirstItem(Inventory *self, InventoryCategory category) {
    return (Item*) zlist_first(self->bags[category]);
}

Item *inventoryGetNextItem(Inventory *self, InventoryCategory category) {
    return (Item*) zlist_next(self->bags[category]);
}

bool inventoryUnequipItem(Inventory *self, EquipmentSlot eqSlot) {

    EquipableItem *itemToUnequip = self->equippedItems[eqSlot];

    if (itemToUnequip == NULL) {
        // We return false here because the system should have detected earlier that the slot is free
        error("No item to unequip. Slot is free.");
        return false;
    }

    if (!inventoryAddItem(self, (Item *) itemToUnequip)) {
        error("Cannot add item to the inventory.");
        return false;
    }

    // Set slot as free
    self->equippedItems[eqSlot] = NULL;

    // Process unequip events
    /// TODO


    return true;
}

bool inventoryEquipItem(Inventory *self, uint64_t itemId, EquipmentSlot eqSlot) {

    EquipableItem *itemToEquip;

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
            dbg("Not possible to unequip item from slot %d", eqSlot);
            return false;
        }
    }

    // Now we have the slot free to equip this item, and item is not in inventory.
    self->equippedItems[eqSlot] = itemToEquip;

    // Remove from inventory
    if (!inventoryRemoveItem(self, (Item *) itemToEquip)) {
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
    return (EquipableItem*) self->equippedItems;
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

bool inventorySwapItems(Inventory *self, Item **_item1, Item **_item2) {
    // Check bag
    Item *item1 = *_item1;
    Item *item2 = *_item2;
    if (item1->itemCategory != item2->itemCategory) {
        error("Items to swap are from different bags");
        return false;
    }

    Item *itemTemp;

    itemTemp = item1;
    item1 = item2;
    item2 = itemTemp;

    return true;

}


void inventoryPrintEquipment(Inventory *self) {
    dbg("head_top = %d (%x)", self->equippedItems[0] ? itemGetItemId(self->equippedItems[0]) : 0, self->equippedItems[0]);
    dbg("head_middle = %d (%x)", self->equippedItems[1] ? itemGetItemId(self->equippedItems[1]) : 0, self->equippedItems[1]);
    dbg("itemUnk1 = %d (%x)", self->equippedItems[2] ? itemGetItemId(self->equippedItems[2]) : 0, self->equippedItems[2]);
    dbg("body_armor = %d (%x)", self->equippedItems[3] ? itemGetItemId(self->equippedItems[3]) : 0, self->equippedItems[3]);
    dbg("gloves = %d (%x)", self->equippedItems[4] ? itemGetItemId(self->equippedItems[4]) : 0, self->equippedItems[4]);
    dbg("boots = %d (%x)", self->equippedItems[5] ? itemGetItemId(self->equippedItems[5]) : 0, self->equippedItems[5]);
    dbg("helmet = %d (%x)", self->equippedItems[6] ? itemGetItemId(self->equippedItems[6]) : 0, self->equippedItems[6]);
    dbg("bracelet = %d (%x)", self->equippedItems[7] ? itemGetItemId(self->equippedItems[7]) : 0, self->equippedItems[7]);
    dbg("weapon = %d (%x)", self->equippedItems[8] ? itemGetItemId(self->equippedItems[8]) : 0, self->equippedItems[8]);
    dbg("shield = %d (%x)", self->equippedItems[9] ? itemGetItemId(self->equippedItems[9]) : 0, self->equippedItems[9]);
    dbg("costume = %d (%x)", self->equippedItems[10] ? itemGetItemId(self->equippedItems[10]) : 0, self->equippedItems[10]);
    dbg("itemUnk3 = %d (%x)", self->equippedItems[11] ? itemGetItemId(self->equippedItems[11]) : 0, self->equippedItems[11]);
    dbg("itemUnk4 = %d (%x)", self->equippedItems[12] ? itemGetItemId(self->equippedItems[12]) : 0, self->equippedItems[12]);
    dbg("itemUnk5 = %d (%x)", self->equippedItems[13] ? itemGetItemId(self->equippedItems[13]) : 0, self->equippedItems[13]);
    dbg("leg_armor = %d (%x)", self->equippedItems[14] ? itemGetItemId(self->equippedItems[14]) : 0, self->equippedItems[14]);
    dbg("itemUnk6 = %d (%x)", self->equippedItems[15] ? itemGetItemId(self->equippedItems[15]) : 0, self->equippedItems[15]);
    dbg("itemUnk7 = %d (%x)", self->equippedItems[16] ? itemGetItemId(self->equippedItems[16]) : 0, self->equippedItems[16]);
    dbg("ring_left = %d (%x)", self->equippedItems[17] ? itemGetItemId(self->equippedItems[17]) : 0, self->equippedItems[17]);
    dbg("ring_right = %d (%x)", self->equippedItems[18] ? itemGetItemId(self->equippedItems[18]) : 0, self->equippedItems[18]);
    dbg("necklace = %d (%x)", self->equippedItems[19] ? itemGetItemId(self->equippedItems[19]) : 0, self->equippedItems[19]);
}

void inventoryPrintBag(Inventory *self, InventoryCategory category) {
    Item *item;
    item = zlist_first(self->bags[category]);

    dbg("Printing Inventory bag[%d]", category);

    if (!item) {
        dbg("-- Bag [%d] is empty --", category);
    }

    int index = 0;
    while (item) {
        dbg("[%d] item: [%d]", index, item->itemType);

        item = zlist_next(self->bags[category]);
        index++;
    }
}
