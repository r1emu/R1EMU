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
    ItemAttributes *attributes;
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
