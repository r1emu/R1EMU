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
#include "item_attribute.h"
#include "common/utils/memory.h"

/** Attribute keys */
#define ITEM_ATTRIBUTE_DURABILITY "durability"
#define ITEM_ATTRIBUTE_PR "pr"
#define ITEM_ATTRIBUTE_COOLDOWN "cooldown"
#define ITEM_ATTRIBUTE_REINFORCE_2 "reinforce_2"
#define ITEM_ATTRIBUTE_MEMO "memo"
#define ITEM_ATTRIBUTE_CUSTOM_NAME "custom_name"
#define ITEM_ATTRIBUTE_CRAFTER_NAME "crafter_name"

/** Attribute structure */
#pragma pack(push, 1)
typedef struct ItemAttributeFloat {
    float value;
} ItemAttributeFloat;
#pragma pack(pop)

typedef ItemAttributeFloat ItemAttributeFloatCPacket;
typedef ItemAttributeFloat ItemAttributeFloatSPacket;

#define DECLARE_ItemAttributeString(x)        \
typedef struct ItemAttributeString {          \
    uint16_t valueSize;                       \
    uint8_t  value[x];                        \
} ItemAttributeString;

#define DECLARE_ItemAttributeStringCPacket(x) \
typedef struct ItemAttributeStringCPacket {   \
    uint16_t valueSize;                       \
    uint8_t  value[x];                        \
} ItemAttributeStringCPacket;

#define DECLARE_ItemAttributeStringSPacket(x) \
typedef struct ItemAttributeStringSPacket {   \
    uint16_t valueSize;                       \
    uint8_t  value[x];                        \
} ItemAttributeStringSPacket;

/** key <-> format mapping */
struct ItemAttributeKeyFormat {
    char *key;
    AttributeFormat format;
} itemAttributeKeyFormats [] = {
    [ITEM_ATTRIBUTE_ID_DURABILITY]   = {.key = ITEM_ATTRIBUTE_DURABILITY,   .format = FLOAT_ATTRIBUTE},
    [ITEM_ATTRIBUTE_ID_PR]           = {.key = ITEM_ATTRIBUTE_PR,           .format = FLOAT_ATTRIBUTE},
    [ITEM_ATTRIBUTE_ID_COOLDOWN]     = {.key = ITEM_ATTRIBUTE_COOLDOWN,     .format = FLOAT_ATTRIBUTE},
    [ITEM_ATTRIBUTE_ID_REINFORCE_2]  = {.key = ITEM_ATTRIBUTE_REINFORCE_2,  .format = FLOAT_ATTRIBUTE},
    [ITEM_ATTRIBUTE_ID_MEMO]         = {.key = ITEM_ATTRIBUTE_MEMO,         .format = STRING_ATTRIBUTE},
    [ITEM_ATTRIBUTE_ID_CUSTOM_NAME]  = {.key = ITEM_ATTRIBUTE_CUSTOM_NAME,  .format = STRING_ATTRIBUTE},
    [ITEM_ATTRIBUTE_ID_CRAFTER_NAME] = {.key = ITEM_ATTRIBUTE_CRAFTER_NAME, .format = STRING_ATTRIBUTE}
};

/** Private ItemAttribute functions */
static ItemAttribute *itemAttributeNew(AttributeId attributeId, AttributeFormat format, void *value);
static bool itemAttributeInit(ItemAttribute *self, AttributeId attributeId, AttributeFormat format, void *value);
static void itemAttributeCPacket(ItemAttribute *self, PacketStream *packetStream);
static size_t itemAttributeGetCPacketSize(ItemAttribute *self);
static void itemAttributeFree(ItemAttribute *self);
static void itemAttributeDestroy(ItemAttribute **_self);
static void itemAttributeSPacket(ItemAttribute *self, PacketStream *stream);
static void itemAttributeFloatSPacket(ItemAttribute *self, PacketStream *stream);
static void itemAttributeStringSPacket(ItemAttribute *self, PacketStream *stream);

static size_t itemAttributeGetSPacketSize(ItemAttribute *self);

ItemAttributes *itemAttributesNew(void) {
    ItemAttributes *self;

    if ((self = malloc(sizeof(ItemAttributes))) == NULL) {
        return NULL;
    }

    if (!itemAttributesInit(self)) {
        itemAttributesDestroy(&self);
        error("ItemAttributes failed to initialize.");
        return NULL;
    }

    return self;
}

bool itemAttributesInit(ItemAttributes *self) {

    if (!(self->hashtable = zhash_new())) {
        error("Cannot allocate a new hashtable.");
        return false;
    }

    return true;
}

ItemAttribute *itemAttributeNew(AttributeId attributeId, AttributeFormat format, void *value) {
    ItemAttribute *self;

    if ((self = malloc(sizeof(ItemAttribute))) == NULL) {
        return NULL;
    }

    if (!itemAttributeInit(self, attributeId, format, value)) {
        itemAttributeDestroy(&self);
        error("ItemAttribute failed to initialize.");
        return NULL;
    }

    return self;
}

ItemAttributeFloat *itemAttributeFloatNew(float value) {

    ItemAttributeFloat *self = NULL;

    if (!(self = malloc(sizeof(ItemAttributeFloat)))) {
        error("Cannot allocate an item attribute float.");
        return NULL;
    }

    self->value = value;

    return self;
}

void *itemAttributeStringNew(size_t valueSize, char *value) {

    DECLARE_ItemAttributeString(valueSize);
    ItemAttributeString *self = NULL;

    if (!(self = malloc(sizeof(ItemAttributeString)))) {
        error("Cannot allocate an item attribute float.");
        return NULL;
    }

    self->valueSize = valueSize;
    strncpy(self->value, value, valueSize);

    return self;
}

bool itemAttributeInit(ItemAttribute *self, AttributeId attributeId, AttributeFormat format, void *value) {

    memset(self, 0, sizeof(*self));
    bool status = false;

    self->format = format;

    switch (format) {
        case FLOAT_ATTRIBUTE: {
            if (!(self->value = itemAttributeFloatNew(*(float *) value))) {
                error("Cannot allocate a float item attribute.");
                goto cleanup;
            }
            break;
        }

        case STRING_ATTRIBUTE: {
            size_t valueSize = strlen(value) + 1;
            if (!(self->value = itemAttributeStringNew(valueSize, value))) {
                error("Cannot allocate a string item attribute.");
                goto cleanup;
            }
            break;
        }

        default:
            error("Unknown attribute format. Please complete this function.");
            goto cleanup;
    }

    status = true;

cleanup:
    return status;
}

size_t itemAttributesGetCPacketSize(ItemAttributes *self) {

    size_t packetSize = 0;

    for (ItemAttribute *attr = zhash_first(self->hashtable); attr != NULL; attr = zhash_next(self->hashtable)) {
        packetSize += itemAttributeGetCPacketSize(attr);
    }

    return packetSize;
}

void itemAttributeFloatCPacket(ItemAttribute *self, PacketStream *stream) {

    ItemAttributeFloat *attributePacket = self->value;
    packetStreamIn(stream, &attributePacket->value);
}

void itemAttributeStringCPacket(ItemAttribute *self, PacketStream *stream) {

    uint16_t valueSize = *(uint16_t *) self->value; // StringPacket begins with size

    #pragma pack(push, 1)
    DECLARE_ItemAttributeString(valueSize);
    #pragma pack(pop)

    ItemAttributeString *attributePacket = self->value;

    packetStreamIn(stream, &attributePacket->valueSize);
    packetStreamIn(stream, attributePacket->value);
}

void itemAttributeCPacket(ItemAttribute *self, PacketStream *stream) {

    switch (self->format) {

        case FLOAT_ATTRIBUTE:
            itemAttributeFloatCPacket(self, stream);
            break;

        case STRING_ATTRIBUTE:
            itemAttributeStringCPacket(self, stream);
            break;

        default:
            warning("Unknown attribute format. Please complete this function.");
            break;
    }
}

void itemAttributesCPacket(ItemAttributes *self, PacketStream *stream) {

    for (ItemAttribute *value = zhash_first(self->hashtable); value != NULL; value = zhash_next(self->hashtable)) {
        itemAttributeCPacket(value, stream);
    }
}

static size_t itemAttributeFloatGetCPacketSize(ItemAttribute *self) {
    return sizeof(ItemAttributeFloatCPacket);
}

static size_t itemAttributeStringGetCPacketSize(ItemAttribute *self) {

    #pragma pack(push, 1)
    DECLARE_ItemAttributeStringCPacket(strlen(self->value));
    #pragma pack(pop)

    return sizeof(ItemAttributeStringCPacket);
}

static size_t itemAttributeGetCPacketSize(ItemAttribute *self) {

    size_t packetSize = 0;

    switch (self->format) {

        case FLOAT_ATTRIBUTE:
            packetSize = itemAttributeFloatGetCPacketSize(self);
            break;

        case STRING_ATTRIBUTE:
            packetSize = itemAttributeStringGetCPacketSize(self);
            break;

        default:
            warning("Unknown attribute format. Please complete this function.");
            break;
    }

    return packetSize;
}

bool itemAttributesGet(ItemAttributes *self, ItemAttributeId itemAttrId, void **_output) {

    bool status = false;
    char *attributeKey = NULL;

    if (!(attributeKey = itemAttributeKeyFormats[itemAttrId].key)) {
        error("No attribute key for '%d'", itemAttrId);
        goto cleanup;
    }

    ItemAttribute *itemAttribute = zhash_lookup(self->hashtable, attributeKey);
    *_output = itemAttribute->value;

    status = true;

cleanup:
    return status;
}

bool itemAttributesUpdate(ItemAttributes *self, ItemAttributeId itemAttrId, void *value) {

    bool status = false;
    char *attributeKey = NULL;

    if (!(attributeKey = itemAttributeKeyFormats[itemAttrId].key)) {
        error("No attribute key for '%d'", itemAttrId);
        goto cleanup;
    }

    ItemAttribute *itemAttribute = NULL;

    if (!(itemAttribute = zhash_lookup(self->hashtable, attributeKey))) {
        error("Cannot find itemAttribute %d.", itemAttrId);
        goto cleanup;
    }

    // free old value
    free(itemAttribute->value);

    // replace it with the new one
    if (!(itemAttributeInit(itemAttribute, itemAttrId, itemAttributeKeyFormats[itemAttrId].format, value))) {
        error("Cannot remplace the new item attribute.");
        goto cleanup;
    }

    status = true;

cleanup:
    return status;
}

bool itemAttributesAdd(ItemAttributes *self, ItemAttributeId itemAttrId, void *value) {

    bool status = false;
    char *attributeKey = NULL;
    ItemAttribute *itemAttribute = NULL;

    if (!(attributeKey = itemAttributeKeyFormats[itemAttrId].key)) {
        error("No attribute key for '%d'", itemAttrId);
        goto cleanup;
    }

    if (!(itemAttribute = itemAttributeNew(itemAttrId, itemAttributeKeyFormats[itemAttrId].format, value))) {
        error("Cannot allocate a new item attribute.");
        goto cleanup;
    }

    if (zhash_insert(self->hashtable, attributeKey, itemAttribute) != 0) {
        error("Cannot insert the item attribute %d", itemAttrId);
        goto cleanup;
    }

    if (!(zhash_freefn(self->hashtable, attributeKey, (zhash_free_fn *) itemAttributeFree))) {
        error("Cannot set item attribute destructor.");
        goto cleanup;
    }

    status = true;

cleanup:
    if (!status) {
        itemAttributeDestroy(&itemAttribute);
    }
    return status;
}

bool itemAttributesRemove(ItemAttributes *self, ItemAttributeId itemAttrId) {

    bool status = false;
    char *attributeKey = NULL;

    if (!(attributeKey = itemAttributeKeyFormats[itemAttrId].key)) {
        error("No attribute key for '%d'", itemAttrId);
        goto cleanup;
    }

    zhash_delete(self->hashtable, attributeKey);

    status = true;

cleanup:
    return status;
}

void itemAttributesFree(ItemAttributes *self) {
    zhash_destroy(&self->hashtable);
}

void itemAttributesDestroy(ItemAttributes **_self) {
    ItemAttributes *self = *_self;

    if (_self && self) {
        itemAttributesFree(self);
        free(self);
        *_self = NULL;
    }
}

void itemAttributeFree(ItemAttribute *self) {
    free(self->value);
    free(self);
}

void itemAttributeDestroy(ItemAttribute **_self) {
    ItemAttribute *self = *_self;

    if (_self && self) {
        itemAttributeFree(self);
        *_self = NULL;
    }
}

static size_t itemAttributeFloatGetSPacketSize(ItemAttribute *self) {
    return sizeof(ItemAttributeFloatSPacket);
}

static size_t itemAttributeStringGetSPacketSize(ItemAttribute *self) {

    size_t valueSize = strlen(self->value) + 1;

    #pragma pack(push, 1)
    DECLARE_ItemAttributeStringSPacket(valueSize);
    #pragma pack(pop)

    return sizeof(ItemAttributeStringSPacket);
}

static size_t itemAttributeGetSPacketSize(ItemAttribute *self) {

    size_t packetSize = 0;

    packetSize += sizeof(ItemAttributeSPacket);

    switch (self->format) {

        case FLOAT_ATTRIBUTE:
            packetSize += itemAttributeFloatGetSPacketSize(self);
            break;

        case STRING_ATTRIBUTE:
            packetSize += itemAttributeStringGetSPacketSize(self);
            break;

        default:
            warning("Unknown attribute format. Please complete this function.");
            break;
    }

    return packetSize;
}

static void itemAttributeStringSPacket(ItemAttribute *self, PacketStream *stream) {

    uint16_t valueSize = *(uint16_t *) self->value; // StringPacket begins with size

    #pragma pack(push, 1)
    DECLARE_ItemAttributeString(valueSize);
    #pragma pack(pop)

    ItemAttributeString *attributePacket = self->value;

    packetStreamIn(stream, &attributePacket->valueSize);
    packetStreamIn(stream, attributePacket->value);
}

size_t itemAttributesGetSPacketSize(ItemAttributes *self) {

    size_t packetSize = 0;

    packetSize += sizeof(ItemAttributesSPacket);

    // Get size of all attributes
    for (ItemAttribute *attr = zhash_first(self->hashtable); attr != NULL; attr = zhash_next(self->hashtable)) {
        packetSize += itemAttributeGetSPacketSize(attr);
    }

    return packetSize;
}

void itemAttributesSPacket(ItemAttributes *self, PacketStream *stream) {

    size_t attributesCount = zhash_size(self->hashtable);
    packetStreamIn(stream, &attributesCount);

    // Write all attributes
    for (ItemAttribute *attr = zhash_first(self->hashtable); attr != NULL; attr = zhash_next(self->hashtable)) {
        itemAttributeSPacket(attr, stream);
    }
}

static void itemAttributeSPacket(ItemAttribute *self, PacketStream *stream) {

    packetStreamIn(stream, &self->format);
    packetStreamIn(stream, &self->attributeId);

    switch (self->format) {

        case FLOAT_ATTRIBUTE:
            itemAttributeFloatSPacket(self, stream);
            break;

        case STRING_ATTRIBUTE:
            itemAttributeStringSPacket(self, stream);
            break;

        default:
            warning("Unknown attribute format. Please complete this function.");
            break;
    }
}

static void itemAttributeFloatSPacket(ItemAttribute *self, PacketStream *stream) {

    ItemAttributeFloat *attributePacket = self->value;
    packetStreamIn(stream, &attributePacket->value);
}

static bool itemAttributeFloatUnpacket(ItemAttribute *self, PacketStream *stream) {

    ItemAttributeFloat *attributePacket = self->value;
    packetStreamOut(stream, &attributePacket->value);
    return true;
}

static bool itemAttributeStringUnpacket(ItemAttribute *self, PacketStream *stream) {

    bool status = false;
    uint16_t valueSize;
    packetStreamOut(stream, &valueSize);

    #pragma pack(push, 1)
    DECLARE_ItemAttributeString(valueSize);
    #pragma pack(pop)

    ItemAttributeString *attr = self->value;

    if (valueSize != attr->valueSize) {
        if (!(attr = realloc(attr, sizeof(ItemAttributeString)))) {
            error("Cannot realloc an attribute string of size '%d'", sizeof(ItemAttributeString));
            goto cleanup;
        }
    }

    attr->valueSize = valueSize;
    packetStreamOut(stream, attr->value);

    status = true;

cleanup:
    return status;
}

static bool itemAttributeUnpacket(ItemAttribute *self, PacketStream *stream) {

    bool status = false;
    packetStreamOut(stream, &self->format);
    packetStreamOut(stream, &self->attributeId);

    switch (self->format) {

        case FLOAT_ATTRIBUTE:
            if (!(itemAttributeFloatUnpacket(self, stream))) {
                error("Cannot unpack float item attribute");
                goto cleanup;
            }
            break;

        case STRING_ATTRIBUTE:
            if (!(itemAttributeStringUnpacket(self, stream))) {
                error("Cannot unpacket string item attribute");
                goto cleanup;
            }
            break;

        default:
            warning("Unknown attribute format. Please complete this function.");
            break;
    }

    status = true;

cleanup:
    return status;
}

bool itemAttributesUnpacket(ItemAttributes *self, PacketStream *stream) {

    bool status = false;
    size_t attributesCount;

    packetStreamOut(stream, &attributesCount);

    for (size_t i = 0; i < attributesCount; i++) {
        ItemAttribute attr;
        if (!(itemAttributeUnpacket(&attr, stream))) {
            error("Cannot unpack item attribute.");
            goto cleanup;
        }
    }

    status = true;

cleanup:
    return status;
}
