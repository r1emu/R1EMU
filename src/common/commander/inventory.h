/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file inventory.h
 * @brief
 *
 *
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#pragma once

// ---------- Includes ------------
#include "R1EMU.h"
#include "common/item/item.h"

// ---------- Defines -------------
#define INVENTORY_CAT_SIZE 5000

typedef enum {
    INVENTORY_TYPE_COMMANDER = 0,
    INVENTORY_TYPE_WAREHOUSE = 1,
}   InventoryType;

typedef enum {
    INVENTORY_ADD_UNK0 = 0,
    INVENTORY_ADD_PICKUP = 3,
    INVENTORY_ADD_NOT_NEW = 4,
}   InventoryAddType;

typedef enum {
    INVENTORY_CAT_WEAPON = 1,
    INVENTORY_CAT_ARMOR = 2,
    INVENTORY_CAT_SUBWEAPON = 3,
    INVENTORY_CAT_COSTUME = 4,
    INVENTORY_CAT_ACCESSORY = 5,
    INVENTORY_CAT_CONSUMABLE = 6,
    INVENTORY_CAT_GEM = 7,
    INVENTORY_CAT_MATERIAL = 8,
    INVENTORY_CAT_CARD = 9,
    INVENTORY_CAT_COLLECTION = 10,
    INVENTORY_CAT_BOOK = 11,
    INVENTORY_CAT_QUEST = 12,
    INVENTORY_CAT_PETWEAPON = 13,
    INVENTORY_CAT_PETARMOR = 14,
    INVENTORY_CAT_COUNT,
}   InventoryCategory;

typedef enum {
    EQSLOT_HEAD_TOP = 0,
    EQSLOT_HEAD_MIDDLE,
    EQSLOT_UNKOWN1,
    EQSLOT_BODY_ARMOR,
    EQSLOT_GLOVES,
    EQSLOT_BOOTS,
    EQSLOT_HELMET,
    EQSLOT_BRACELET,
    EQSLOT_WEAPON,
    EQSLOT_SHIELD,
    EQSLOT_COSTUME,
    EQSLOT_UNKOWN3,
    EQSLOT_UNKOWN4,
    EQSLOT_UNKOWN5,
    EQSLOT_LEG_ARMOR,
    EQSLOT_UNKOWN6,
    EQSLOT_UNKOWN7,
    EQSLOT_RIGHT_LEFT,
    EQSLOT_RIGHT_RIGHT,
    EQSLOT_NECKLACE,
    EQSLOT_COUNT
} EquipmentSlot;

typedef enum {
    EMPTYEQSLOT_NoHat = 2,
    EMPTYEQSLOT_NoOuter = 4,
    EMPTYEQSLOT_NoPants = 5,
    EMPTYEQSLOT_NoGloves = 6,
    EMPTYEQSLOT_NoBoots = 7,
    EMPTYEQSLOT_NoShirt = 8,
    EMPTYEQSLOT_NoRing = 9,
    EMPTYEQSLOT_NoNeck = 10,
    EMPTYEQSLOT_NoHelmet = 10000,
    EMPTYEQSLOT_NoArmband = 11000,
    EMPTYEQSLOT_NoWeapon_THSpear = 9999983,
    EMPTYEQSLOT_NoWeapon_Wand = 9999988,
    EMPTYEQSLOT_NoWeapon_Spear = 9999989,
    EMPTYEQSLOT_NoWeapon_Sword = 9999991,
    EMPTYEQSLOT_NoWeapon_THSword = 9999992,
    EMPTYEQSLOT_NoWeapon_Shield = 9999993,
    EMPTYEQSLOT_NoWeapon_Staff = 9999994,
    EMPTYEQSLOT_NoWeapon_Bow = 9999995,
    EMPTYEQSLOT_NoWeapon = 9999996,
    EMPTYEQSLOT_NoWeapon_Shotsword = 9999997,
    EMPTYEQSLOT_NoWeapon_Mace = 9999998,
    EMPTYEQSLOT_NoWeapon_THStaff = 9999999,
} EmptyEquipmentSlot;

// ------ Structure declaration -------

/**
 * @brief Inventory contains
 */
typedef struct Inventory
{
    zhash_t *items;
    zlist_t *bags[INVENTORY_CAT_COUNT];
    Item *equippedItems[EQSLOT_COUNT];
}   Inventory;

typedef struct InventorySPacket
{
    ItemSPacket equippedItems[0]; // Const Size : EQSLOT_COUNT

    size_t itemsCount;
    ItemSPacket items[0]; // Size : itemsCount

}   InventorySPacket;

// ----------- Functions ------------
/**
 * Allocate a new Inventory structure.
 * @return A pointer to an allocated Inventory, or NULL if an error occured.
 */
Inventory *inventoryNew(void);

/**
 * Initialize an allocated Inventory structure.
 * @param self An allocated Inventory to initialize.
 * @return true on success, false otherwise.
 */
bool inventoryInit(Inventory *self);

/**
 * Free an allocated Inventory structure.
 * @param self A pointer to an allocated Inventory.
 */
void inventoryFree(Inventory *self);

/**
 * Free an allocated Inventory structure and nullify the content of the pointer.
 * @param self A pointer to an allocated Inventory.
 */
void inventoryDestroy(Inventory **self);

/**
 * Adds an item to inventory
 * @param self A pointer to an allocated Inventory.
 * @param item an Item.
 */
bool inventoryAddItem(Inventory *self, Item *itemToAdd);
bool inventoryRemoveItem(Inventory *self, Item *itemToRemove);

/**
 * Get count (size) of items in inventory
 * @param self A pointer to an allocated Inventory.
 * @return count of items in inventory
 */
size_t inventoryGetItemsCount(Inventory *self);

Item *inventoryGetFirstItem(Inventory *self, InventoryCategory category);
Item *inventoryGetNextItem(Inventory *self, InventoryCategory category);

bool inventoryEquipItem(Inventory *self, uint64_t itemId, EquipmentSlot eqSlot);
void inventoryPrintEquipment(Inventory *self);
bool inventoryGetEquipmentEmptySlot(EquipmentSlot slot, uint32_t *value);
void inventoryPrintBag(Inventory *self, InventoryCategory category);
bool inventoryGetItemByItemId(Inventory *self, uint64_t itemId, Item **_item);
bool inventorySwapItems(Inventory *self, Item **_item1, Item **_item2);


size_t inventoryGetPacketSize(Inventory *self);
void inventorySerialize(Inventory *self, PacketStream *stream);
bool inventoryUnserialize(Inventory *self, PacketStream *stream);
