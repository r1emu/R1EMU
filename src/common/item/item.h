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
    float reinforce_2; // I belive it is +1, +2, +3
} ItemAttributes;

typedef enum ItemAttributeType
{
    ITEM_ATTRIBUTE_DURABILITY = 3770,
    ITEM_ATTRIBUTE_PR = 3781,
    ITEM_ATTRIBUTE_COOLDOWN   = 3843,
    ITEM_ATTRIBUTE_REINFORCE_2   = 3852,
    ITEM_ATTRIBUTE_MEMO   = 3972,
    ITEM_ATTRIBUTE_CUSTOM_NAME   = 3975,
    ITEM_ATTRIBUTE_CRAFTER_NAME   = 3978,
} ItemAttributeType;

typedef struct {
    uint64_t uniqueId;
    uint32_t amount;
    uint32_t inventoryIndex;
    uint32_t id;
}   ItemPkt;

typedef enum AttributeType{
    FLOAT_ATTRIBUTE,
    STRING_ATTRIBUTE,
} AttributeType;

typedef struct Attribute{
    void *attribute;
    AttributeType type;
} Attribute;

typedef struct FloatAttribute{
    ItemAttributeType attributeType;
    float value;
} FloatAttribute;

typedef struct StringAttribute{
    ItemAttributeType attributeType;
    char *value;
} StringAttribute;

typedef struct AttributeList{
    Attribute *attributes;
    int numAttributes;
} AttributeList;

typedef struct ItemEquip{
    uint32_t itemType;
    uint64_t itemId;
    uint32_t inventoryIndex;
    AttributeList *attributeList;
} ItemEquip;

// ----------- Functions ------------

bool initItemAttributes(ItemAttributes *self, float durability, float cooldown, char *memo, char *customName, char *crafterName,
                        float pr, float reinforce_2);

bool initStringAttribute(StringAttribute *self, ItemAttributeType attributeType, char *value);

bool initFloatAttribute(FloatAttribute *self, ItemAttributeType attributeType, float value);

bool initAttribute(Attribute *self, void *attribute, AttributeType attributeType);

bool initAttributeList(AttributeList *self, Attribute *attributes, int numAttributes);

bool initItemEquip(ItemEquip *self, uint32_t itemType, uint64_t itemId, uint32_t inventoryIndex, AttributeList *attributeList);

size_t getAttributeListPacketSize(AttributeList *attributeList);

bool getAttributeListPacket(AttributeList *attributeList, char *packet);

bool getFloatAttributePacket(FloatAttribute *attribute, char *packetBytes);

size_t getStringAttributeSize(StringAttribute *attribute);

bool getStringAttributePacket(StringAttribute *attribute, char *packetBytes);

size_t getItemEquipPacketSize(ItemEquip *itemEquip);

int getNumItemAttributes(ItemAttributes *itemAttributes);

bool getItemAttributesPacket(ItemAttributes *itemAttributes, char *packet);

size_t getItemAttributesPacketSize(ItemAttributes *itemAttributes);
