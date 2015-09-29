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

    for (int i = 0; i < INVENTORY_CAT_COUNT; i++) {
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

    if (itemToAdd->itemCategory > INVENTORY_CAT_COUNT) {
        error("Item has invalid category");
        return false;
    }

    ItemKey itemKey;
    itemGenKey(itemToAdd->itemId, itemKey);

    if (zhash_insert(self->items, itemKey, itemToAdd) != 0) {
        error("Cannot insert the item '%s' in the hashtable.", itemKey);
        return false;
    }

    if (!zhash_freefn(self->items, itemKey, (zhash_free_fn *) itemFree)) {
        error("Cannot set item '%s' destructor.", itemKey);
        return false;
    }

    if (zlist_append(self->bags[itemToAdd->itemCategory], itemToAdd) != 0) {
        error("Cannot push item into the category bag in inventory");
        return false;
    }

    return true;
}

bool inventoryRemoveItem(Inventory *self, Item *itemToRemove) {

    ItemKey itemKey;
    itemGenKey(itemToRemove->itemId, itemKey);

    zlist_remove(self->bags[itemToRemove->itemCategory], itemToRemove);
    zhash_delete(self->items, itemKey);

    return true;
}

bool inventoryGetItemByItemId(Inventory *self, uint64_t itemId, Item **_item) {

    Item *item = NULL;

    ItemKey itemKey;
    itemGenKey(itemId, itemKey);

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

size_t inventoryGetEquipmentCount(Inventory *self) {
    size_t count = 0;

    for (size_t i = 0; i < sizeof_array(self->equippedItems); i++) {
        if (self->equippedItems[i] != NULL) {
            count++;
        }
    }

    return count;
}

Item *inventoryGetFirstItem(Inventory *self, InventoryCategory category) {
    return (Item*) zlist_first(self->bags[category]);
}

Item *inventoryGetNextItem(Inventory *self, InventoryCategory category) {
    return (Item*) zlist_next(self->bags[category]);
}

bool inventoryUnequipItem(Inventory *self, EquipmentSlot eqSlot) {

    Item *itemToUnequip = NULL;

    if (!(itemToUnequip = self->equippedItems[eqSlot])) {
        // We return false here because the system should have detected earlier that the slot is free
        error("No item to unequip. Slot is free.");
        return false;
    }

    if (!inventoryAddItem(self, itemToUnequip)) {
        error("Cannot add item to the inventory.");
        return false;
    }

    // Set slot as free
    self->equippedItems[eqSlot] = NULL;

    return true;
}

bool inventoryCheckSlot(EquipmentSlot eqSlot) {
    return (eqSlot >= 0 && eqSlot < EQSLOT_COUNT);
}

bool inventoryEquipItem(Inventory *self, uint64_t itemId, EquipmentSlot eqSlot) {

    Item *itemToEquip;

    // Get item from inventory
    if (!inventoryGetItemByItemId(self, itemId, &itemToEquip)) {
        error("Can not find item in inventory. ItemId: %llx", itemId);
        return false;
    }

    // Check if eqSlot is right for the item we want to equip.
    if (!(inventoryCheckSlot(eqSlot))) {
        error("The slot '%d' isn't valid.", eqSlot);
        return false;
    }

    // Check if there is something in the slot already, if so, unequip.
    if (self->equippedItems[eqSlot] != NULL) {
        if (!inventoryUnequipItem(self, eqSlot)) {
            error("Not possible to unequip item from slot %d", eqSlot);
            return false;
        }
    }

    // Now we have the slot free to equip this item, and item is not in inventory.
    self->equippedItems[eqSlot] = itemToEquip;
    itemToEquip->index = eqSlot;

    // Remove from inventory
    if (!inventoryRemoveItem(self, itemToEquip)) {
        error("Can't remove item from inventory. Equip Item failed.");
        // Roll back equipped item
        self->equippedItems[eqSlot] = NULL;
        return false;
    }

    return true;

}

Item *inventoryGetEquipment(Inventory *self) {
    return (Item*) self->equippedItems;
}

bool inventoryGetEquipmentEmptySlot(EquipmentSlot slot, EmptyEquipmentSlot *value) {

    switch (slot) {
        case EQSLOT_HEAD_TOP: *value = EMPTYEQSLOT_NoHat; break;
        case EQSLOT_HEAD_MIDDLE: *value = EMPTYEQSLOT_NoHat; break;
        case EQSLOT_UNKOWN1: *value = EMPTYEQSLOT_NoOuter; break;
        case EQSLOT_BODY_ARMOR: *value = EMPTYEQSLOT_NoShirt; break;
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
            error("Unknown eqSlot '%d'", slot);
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

void inventoryPrintBag(Inventory *self, InventoryCategory category) {

    Item *item;
    zlist_t *bag = self->bags[category];

    dbg("Printing Inventory bag [%d]", category);

    if (zlist_size(bag) == 0) {
        dbg("-- Bag [%d] is empty --", category);
        return;
    }

    int index = 0;
    for (item = zlist_first(bag); item != NULL; item = zlist_next(bag)) {
        dbg("[%d] item: [%d]", index++, item->itemType);
    }
}

size_t inventoryGetPacketSize(Inventory *self) {
    size_t packetSize = 0;

    packetSize += sizeof(InventorySPacket);

    // Get size of equipped items
    for (size_t i = 0; i < EQSLOT_COUNT; i++) {
        Item *item = self->equippedItems[i];
        packetSize += itemGetPacketSize(item);
    }

    // Get size of inventory items
    for (Item *item = zhash_first(self->items); item != NULL; item = zhash_next(self->items)) {
        packetSize += itemGetPacketSize(item);
    }

    return packetSize;
}

void inventorySerialize(Inventory *self, PacketStream *stream) {

    size_t equipmentCount = inventoryGetEquipmentCount(self);

    packetStreamIn(stream, &equipmentCount);

    // Write equipped items
    for (size_t i = 0; i < EQSLOT_COUNT; i++) {
        Item *item = self->equippedItems[i];
        if (item != NULL) {
            itemSerialize(item, stream);
        }
    }

    size_t itemsCount = zhash_size(self->items);
    packetStreamIn(stream, &itemsCount);

    // Write inventory items
    for (Item *item = zhash_first(self->items); item != NULL; item = zhash_next(self->items)) {
        itemSerialize(item, stream);
    }
}

bool inventoryUnserialize(Inventory *self, PacketStream *stream) {

    bool status = false;
     Item *newItem = NULL;

    size_t equipmentCount = inventoryGetEquipmentCount(self);
    packetStreamOut(stream, &equipmentCount);

    // Free the old equipped items
    for (size_t i = 0; i < EQSLOT_COUNT; i++) {
        // Free the old item
        itemDestroy(&self->equippedItems[i]);
    }

    for (size_t i = 0; i < equipmentCount; i++) {
        // Read the new item
        ItemSPacket *itemPkt = packetStreamGetCurrentBuffer(stream);

        if (!(newItem = itemNew(itemPkt->itemId, itemPkt->itemType, itemPkt->amount, itemPkt->index))) {
            error("Cannot allocate a new copy of the item '%llx'.", itemPkt->itemId);
            goto cleanup;
        }

        if (!(itemUnserialize(newItem, stream))) {
            error("Cannot unserialize the equipped item %d.", i);
            goto cleanup;
        }

        self->equippedItems[newItem->index] = newItem;
    }

    size_t itemsCount;
    packetStreamOut(stream, &itemsCount);

    // Purge old inventory
    zhash_purge(self->items);
    for (size_t i = 0; i < sizeof_array(self->bags); i++) {
        zlist_purge(self->bags[i]);
    }

    for (size_t i = 0; i < itemsCount; i++) {
        ItemSPacket *itemPkt = packetStreamGetCurrentBuffer(stream);

        if (!(newItem = itemNew(itemPkt->itemId, itemPkt->itemType, itemPkt->amount, itemPkt->index))) {
            error("Cannot allocate a new copy of the item '%llx'.", itemPkt->itemId);
            goto cleanup;
        }

        if (!(itemUnserialize(newItem, stream))) {
            error("Cannot unserialize the item '%llx'", newItem->itemId);
            goto cleanup;
        }

        if (!(inventoryAddItem(self, newItem))) {
            error("Cannot add a new item '%llx' in the inventory.", newItem->itemId);
            goto cleanup;
        }
    }

    status = true;
cleanup:
    return status;
}
