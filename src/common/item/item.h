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
}   InventoryCategory;

typedef struct ItemAttributes {

    zhash_t *hashtable; // TODO

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

/**
 * @brief Item contains
 */

typedef struct Item
{
    uint64_t itemId; // Unique ID for an Item - key.
    InventoryCategory itemCategory;
    uint32_t itemType; // ID of this item indicating which item is it.
    uint32_t amount; // amount of this item
    ItemAttributes *attributes;
    uint8_t useGender;
    uint8_t useJob;
    bool isTwoHanded;
    uint8_t equipSlot;
    bool isDummy; // When an item is created for dummy porpuses.
    uint32_t inventoryIndex;
} Item;

// ----------- Functions ------------

/**
 * @brief Initialize an ItemAttributes
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
