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

bool initItemAttributes(ItemAttributes *self, float durability, float cooldown, char *memo, char *customName, char *crafterName,
                        float pr, float reinforce_2)
{
    self->durability = durability;
    self->cooldown = cooldown;
    self->memo = memo;
    self->customName = customName;
    self->crafterName = crafterName;
    self->pr = pr;
    self->reinforce_2 = reinforce_2;

    return true;
}

bool initStringAttribute(StringAttribute *self, ItemAttributeType attributeType, char *value){
    self->attributeType = attributeType;
    self->value = value;
    return true;
}

bool initFloatAttribute(FloatAttribute *self, ItemAttributeType attributeType, float value){
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
                totalSize += FLOAT_ATTRIBUTE_SIZE;
                break;
            }
            case STRING_ATTRIBUTE:
            {
                StringAttribute *attribute = (StringAttribute *) (attributes[i].attribute);
                totalSize += getStringAttributeSize(attribute);
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
                size_t packetLength = getStringAttributeSize(attribute);
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

size_t getStringAttributeSize(StringAttribute *attribute){
    return STRING_ATTRIBUTE_HEADER_SIZE + strlen(attribute->value) + 1; //null terminator
}

bool getStringAttributePacket(StringAttribute *attribute, char *packetBytes){

    size_t valueLength = strlen(attribute->value) + 1;

    #pragma pack(push, 1)
    struct {
        uint16_t type;
        uint16_t valueLength;
        char value[valueLength]; // +1 for null terminator
    } stringAttributePacket;
    #pragma pack(pop)

    stringAttributePacket.type = attribute->attributeType;
    stringAttributePacket.valueLength = valueLength;

    memcpy(&stringAttributePacket.value, attribute->value, valueLength);
    memcpy(packetBytes, &stringAttributePacket, sizeof(stringAttributePacket));

    return true;
}

size_t getItemEquipPacketSize(ItemEquip *itemEquip){
    return ITEM_EQUIP_NOATTR_SIZE + getAttributeListPacketSize(itemEquip->attributeList);
}

int getNumItemAttributes(ItemAttributes *itemAttributes){
    int total = 0;
    if(itemAttributes->durability) ++total;
    if(itemAttributes->cooldown) ++total;
    if(itemAttributes->memo) ++total;
    if(itemAttributes->customName) ++total;
    if(itemAttributes->crafterName) ++total;
    if(itemAttributes->pr) ++total;
    if(itemAttributes->reinforce_2) ++total;
    return total;
}

bool getItemAttributesPacket(ItemAttributes *itemAttributes, char *packet){
    int numAttributes = getNumItemAttributes(itemAttributes);

    Attribute attributes[numAttributes];
    AttributeList attributeList;
    initAttributeList(&attributeList, attributes, numAttributes);
    int cur_idx = 0;

    float durability = itemAttributes->durability;
    float cooldown = itemAttributes->cooldown;
    char *memo = itemAttributes->memo;
    char *customName = itemAttributes->customName;
    char *crafterName = itemAttributes->crafterName;
    float pr = itemAttributes->pr;
    float reinforce_2 = itemAttributes->reinforce_2;

    if(durability){
        FloatAttribute durabilityAttribute;
        initFloatAttribute(&durabilityAttribute, ITEM_ATTRIBUTE_DURABILITY, durability);
        Attribute attribute;
        initAttribute(&attribute, (void *) &durabilityAttribute, FLOAT_ATTRIBUTE);
        attributeList.attributes[cur_idx++] = attribute;
    }

    if(cooldown){
        FloatAttribute cooldownAttribute;
        initFloatAttribute(&cooldownAttribute, ITEM_ATTRIBUTE_COOLDOWN, cooldown);
        Attribute attribute;
        initAttribute(&attribute, (void *) &cooldownAttribute, FLOAT_ATTRIBUTE);
        attributeList.attributes[cur_idx++] = attribute;

    }

    if(memo){
        StringAttribute memoAttribute;
        initStringAttribute(&memoAttribute, ITEM_ATTRIBUTE_MEMO, memo);
        Attribute attribute;
        initAttribute(&attribute, (void *) &memoAttribute, STRING_ATTRIBUTE);
        attributeList.attributes[cur_idx++] = attribute;
    }

    if(customName){
        StringAttribute customNameAttribute;
        initStringAttribute(&customNameAttribute, ITEM_ATTRIBUTE_CUSTOM_NAME, customName);
        Attribute attribute;
        initAttribute(&attribute, (void *) &customNameAttribute, STRING_ATTRIBUTE);
        attributeList.attributes[cur_idx++] = attribute;
    }

    if(crafterName){
        StringAttribute crafterNameAttribute;
        initStringAttribute(&crafterNameAttribute, ITEM_ATTRIBUTE_CRAFTER_NAME, crafterName);
        Attribute attribute;
        initAttribute(&attribute, (void *) &crafterNameAttribute, STRING_ATTRIBUTE);
        attributeList.attributes[cur_idx++] = attribute;
    }

    if(pr){
        FloatAttribute prAttribute;
        initFloatAttribute(&prAttribute, ITEM_ATTRIBUTE_PR, pr);
        Attribute attribute;
        initAttribute(&attribute, (void *) &prAttribute, FLOAT_ATTRIBUTE);
        attributeList.attributes[cur_idx++] = attribute;
    }

    if(reinforce_2){
        FloatAttribute reinforce_2Attribute;
        initFloatAttribute(&reinforce_2Attribute, ITEM_ATTRIBUTE_REINFORCE_2, reinforce_2);
        Attribute attribute;
        initAttribute(&attribute, (void *) &reinforce_2Attribute, FLOAT_ATTRIBUTE);
        attributeList.attributes[cur_idx++] = attribute;
    }

    getAttributeListPacket(&attributeList, packet);
    return true;
}



size_t getItemAttributesPacketSize(ItemAttributes *itemAttributes){

    size_t total = 0;

    if(itemAttributes->durability) total += FLOAT_ATTRIBUTE_SIZE;
    if(itemAttributes->cooldown) total += FLOAT_ATTRIBUTE_SIZE;
    if(itemAttributes->memo) total += STRING_ATTRIBUTE_HEADER_SIZE + strlen(itemAttributes->memo) + 1;
    if(itemAttributes->customName) total += STRING_ATTRIBUTE_HEADER_SIZE + strlen(itemAttributes->customName) + 1;
    if(itemAttributes->crafterName) total += STRING_ATTRIBUTE_HEADER_SIZE + strlen(itemAttributes->crafterName) + 1;
    if(itemAttributes->pr) total += FLOAT_ATTRIBUTE_SIZE;
    if(itemAttributes->reinforce_2) total += FLOAT_ATTRIBUTE_SIZE;
    return total;
}
