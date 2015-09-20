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

// ---------- Defines -------------

#define FLOAT_ATTRIBUTE_SIZE 6
#define STRING_ATTRIBUTE_HEADER_SIZE 4
#define ITEM_EQUIP_NOATTR_SIZE 24

// ------ Structure declaration -------
typedef struct ItemAttributes {
    float durability;
    float cooldown;
    char *memo;
    char *customName;
    char *crafterName;
    float pr; // dont know what it means yet
    float reinforce_2; // I believe it is +1, +2, +3
} ItemAttributes;

typedef enum ItemAttributeType {
    ITEM_ATTRIBUTE_DURABILITY     = 3770,
    ITEM_ATTRIBUTE_PR             = 3781,
    ITEM_ATTRIBUTE_COOLDOWN       = 3843,
    ITEM_ATTRIBUTE_REINFORCE_2    = 3852,
    ITEM_ATTRIBUTE_MEMO           = 3972,
    ITEM_ATTRIBUTE_CUSTOM_NAME    = 3975,
    ITEM_ATTRIBUTE_CRAFTER_NAME   = 3978,
} ItemAttributeType;

typedef struct ItemPkt {
    uint64_t uniqueId;
    uint32_t amount;
    uint32_t inventoryIndex;
    uint32_t id;
}   ItemPkt;

typedef enum AttributeType {
    FLOAT_ATTRIBUTE,
    STRING_ATTRIBUTE,
} AttributeType;

typedef struct Attribute {
    void *attribute;
    AttributeType type;
} Attribute;

typedef struct FloatAttribute {
    ItemAttributeType attributeType;
    float value;
} FloatAttribute;

typedef struct StringAttribute {
    ItemAttributeType attributeType;
    char *value;
} StringAttribute;

typedef struct AttributeArray {
    Attribute *attributes;
    int numAttributes;
} AttributeArray;

typedef struct ItemEquip {
    uint32_t itemType;
    uint64_t itemId;
    uint32_t inventoryIndex;
    AttributeArray attributeArray;
} ItemEquip;

// ----------- Functions ------------

/**
 * @brief Equip an item in the inventory
 */
bool itemAttributesInit(
    ItemAttributes *self,
    float durability,
    float cooldown,
    char *memo,
    char *customName,
    char *crafterName,
    float pr,
    float reinforce_2);

/**
 * @brief Equip an item in the inventory
 */
bool itemEquipInit(
    ItemEquip *self,
    uint32_t itemType,
    uint64_t itemId,
    uint32_t inventoryIndex,
    AttributeArray *attributeArray);

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

/**
 * Get the packet structure of the ItemEquip
 */
size_t itemEquipGetPacketSize(ItemEquip *itemEquip);

/**
 * Get the packet structure of the ItemAttributes
 */
bool itemAttributesGetPacket(ItemAttributes *itemAttributes, char *packet);

/**
 * Get the packet size of the itemAttributes
 */
size_t itemAttributesGetPacketSize(ItemAttributes *itemAttributes);
