/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file item.h
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
#include "item_attribute.h"

// ---------- Defines -------------

// ------ Structure declaration -------
/**
 * @brief Item contains basic information about an item
 */
typedef struct Item {

    /** Unique ID for an Item - key. */
    uint64_t itemId;

    /** ID of this item indicating which item is it. */
    uint32_t itemType;

    /** Amount of this item */
    uint32_t amount;

    /** Index of the item in the inventory */
    uint32_t inventoryIndex;

    /** Attributes of the item */
    ItemAttributes attributes;
} Item;

/**
 * @brief ItemPacket is the packet structure of an item
 */
typedef struct ItemPacket {
    uint64_t uniqueId;
    uint32_t id;
    uint32_t amount;
    uint32_t inventoryIndex;
} ItemPacket;

// ----------- Functions ------------

/**
 * Allocate a new Item structure.
 * @return A pointer to an allocated Item, or NULL if an error occured.
 */
Item *itemNew(uint64_t itemId, uint32_t itemType, uint32_t amount, uint32_t inventoryIndex);

/**
 * Initialize an allocated Item structure.
 * @param self An allocated Item to initialize.
 * @return true on success, false otherwise.
 */
bool itemInit(Item *self, uint64_t itemId, uint32_t itemType, uint32_t amount, uint32_t inventoryIndex);

/**
 * Generate a key for an item from ItemId
 */
void itemGenKey(uint64_t itemIdKey, char itemKey[17]);

/**
 * Free an allocated Item structure.
 * @param self A pointer to an allocated Item.
 */
void itemFree(Item *self);

/**
 * Free an allocated Item structure and nullify the content of the pointer.
 * @param self A pointer to an allocated Item.
 */
void itemDestroy(Item **self);

/**
 * Add, get, update or remove attributes from an item
 */
bool itemAddAttribute(Item *self, ItemAttributeId itemAttrId, void *value);
bool itemGetAttribute(Item *self, ItemAttributeId itemAttrId, void **value);
bool itemUpdateAttribute(Item *self, ItemAttributeId itemAttrId, void *value);
bool itemRemoveAttribute(Item *self, ItemAttributeId itemAttrId);
