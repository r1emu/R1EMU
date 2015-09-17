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
    uint16_t attributeType;
    float value;
} FloatAttribute;

typedef struct StringAttribute{
    uint16_t attributeType;
    size_t valueLength;
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

bool initStringAttribute(StringAttribute *self, uint16_t attributeType, size_t valueLength, char *value);

bool initFloatAttribute(FloatAttribute *self, uint16_t attributeType, float value);

bool initAttribute(Attribute *self, void *attribute, AttributeType attributeType);

bool initAttributeList(AttributeList *self, Attribute *attributes, int numAttributes);

bool initItemEquip(ItemEquip *self, uint32_t itemType, uint64_t itemId, uint32_t inventoryIndex, AttributeList *attributeList);

size_t getAttributeListPacketSize(AttributeList *attributeList);

bool getAttributeListPacket(AttributeList *attributeList, char *packet);

bool getFloatAttributePacket(FloatAttribute *attribute, char *packetBytes);

bool getStringAttributePacket(StringAttribute *attribute, char *packetBytes);

size_t getItemEquipPacketSize(ItemEquip *itemEquip);
