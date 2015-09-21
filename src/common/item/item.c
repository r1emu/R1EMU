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

// Private functions
static bool stringAttributeInit(StringAttribute *self, ItemAttributeType attributeType, char *value);
static bool floatAttributeInit(FloatAttribute *self, ItemAttributeType attributeType, float value);
static bool attributeInit(Attribute *self, void *attribute, AttributeFormat attributeFormat);
static bool attributeArrayInit(AttributeArray *self, Attribute *attributes, int numAttributes);
static size_t attributeArrayGetPacketSize(AttributeArray *attributeArray);
static bool attributeArrayGetPacket(AttributeArray *attributeArray, char *packet);
static bool floatAttributeGetPacket(FloatAttribute *attribute, char *packetBytes);
static size_t stringAttributeGetSize(StringAttribute *attribute);
static bool stringAttributeGetPacket(StringAttribute *attribute, char *packetBytes);
static size_t itemAttributesGetNum(ItemAttributes *itemAttributes);

char *attributeKeys [] = {
    [ITEM_ATTRIBUTE_DURABILITY]   = "durability",
    [ITEM_ATTRIBUTE_PR]           = "pr",
    [ITEM_ATTRIBUTE_COOLDOWN]     = "cooldown",
    [ITEM_ATTRIBUTE_REINFORCE_2]  = "reinforce_2",
    [ITEM_ATTRIBUTE_MEMO]         = "memo",
    [ITEM_ATTRIBUTE_CUSTOM_NAME]  = "custom_name",
    [ITEM_ATTRIBUTE_CRAFTER_NAME] = "crafter_name"
};

bool itemAttributesGetAttribute(ItemAttributes *self, ItemAttributeType itemAttrType, void **_output) {

    bool status = false;
    char *attributeKey = NULL;

    if (!(attributeKey = attributeKeys[itemAttrType])) {
        error("No attribute key for '%d'", itemAttrType);
        goto cleanup;
    }

    void *value = zhash_lookup(self->hashtable, attributeKey);
    *_output = value;

    status = true;

cleanup:
    return status;
}

ItemAttributes *itemAttributesNew(
    float durability,
    float cooldown,
    char *memo,
    char *customName,
    char *crafterName,
    float pr,
    float reinforce_2)
{
    ItemAttributes *self;

    if ((self = malloc(sizeof(ItemAttributes))) == NULL) {
        return NULL;
    }

    if (!itemAttributesInit(self, durability, cooldown, memo, customName, crafterName, pr, reinforce_2)) {
        itemAttributesDestroy(&self);
        error("ItemAttributes failed to initialize.");
        return NULL;
    }

    return self;
}

bool itemAttributesInit(
    ItemAttributes *self,
    float durability,
    float cooldown,
    char *memo,
    char *customName,
    char *crafterName,
    float pr,
    float reinforce_2)
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

void itemAttributesFree(ItemAttributes *self) {
}

void itemAttributesDestroy(ItemAttributes **_self) {
    ItemAttributes *self = *_self;

    if (_self && self) {
        itemAttributesFree(self);
        free(self);
        *_self = NULL;
    }
}


bool stringAttributeInit(StringAttribute *self, ItemAttributeType attributeType, char *value) {
    self->attributeType = attributeType;
    self->value = value;

    return true;
}

bool floatAttributeInit(FloatAttribute *self, ItemAttributeType attributeType, float value) {
    self->attributeType = attributeType;
    self->value = value;

    return true;
}

bool attributeInit(Attribute *self, void *attribute, AttributeFormat format) {
    self->attribute = attribute;
    self->format = format;

    return true;
}

bool attributeArrayInit(AttributeArray *self, Attribute *attributes, int numAttributes) {
    self->attributes = attributes;
    self->numAttributes = numAttributes;

    return true;
}

bool itemEquipInit(
    ItemEquip *self,
    uint32_t itemType,
    uint64_t itemId,
    uint32_t inventoryIndex,
    AttributeArray *attributeArray)
{
    self->itemType = itemType;
    self->itemId = itemId;
    self->inventoryIndex = inventoryIndex;
    self->attributeArray = *attributeArray;

    return true;
}

size_t attributeArrayGetPacketSize(AttributeArray *attributeArray) {

    int totalSize = 0;
    int numAttributes = attributeArray->numAttributes;
    Attribute *attributes = attributeArray->attributes;

    for (int i = 0; i < numAttributes; i++) {
        AttributeFormat format = attributes[i].format;

        switch (format) {
            case FLOAT_ATTRIBUTE: {
                totalSize += FLOAT_ATTRIBUTE_SIZE;
                break;
            }
            case STRING_ATTRIBUTE: {
                StringAttribute *attribute = (StringAttribute *) (attributes[i].attribute);
                totalSize += stringAttributeGetSize(attribute);
                break;
            }
        }
    }

    return totalSize;
}

bool attributeArrayGetPacket(AttributeArray *attributeArray, char *packet) {

    int numAttributes = attributeArray->numAttributes;
    Attribute *attributes = attributeArray->attributes;

    PacketStream *packetStream = packetStreamNew(packet);

    for (int i = 0; i < numAttributes; i++) {
        AttributeFormat format = attributes[i].format;

        switch (format) {
            case FLOAT_ATTRIBUTE: {
                FloatAttribute *attribute = (FloatAttribute *) (attributes[i].attribute);

                size_t packetLength = FLOAT_ATTRIBUTE_SIZE;
                char packetBytes[packetLength];

                floatAttributeGetPacket(attribute, packetBytes);
                packetStreamAppend(packetStream, packetBytes, packetLength);
                break;
            }

            case STRING_ATTRIBUTE: {
                StringAttribute *attribute = (StringAttribute *) (attributes[i].attribute);
                size_t packetLength = stringAttributeGetSize(attribute);
                char packetBytes[packetLength];

                stringAttributeGetPacket(attribute, packetBytes);
                packetStreamAppend(packetStream, packetBytes, packetLength);
                break;
            }
        }
    }

    return true;
}

bool floatAttributeGetPacket(FloatAttribute *attribute, char *packetBytes) {

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

size_t stringAttributeGetSize(StringAttribute *attribute) {
    return STRING_ATTRIBUTE_HEADER_SIZE + strlen(attribute->value) + 1; //null terminator
}

bool stringAttributeGetPacket(StringAttribute *attribute, char *packetBytes) {

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

size_t itemEquipGetPacketSize(ItemEquip *itemEquip) {
    return ITEM_EQUIP_NOATTR_SIZE + attributeArrayGetPacketSize(&itemEquip->attributeArray);
}

size_t itemAttributesGetNum(ItemAttributes *itemAttributes) {
    size_t total = 0;

    if (itemAttributes->durability) {
        total++;
    }

    if (true) { // Cooldown is always present
        total++;
    }

    if (itemAttributes->memo) {
        total++;
    }

    if (itemAttributes->customName) {
        total++;
    }

    if (itemAttributes->crafterName) {
        total++;
    }

    if (itemAttributes->pr) {
        total++;
    }

    if (itemAttributes->reinforce_2) {
        total++;
    }

    return total;
}

bool itemAttributesGetPacket(ItemAttributes *itemAttributes, char *packet) {

    bool status = false;

    int numAttributes = itemAttributesGetNum(itemAttributes);

    Attribute attributes[numAttributes];
    AttributeArray attributeArray;
    attributeArrayInit(&attributeArray, attributes, numAttributes);
    int cur_idx = 0;

    float durability = itemAttributes->durability;
    float cooldown = itemAttributes->cooldown;
    char *memo = itemAttributes->memo;
    char *customName = itemAttributes->customName;
    char *crafterName = itemAttributes->crafterName;
    float pr = itemAttributes->pr;
    float reinforce_2 = itemAttributes->reinforce_2;

    if (durability) {
        FloatAttribute durabilityAttribute;
        if (!(floatAttributeInit(&durabilityAttribute, ITEM_ATTRIBUTE_DURABILITY, durability))) {
            error("Cannot initialize float attribute.");
            goto cleanup;
        }

        Attribute attribute;
        if (!(attributeInit(&attribute, (void *) &durabilityAttribute, FLOAT_ATTRIBUTE))) {
            error("Cannot initialize durability attribute.");
            goto cleanup;
        }
        attributeArray.attributes[cur_idx++] = attribute;
    }

    if (true) { // Cooldown is always present
        FloatAttribute cooldownAttribute;
        if (!(floatAttributeInit(&cooldownAttribute, ITEM_ATTRIBUTE_COOLDOWN, cooldown))) {
            error("Cannot initialize float attribute.");
            goto cleanup;
        }

        Attribute attribute;
        if (!(attributeInit(&attribute, (void *) &cooldownAttribute, FLOAT_ATTRIBUTE))) {
            error("Cannot initialize cooldown attribute.");
            goto cleanup;
        }

        attributeArray.attributes[cur_idx++] = attribute;
    }

    if (memo) {
        StringAttribute memoAttribute;
        if (!(stringAttributeInit(&memoAttribute, ITEM_ATTRIBUTE_MEMO, memo))) {
            error("Cannot initialize string attribute.");
            goto cleanup;
        }

        Attribute attribute;
        if (!(attributeInit(&attribute, (void *) &memoAttribute, STRING_ATTRIBUTE))) {
            error("Cannot initialize memo attribute.");
            goto cleanup;
        }

        attributeArray.attributes[cur_idx++] = attribute;
    }

    if (customName) {
        StringAttribute customNameAttribute;
        if (!(stringAttributeInit(&customNameAttribute, ITEM_ATTRIBUTE_CUSTOM_NAME, customName))) {
            error("Cannot initialize string attribute.");
            goto cleanup;
        }

        Attribute attribute;
        if (!(attributeInit(&attribute, (void *) &customNameAttribute, STRING_ATTRIBUTE))) {
            error("Cannot initialize custom name attribute.");
            goto cleanup;
        }
        attributeArray.attributes[cur_idx++] = attribute;
    }

    if (crafterName) {
        StringAttribute crafterNameAttribute;
        if (!(stringAttributeInit(&crafterNameAttribute, ITEM_ATTRIBUTE_CRAFTER_NAME, crafterName))) {
            error("Cannot initialize string attribute.");
            goto cleanup;
        }

        Attribute attribute;
        if (!(attributeInit(&attribute, (void *) &crafterNameAttribute, STRING_ATTRIBUTE))) {
            error("Cannot initialize crafter name attribute.");
            goto cleanup;
        }
        attributeArray.attributes[cur_idx++] = attribute;
    }

    if (pr) {
        FloatAttribute prAttribute;
        if (!(floatAttributeInit(&prAttribute, ITEM_ATTRIBUTE_PR, pr))) {
            error("Cannot initialize float attribute.");
            goto cleanup;
        }

        Attribute attribute;
        if (!(attributeInit(&attribute, (void *) &prAttribute, FLOAT_ATTRIBUTE))) {
            error("Cannot initialize pr attribute.");
            goto cleanup;
        }
        attributeArray.attributes[cur_idx++] = attribute;
    }

    if (reinforce_2) {
        FloatAttribute reinforce_2Attribute;
        if (!(floatAttributeInit(&reinforce_2Attribute, ITEM_ATTRIBUTE_REINFORCE_2, reinforce_2))) {
            error("Cannot initialize float attribute.");
            goto cleanup;
        }

        Attribute attribute;
        if (!(attributeInit(&attribute, (void *) &reinforce_2Attribute, FLOAT_ATTRIBUTE))) {
            error("Cannot initialize reinforce_2 attribute.");
            goto cleanup;
        }
        attributeArray.attributes[cur_idx++] = attribute;
    }

    if (!(attributeArrayGetPacket(&attributeArray, packet))) {
        error("Cannot get attribute list packet.");
        goto cleanup;
    }

    status = true;

cleanup:
    return status;
}

size_t itemAttributesGetPacketSize(ItemAttributes *itemAttributes) {

    size_t total = 0;

    if (itemAttributes->durability) {
        total += FLOAT_ATTRIBUTE_SIZE;
    }

    if (true) { // Cooldown is always present
        total += FLOAT_ATTRIBUTE_SIZE;
    }

    if (itemAttributes->memo) {
        total += STRING_ATTRIBUTE_HEADER_SIZE + strlen(itemAttributes->memo) + 1;
    }

    if (itemAttributes->customName) {
        total += STRING_ATTRIBUTE_HEADER_SIZE + strlen(itemAttributes->customName) + 1;
    }

    if (itemAttributes->crafterName) {
        total += STRING_ATTRIBUTE_HEADER_SIZE + strlen(itemAttributes->crafterName) + 1;
    }

    if (itemAttributes->pr) {
        total += FLOAT_ATTRIBUTE_SIZE;
    }

    if (itemAttributes->reinforce_2) {
        total += FLOAT_ATTRIBUTE_SIZE;
    }

    return total;
}
