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
typedef enum InventoryType {
    INVENTORY_TYPE_COMMANDER = 0,
    INVENTORY_TYPE_WAREHOUSE = 1,
}   InventoryType;

typedef enum InventoryAddType {
    INVENTORY_ADD_UNK0 = 0,
    INVENTORY_ADD_PICKUP = 3,
    INVENTORY_ADD_NOT_NEW = 4,
}   InventoryAddType;

// ------ Structure declaration -------
typedef struct Inventory Inventory;

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

