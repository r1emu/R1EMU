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

// attribute ID size
typedef uint16_t AttributeId;

/** Attribute packets structure */
#pragma pack(push, 1)
typedef struct ItemAttributeFloatPacket {
    AttributeId attributeId;
    float value;
} ItemAttributeFloatPacket;
#pragma pack(pop)

#define DECLARE_ItemAttributeStringPacket(x) \
typedef struct ItemAttributeStringPacket {   \
    AttributeId attributeId;                 \
    uint16_t valueSize;                      \
    uint8_t  value[x];                       \
} ItemAttributeStringPacket;                 \

/** ItemAttribute formats */
typedef enum AttributeFormat {
    FLOAT_ATTRIBUTE,
    STRING_ATTRIBUTE,
} AttributeFormat;

/** ItemAttribute data */
typedef struct ItemAttribute {
    AttributeFormat format;
    AttributeId attributeId;
    void *value;
} ItemAttribute;

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
static void itemAttributeGetPacket(ItemAttribute *self, PacketStream *packetStream);
static size_t itemAttributeGetPacketSize(ItemAttribute *self);
static void itemAttributeFree(ItemAttribute *self);
static void itemAttributeDestroy(ItemAttribute **_self);

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

bool itemAttributeInit(ItemAttribute *self, AttributeId attributeId, AttributeFormat format, void *value) {

    bool status = false;

    self->format = format;

    switch (format) {
        case FLOAT_ATTRIBUTE:
            self->value = floatdup(value);
            break;

        case STRING_ATTRIBUTE:
            self->value = strdup(value);
            break;

        default:
            error("Unknown attribute format. Please complete this function.");
            goto cleanup;
    }

    status = true;

cleanup:
    return status;
}

size_t itemAttributesGetPacketSize(ItemAttributes *self) {

    size_t size = 0;

    for (ItemAttribute *attr = zhash_first(self->hashtable); attr != NULL; attr = zhash_next(self->hashtable)) {
        size += itemAttributeGetPacketSize(attr);
    }

    return size;
}

void itemAttributeFloatGetPacket(ItemAttribute *self, PacketStream *packetStream) {

    ItemAttributeFloatPacket attributePacket = {
        .attributeId = self->attributeId,
        .value = *(float *) self->value
    };

    packetStreamAppend(packetStream, &attributePacket, sizeof(attributePacket));
}

void itemAttributeStringGetPacket(ItemAttribute *self, PacketStream *packetStream) {

    size_t valueSize = strlen(self->value) + 1;

    #pragma pack(push, 1)
    DECLARE_ItemAttributeStringPacket(valueSize);
    #pragma pack(pop)

    ItemAttributeStringPacket attributePacket;
    attributePacket.attributeId = self->attributeId;
    attributePacket.valueSize = strlen(self->value);
    strncpy(attributePacket.value, self->value, valueSize);

    packetStreamAppend(packetStream, &attributePacket, sizeof(attributePacket));
}

void itemAttributeGetPacket(ItemAttribute *self, PacketStream *packetStream) {

    switch (self->format) {

        case FLOAT_ATTRIBUTE:
            itemAttributeFloatGetPacket(self, packetStream);
            break;

        case STRING_ATTRIBUTE:
            itemAttributeStringGetPacket(self, packetStream);
            break;

        default:
            warning("Unknown attribute format. Please complete this function.");
            break;
    }
}

void itemAttributesGetPacket(ItemAttributes *self, PacketStream *packetStream) {

    for (ItemAttribute *value = zhash_first(self->hashtable); value != NULL; value = zhash_next(self->hashtable)) {
        itemAttributeGetPacket(value, packetStream);
    }
}

static size_t itemAttributeFloatGetPacketSize(ItemAttribute *self) {
    return sizeof(ItemAttributeFloatPacket);
}

static size_t itemAttributeStringGetPacketSize(ItemAttribute *self) {

    #pragma pack(push, 1)
    DECLARE_ItemAttributeStringPacket(strlen(self->value));
    #pragma pack(pop)

    return sizeof(ItemAttributeStringPacket);
}

static size_t itemAttributeGetPacketSize(ItemAttribute *self) {

    size_t size = 0;

    switch (self->format) {

        case FLOAT_ATTRIBUTE:
            size = itemAttributeFloatGetPacketSize(self);
            break;

        case STRING_ATTRIBUTE:
            size = itemAttributeStringGetPacketSize(self);
            break;

        default:
            warning("Unknown attribute format. Please complete this function.");
            break;
    }

    return size;
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
