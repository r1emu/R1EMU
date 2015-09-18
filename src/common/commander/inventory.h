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
    INVENTORY_CAT_QUEST =  12,
    INVENTORY_CAT_PETWEAPON = 13,
    INVENTORY_CAT_PETARMOR = 14,
}   InventoryCategory;

// ------ Structure declaration -------
typedef struct Inventory Inventory;

/**
 * @brief Inventory contains the basic structure of an inventory
 */
struct Inventory
{
};

// ----------- Functions ------------
/**
 * Allocate a new Inventory structure.
 * @return A pointer to an allocated Inventory, or NULL if an error occurred.
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

