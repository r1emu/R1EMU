/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */


// ---------- Includes ------------
#include "item.h"
#include "common/packet/packet_stream.h"

bool initStringAttribute(StringAttribute *self, uint16_t attributeType, size_t valueLength, char *value){
    self->attributeType = attributeType;
    self->valueLength = valueLength;
    self->value = value;
    return true;
}

bool initFloatAttribute(FloatAttribute *self, uint16_t attributeType, float value){
    self->attributeType = attributeType;
    self->value = value;
    return true;
}

bool initAttribute(Attribute *self, void *attribute, AttributeType type){
    self->attribute = attribute;
    self->type = type;
    return true;
}

bool initAttributeList(AttributeList *self, Attribute *attributes, int numAttributes){
    self->attributes = attributes;
    self->numAttributes = numAttributes;
    return true;
}

bool initItemEquip(ItemEquip *self, uint32_t itemType, uint64_t itemId, uint32_t inventoryIndex, AttributeList *attributeList){
    self->itemType = itemType;
    self->itemId = itemId;
    self->inventoryIndex = inventoryIndex;
    self->attributeList = attributeList;
    return true;
}

size_t getAttributeListPacketSize(AttributeList *attributeList){

    int totalSize = 0;
    int numAttributes = attributeList->numAttributes;
    Attribute *attributes = attributeList->attributes;

    for(int i = 0; i < numAttributes; i++){
        AttributeType type = attributes[i].type;
        switch(type){
            case FLOAT_ATTRIBUTE:
            {
                //float attributes are always six bytes
                totalSize += FLOAT_ATTRIBUTE_SIZE;
                break;
            }
            case STRING_ATTRIBUTE:
            {
                StringAttribute *attribute = (StringAttribute *) (attributes[i].attribute);
                totalSize += attribute->valueLength + STRING_ATTRIBUTE_HEADER_SIZE;
                break;
            }
        }
    }

    return totalSize;
}
bool getAttributeListPacket(AttributeList *attributeList, char *packet){

    int numAttributes = attributeList->numAttributes;
    Attribute *attributes = attributeList->attributes;

    PacketStream *packetStream = packetStreamNew(packet);

    for(int i = 0; i < numAttributes; i++){
        AttributeType type = attributes[i].type;

        switch(type){
            case FLOAT_ATTRIBUTE:
            {
                FloatAttribute *attribute = (FloatAttribute *) (attributes[i].attribute);

                size_t packetLength = FLOAT_ATTRIBUTE_SIZE;
                char packetBytes[packetLength];

                getFloatAttributePacket(attribute, packetBytes);
                packetStreamAppend(packetStream, packetBytes, packetLength);
                break;
            }

            case STRING_ATTRIBUTE:
            {
                StringAttribute *attribute = (StringAttribute *) (attributes[i].attribute);

                size_t packetLength = attribute->valueLength + STRING_ATTRIBUTE_HEADER_SIZE;
                char packetBytes[packetLength];

                getStringAttributePacket(attribute, packetBytes);
                packetStreamAppend(packetStream, packetBytes, packetLength);

                break;

            }
        }
    }
    return true;
}

bool getFloatAttributePacket(FloatAttribute *attribute, char *packetBytes){

    #pragma pack(push, 1)
    struct {
        uint16_t type;
        float value;
    } floatAttributePacket;
    #pragma pack(pop)

    floatAttributePacket.type = attribute->attributeType;
    floatAttributePacket.value = attribute->value;

    memcpy(packetBytes, &floatAttributePacket, sizeof(floatAttributePacket));

    return true;
}

bool getStringAttributePacket(StringAttribute *attribute, char *packetBytes){

    #pragma pack(push, 1)
    struct {
        uint16_t type;
        uint16_t valueLength;
        char value[attribute->valueLength];
    } stringAttributePacket;
    #pragma pack(pop)

    stringAttributePacket.type = attribute->attributeType;
    stringAttributePacket.valueLength = attribute->valueLength;

    memcpy(&stringAttributePacket.value, attribute->value, attribute->valueLength);
    memcpy(packetBytes, &stringAttributePacket, sizeof(stringAttributePacket));

    return true;
}

size_t getItemEquipPacketSize(ItemEquip *itemEquip){
    return ITEM_EQUIP_NOATTR_SIZE + getAttributeListPacketSize(itemEquip->attributeList);
}

