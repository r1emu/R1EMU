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

#include "item_accessory.h"

extern inline float *itemAccessoryGetPr(ItemAccessory *self);
extern inline float *itemAccessoryGetCooldown(ItemAccessory *self);

ItemAccessoryId itemAccessoryIds[] = {
    FOREACH_ITEM_ACCESSORY(GENERATE_PROPERTY_ASSOC)
};

ItemAccessory *itemAccessoryNew(Item *item) {
    ItemAccessory *self;

    if ((self = malloc(sizeof(ItemAccessory))) == NULL) {
        return NULL;
    }

    if (!itemAccessoryInit(self, item)) {
        itemAccessoryDestroy(&self);
        error("ItemAccessory failed to initialize.");
        return NULL;
    }

    return self;
}

bool itemAccessoryInit(ItemAccessory *self, Item *item) {
    memset(self, 0, sizeof(*self));

    if (!(itemEquipableInit(&self->equipable, item))) {
        error("Cannot initialize an equipable item.");
        return false;
    }

    return true;
}

void itemAccessoryFree(ItemAccessory *self) {
    itemEquipableFree(&self->equipable);
    free(self->pr);
    free(self->cooldown);
}

void itemAccessoryDestroy(ItemAccessory **_self) {
    ItemAccessory *self = *_self;

    if (_self && self) {
        itemAccessoryFree(self);
        free(self);
        *_self = NULL;
    }
}

void itemAccessoryPrint(ItemAccessory *self) {
    PRINT_STRUCTURE {
        dbg("=== ItemAccessory %p ===", self);
        if (self->pr) {
            dbg("pr = %f", *self->pr);
        }
        if (self->cooldown) {
            dbg("cooldown = %f", *self->cooldown);
        }
        itemEquipablePrint(&self->equipable);
    }
}

size_t itemAccessoryGetCPacketSize(ItemAccessory *self) {
    size_t size = 0;
    size += propertyFloatGetCPacketSize(self->pr);
    size += propertyFloatGetCPacketSize(self->cooldown);
    return size;
}

size_t itemAccessoryGetSPacketSize(ItemAccessory *self) {
    size_t size = 0;
    size += itemEquipableGetSPacketSize(&self->equipable);
    size += propertyFloatGetSPacketSize(self->pr);
    size += propertyFloatGetSPacketSize(self->cooldown);
    return size;
}

void itemAccessorySerializeCPacket(ItemAccessory *self, PacketStream *stream) {
    propertyFloatSerializeCPacket(ITEM_ACCESSORY_PROPERTY_ID_PR, self->pr, stream);
    propertyFloatSerializeCPacket(ITEM_ACCESSORY_PROPERTY_ID_COOLDOWN, self->cooldown, stream);
}

void itemAccessorySerializeSPacket(ItemAccessory *self, PacketStream *stream) {
    itemEquipableSerializeSPacket(&self->equipable, stream);
    propertyFloatSerializeSPacket(ITEM_ACCESSORY_PROPERTY_ID_PR, self->pr, stream);
    propertyFloatSerializeSPacket(ITEM_ACCESSORY_PROPERTY_ID_COOLDOWN, self->cooldown, stream);
}

size_t itemAccessoryUnserializeSPacket(ItemAccessory *self, PacketStream *stream) {

    if (!(itemEquipableUnserializeSPacket(&self->equipable, stream))) {
        error("Cannot unserialize equipable packet.");
        return false;
    }
    if (!(propertyFloatUnserializeSPacket(ITEM_ACCESSORY_PROPERTY_ID_PR, &self->cooldown, stream))) {
        error("Cannot unserialize packet PR.");
        return false;
    }
    if (!(propertyFloatUnserializeSPacket(ITEM_ACCESSORY_PROPERTY_ID_COOLDOWN, &self->cooldown, stream))) {
        error("Cannot unserialize packet COOLDOWN.");
        return false;
    }

    return true;
}
