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

    for (int i = 0; i < ITEM_CAT_COUNT; i++) {
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

ItemHandler *itemHandlerNew(Item *item) {
     ItemHandler *self = malloc(sizeof(ItemHandler));
     *self = item;
     return self;
}

bool inventoryAddItem(Inventory *self, ItemHandler itemToAdd) {

    ItemCategory itemCategory = itemGetCategory(itemToAdd);
    ActorId_t actorId = actorGetUId(itemToAdd);

    ItemHandler *itemHandler = itemHandlerNew(itemToAdd);

    ActorKey actorKey;
    actorGenKey(actorId, actorKey);

    if (zhash_insert(self->items, actorKey, itemHandler) != 0) {
        error("Cannot insert the item '%s' in the hashtable.", actorKey);
        return false;
    }



    if (zlist_append(self->bags[itemCategory], itemHandler) != 0) {
        error("Cannot push item into the category bag in inventory");
        return false;
    }

    return true;
}

bool inventoryRemoveItem(Inventory *self, ItemHandler itemToRemove) {

    ActorId_t actorId = actorGetUId(itemToRemove);
    ItemCategory itemCategory = itemGetCategory(itemToRemove);

    ActorKey actorKey;
    actorGenKey(actorId, actorKey);

    ItemHandler *itemHandler = zhash_lookup(self->items, actorKey);

    zlist_remove(self->bags[itemCategory], itemHandler);
    zhash_delete(self->items, actorKey);

    /// TODO: how to destroy the ItemHandler

    return true;
}

void itemGenActorKey(Item *self, ActorKey actorKey) {
    ActorId_t actorId = actorGetUId(self);
    actorGenKey(actorId, actorKey);
}

bool inventoryGetItemByActorId(Inventory *self, ActorId_t actorId, ItemHandler *itemHandler) {

    ItemHandler *item = NULL;

    ActorKey actorKey;
    actorGenKey(actorId, actorKey);

    *itemHandler = NULL;

    if (!(item = zhash_lookup(self->items, actorKey))) {
        error("Cannot find the item '%s' in the inventory.", actorKey);
        return false;
    }

    *itemHandler = *item;

    return true;
}

size_t inventoryGetItemsCount(Inventory *self) {
    return zhash_size(self->items);
}

Item *inventoryGetFirstItem(Inventory *self, ItemCategory category) {
    inventoryPrintBag(self, category);
    ItemHandler *itemHandler = zlist_first(self->bags[category]);
    if (itemHandler == NULL) {
        return NULL;
    }
    return *itemHandler;
}

Item *inventoryGetNextItem(Inventory *self, ItemCategory category) {
    ItemHandler *itemHandler = zlist_next(self->bags[category]);
    if (itemHandler == NULL) {
        return NULL;
    }
    return *itemHandler;
}

bool inventoryUnequipItem(Inventory *self, ItemEquipmentSlot eqSlot) {

    ItemEquipable *itemToUnequip = self->equippedItems[eqSlot];

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

bool inventoryEquipItem(Inventory *self, ActorId_t actorId, ItemEquipmentSlot eqSlot) {

    ItemEquipable *itemToEquip;

    // Get item from inventory
    if (!inventoryGetItemByActorId(self, actorId, (Item**) &itemToEquip)) {
        dbg("Can not find item in inventory. ActorId: %d", actorId);
        return false;
    }

    dbg("Item to equip: %d", itemGetId((Item*)itemToEquip));

    // Check if eqSlot is right for the item we want to equip.
    /*
    bool expectedEqSlot = false;
    switch (itemEquipableGetCategory(itemToEquip)) {
    }
    if (!expectedEqSlot) {
        error("Unexcepted equipment slot encountered : %d", expectedEqSlot);
        return false;
    }
    */

    // Check if there is the slot already, if so, unequip.
    if (self->equippedItems[eqSlot] != NULL) {
        // Unequip Item
        dbg("equip slot is occupied, unequip item to free slot first.");
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

uint32_t inventoryGetEquipmentEmptySlot(ItemEquipmentSlot slot) {

    uint32_t value;

    switch (slot) {
        case EQSLOT_HAT         : value = EMPTYEQSLOT_NoHat; break;
        case EQSLOT_HAT_L       : value = EMPTYEQSLOT_NoHat; break;
        case EQSLOT_UNKOWN1     : value = EMPTYEQSLOT_NoOuter; break;
        case EQSLOT_BODY_ARMOR  : value = EMPTYEQSLOT_NoHat; break;
        case EQSLOT_GLOVES      : value = EMPTYEQSLOT_NoGloves; break;
        case EQSLOT_BOOTS       : value = EMPTYEQSLOT_NoBoots; break;
        case EQSLOT_HELMET      : value = EMPTYEQSLOT_NoHelmet; break;
        case EQSLOT_BRACELET    : value = EMPTYEQSLOT_NoArmband; break;
        case EQSLOT_WEAPON      : value = EMPTYEQSLOT_NoWeapon; break;
        case EQSLOT_SHIELD      : value = EMPTYEQSLOT_NoWeapon; break;
        case EQSLOT_COSTUME     : value = EMPTYEQSLOT_NoOuter; break;
        case EQSLOT_UNKOWN3     : value = EMPTYEQSLOT_NoRing; break;
        case EQSLOT_UNKOWN4     : value = EMPTYEQSLOT_NoRing; break;
        case EQSLOT_UNKOWN5     : value = EMPTYEQSLOT_NoOuter; break;
        case EQSLOT_LEG_ARMOR   : value = EMPTYEQSLOT_NoShirt; break;
        case EQSLOT_UNKOWN6     : value = EMPTYEQSLOT_NoRing; break;
        case EQSLOT_UNKOWN7     : value = EMPTYEQSLOT_NoRing; break;
        case EQSLOT_RING_LEFT   : value = EMPTYEQSLOT_NoRing; break;
        case EQSLOT_RING_RIGHT  : value = EMPTYEQSLOT_NoRing; break;
        case EQSLOT_NECKLACE    : value = EMPTYEQSLOT_NoNeck; break;

        case EQSLOT_NOSLOT      :
        case EQSLOT_COUNT       : warning("Equipment slot not expected."); break;
        /*
            DONTFIX : We want the compiler to warn about missing cases.

            default: {
            *value = -1;
            return false;
        } */
    }

    return value;
}

bool inventorySwapItems(Inventory *self, ActorId_t actorId1, ActorId_t actorId2) {

    ActorKey actorKey;

    actorGenKey(actorId1, actorKey);
    ItemHandler *item1Handler = zhash_lookup(self->items, actorKey);

    actorGenKey(actorId2, actorKey);
    ItemHandler *item2Handler = zhash_lookup(self->items, actorKey);


    if (itemGetCategory(*item1Handler) != itemGetCategory(*item2Handler)) {
        error("Items to swap are from different bags");
        return false;
    }

    ItemHandler tempItemHandler = *item1Handler;

    *item1Handler = *item2Handler;
    *item2Handler = tempItemHandler;

    return true;
}

int inventoryGetBagIndexByActorId(Inventory *self, ItemCategory category, ActorId_t actorId) {

    int bagIndex = -1;

    zlist_t *bag = self->bags[category];

    int index = 1;

    for (ItemHandler *itemHandler = zlist_first(bag); itemHandler != NULL; itemHandler = zlist_next(bag)) {

        if (actorId == actorGetUId(*itemHandler)) {
            bagIndex = (5000 * category) + index;
            break;
        }

        index++;
    }
    return bagIndex;
}


void inventoryPrintEquipment(Inventory *self) {

    dbg("head_top = %d (%x)", self->equippedItems[0] ? itemGetId((Item*) self->equippedItems[0]) : 0, self->equippedItems[0]);
    dbg("head_middle = %d (%x)", self->equippedItems[1] ? itemGetId((Item*) self->equippedItems[1]) : 0, self->equippedItems[1]);
    dbg("itemUnk1 = %d (%x)", self->equippedItems[2] ? itemGetId((Item*) self->equippedItems[2]) : 0, self->equippedItems[2]);
    dbg("body_armor = %d (%x)", self->equippedItems[3] ? itemGetId((Item*) self->equippedItems[3]) : 0, self->equippedItems[3]);
    dbg("gloves = %d (%x)", self->equippedItems[4] ? itemGetId((Item*) self->equippedItems[4]) : 0, self->equippedItems[4]);
    dbg("boots = %d (%x)", self->equippedItems[5] ? itemGetId((Item*) self->equippedItems[5]) : 0, self->equippedItems[5]);
    dbg("helmet = %d (%x)", self->equippedItems[6] ? itemGetId((Item*) self->equippedItems[6]) : 0, self->equippedItems[6]);
    dbg("bracelet = %d (%x)", self->equippedItems[7] ? itemGetId((Item*) self->equippedItems[7]) : 0, self->equippedItems[7]);
    dbg("weapon = %d (%x)", self->equippedItems[8] ? itemGetId((Item*) self->equippedItems[8]) : 0, self->equippedItems[8]);
    dbg("shield = %d (%x)", self->equippedItems[9] ? itemGetId((Item*) self->equippedItems[9]) : 0, self->equippedItems[9]);
    dbg("costume = %d (%x)", self->equippedItems[10] ? itemGetId((Item*) self->equippedItems[10]) : 0, self->equippedItems[10]);
    dbg("itemUnk3 = %d (%x)", self->equippedItems[11] ? itemGetId((Item*) self->equippedItems[11]) : 0, self->equippedItems[11]);
    dbg("itemUnk4 = %d (%x)", self->equippedItems[12] ? itemGetId((Item*) self->equippedItems[12]) : 0, self->equippedItems[12]);
    dbg("itemUnk5 = %d (%x)", self->equippedItems[13] ? itemGetId((Item*) self->equippedItems[13]) : 0, self->equippedItems[13]);
    dbg("leg_armor = %d (%x)", self->equippedItems[14] ? itemGetId((Item*) self->equippedItems[14]) : 0, self->equippedItems[14]);
    dbg("itemUnk6 = %d (%x)", self->equippedItems[15] ? itemGetId((Item*) self->equippedItems[15]) : 0, self->equippedItems[15]);
    dbg("itemUnk7 = %d (%x)", self->equippedItems[16] ? itemGetId((Item*) self->equippedItems[16]) : 0, self->equippedItems[16]);
    dbg("ring_left = %d (%x)", self->equippedItems[17] ? itemGetId((Item*) self->equippedItems[17]) : 0, self->equippedItems[17]);
    dbg("ring_right = %d (%x)", self->equippedItems[18] ? itemGetId((Item*) self->equippedItems[18]) : 0, self->equippedItems[18]);
    dbg("necklace = %d (%x)", self->equippedItems[19] ? itemGetId((Item*) self->equippedItems[19]) : 0, self->equippedItems[19]);

}

void inventoryPrintBag(Inventory *self, ItemCategory category) {

    dbg("Printing Inventory bag[%d], size: %d", category, zlist_size(self->bags[category]));

    int index = 0;
    for (ItemHandler *itemHandler = zlist_first(self->bags[category]); itemHandler != NULL; itemHandler = zlist_next(self->bags[category])) {
        dbg("[%d] item: [%d]", index, itemGetId(*itemHandler));
        index++;
    }

    if (index == 0) {
        dbg("-- Bag [%d] is empty --", category);
    }
    dbg("------------------------------------");

}
