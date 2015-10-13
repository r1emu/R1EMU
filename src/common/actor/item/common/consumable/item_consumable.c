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

#include "item_consumable.h"
extern inline float *itemConsumableGetCooldown(ItemConsumable *self);

ItemConsumableId itemConsumableIds[] = {
    FOREACH_ITEM_CONSUMABLE(GENERATE_PROPERTY_ASSOC)
};

ItemConsumable *itemConsumableNew(Item *item) {
    ItemConsumable *self;

    if ((self = malloc(sizeof(ItemConsumable))) == NULL) {
        return NULL;
    }

    if (!itemConsumableInit(self, item)) {
        itemConsumableDestroy(&self);
        error("ItemConsumable failed to initialize.");
        return NULL;
    }

    return self;
}

bool itemConsumableInit(ItemConsumable *self, Item *item) {
    memset(self, 0, sizeof(*self));

    self->item = *item;

    return true;
}

void itemConsumableFree(ItemConsumable *self) {
    itemFree(&self->item);
    free(self->cooldown);
}

void itemConsumableDestroy(ItemConsumable **_self) {
    ItemConsumable *self = *_self;

    if (_self && self) {
        itemConsumableFree(self);
        free(self);
        *_self = NULL;
    }
}

void itemConsumablePrint(ItemConsumable *self) {
    PRINT_STRUCTURE {
        dbg("=== ItemConsumable %p ===", self);
        if (self->cooldown) {
            dbg("cooldown = %f", *self->cooldown);
        }
        itemPrint(&self->item);
    }
}

size_t itemConsumableGetCPacketSize(ItemConsumable *self) {
    size_t size = 0;
    size += propertyFloatGetCPacketSize(self->cooldown);
    return size;
}

size_t itemConsumableGetSPacketSize(ItemConsumable *self) {
    size_t size = 0;
    size += itemChildGetSPacketSize(&self->item);
    size += propertyFloatGetSPacketSize(self->cooldown);
    return size;
}

void itemConsumableSerializeCPacket(ItemConsumable *self, PacketStream *stream) {
    propertyFloatSerializeCPacket(ITEM_CONSUMABLE_PROPERTY_ID_COOLDOWN, self->cooldown, stream);
}

void itemConsumableSerializeSPacket(ItemConsumable *self, PacketStream *stream) {
    itemChildSerializeSPacket(&self->item, stream);
    propertyFloatSerializeSPacket(ITEM_CONSUMABLE_PROPERTY_ID_COOLDOWN, self->cooldown, stream);
}

bool itemConsumableUnserializeSPacket(ItemConsumable *self, PacketStream *stream) {

    if (!(itemChildUnserializeSPacket(&self->item, stream))) {
        error("Cannot unserialize item packet.");
        return false;
    }
    if (!(propertyFloatUnserializeSPacket(ITEM_CONSUMABLE_PROPERTY_ID_COOLDOWN, &self->cooldown, stream))) {
        error("Cannot unserialize packet COOLDOWN.");
        return false;
    }

    return true;
}
