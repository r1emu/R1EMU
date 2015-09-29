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

// ------ Structure declaration -------

// ------ Static declaration -------

// ------ Extern function implementation -------

Item *itemNew(uint64_t itemId, uint32_t itemType, uint32_t amount, uint32_t index) {

    Item *self;

    if ((self = calloc(1, sizeof(Item))) == NULL) {
        return NULL;
    }

    if (!itemInit(self, itemId, itemType, amount, index)) {
        itemDestroy(&self);
        error("Item failed to initialize.");
        return NULL;
    }

    return self;
}

bool itemInit(Item *self, uint64_t itemId, uint32_t itemType, uint32_t amount, uint32_t index) {

    self->itemId = itemId;
    self->itemType = itemType;
    self->amount = amount;
    self->index = index;

    if (!(itemAttributesInit(&self->attributes))) {
        error("Cannot initialize item attributes.");
        return false;
    }

    return true;
}

bool itemDup(Item *self, Item **out) {

    bool status = false;
    Item *dup = NULL;

    if (!(dup = itemNew(self->itemId, self->itemType, self->amount, self->index))) {
        error("Cannot duplicate a new item.");
        goto cleanup;
    }

    dup->attributes = self->attributes;

    status = true;
    *out = dup;

cleanup:
    return status;
}

bool itemAddAttribute(Item *self, ItemAttributeId itemAttrId, void *value) {

    if (!(itemAttributesAdd(&self->attributes, itemAttrId, value))) {
        error("Cannot add item attribute.");
        return false;
    }

    return true;
}

bool itemGetAttribute(Item *self, ItemAttributeId itemAttrId, void **value) {

    if (!(itemAttributesGet(&self->attributes, itemAttrId, value))) {
        error("Cannot get item attribute.");
        return false;
    }

    return true;
}

bool itemUpdateAttribute(Item *self, ItemAttributeId itemAttrId, void *value) {

    if (!(itemAttributesUpdate(&self->attributes, itemAttrId, value))) {
        error("Cannot update item attribute.");
        return false;
    }

    return true;
}

bool itemRemoveAttribute(Item *self, ItemAttributeId itemAttrId) {

    if (!(itemAttributesRemove(&self->attributes, itemAttrId))) {
        error("Cannot delete item attribute.");
        return false;
    }

    return true;
}

void itemGenKey(uint64_t itemIdKey, ItemKey itemKey) {
    snprintf(itemKey, ITEM_KEY_SIZE, "%I64x", itemIdKey);
}

void itemFree(Item *self) {
    itemAttributesFree(&self->attributes);
    free(self);
}

void itemDestroy(Item **_self) {

    Item *self = *_self;

    if (_self && self) {
        itemFree(self);
        *_self = NULL;
    }
}

size_t itemGetPacketSize(Item *self) {
    size_t packetSize = 0;

    packetSize += sizeof(ItemSPacket);
    packetSize += itemAttributesGetPacketSize(&self->attributes);

    return packetSize;
}

void itemSerialize(Item *self, PacketStream *stream) {
    packetStreamIn(stream, &self->itemId);
    packetStreamIn(stream, &self->itemType);
    packetStreamIn(stream, &self->amount);
    packetStreamIn(stream, &self->index);
    packetStreamIn(stream, &self->itemCategory);
    itemAttributesSerialize(&self->attributes, stream);
}

bool itemUnserialize(Item *self, PacketStream *stream) {
    packetStreamOut(stream, &self->itemId);
    packetStreamOut(stream, &self->itemType);
    packetStreamOut(stream, &self->amount);
    packetStreamOut(stream, &self->index);
    packetStreamOut(stream, &self->itemCategory);

    if (!(itemAttributesUnserialize(&self->attributes, stream))) {
        error("Cannot unserialize the item attribute.");
        return false;
    }

    return true;
}
