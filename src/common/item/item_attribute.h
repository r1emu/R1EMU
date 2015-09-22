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
#include "common/packet/packet_stream.h"

// ---------- Defines -------------

// ------ Structure declaration -------
typedef struct ItemAttributes {
    zhash_t *hashtable;
} ItemAttributes;

typedef enum ItemAttributeId {
    ITEM_ATTRIBUTE_ID_DURABILITY     = 3770,
    ITEM_ATTRIBUTE_ID_PR             = 3781,
    ITEM_ATTRIBUTE_ID_COOLDOWN       = 3843,
    ITEM_ATTRIBUTE_ID_REINFORCE_2    = 3852,
    ITEM_ATTRIBUTE_ID_MEMO           = 3972,
    ITEM_ATTRIBUTE_ID_CUSTOM_NAME    = 3975,
    ITEM_ATTRIBUTE_ID_CRAFTER_NAME   = 3978,
} ItemAttributeId;

// ----------- Functions ------------
/**
 * @brief Creates memory and initializes an ItemAttributes
 */
ItemAttributes *itemAttributesNew(void);

/**
 * @brief Initialize an ItemAttributes
 */
bool itemAttributesInit(ItemAttributes *self);

/**
 * Get the packet structure of the ItemAttributes
 */
void itemAttributesGetPacket(ItemAttributes *self, PacketStream *packetStream);

/**
 * Get the packet size of the itemAttributes
 */
size_t itemAttributesGetPacketSize(ItemAttributes *self);

/**
 * Get, add, update or remove an attribute from the item attributes
 */
bool itemAttributesGet(ItemAttributes *self, ItemAttributeId itemAttrType, void **_output);
bool itemAttributesAdd(ItemAttributes *self, ItemAttributeId itemAttrType, void *value);
bool itemAttributesUpdate(ItemAttributes *self, ItemAttributeId itemAttrId, void *value);
bool itemAttributesRemove(ItemAttributes *self, ItemAttributeId itemAttrType);

/**
 * @brief Free an allocated ItemAttributes structure.
 * @param self A pointer to an allocated ItemAttributes.
 */
void itemAttributesFree(ItemAttributes *self);

/**
 * @brief Free an allocated ItemAttributes structure and nullify the content of the pointer.
 * @param self A pointer to an allocated ItemAttributes.
 */
void itemAttributesDestroy(ItemAttributes **_self);
