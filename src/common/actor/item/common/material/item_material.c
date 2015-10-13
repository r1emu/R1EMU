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

#include "item_material.h"

ItemMaterialId itemMaterialIds[] = {
    FOREACH_ITEM_MATERIAL(GENERATE_PROPERTY_ASSOC)
};

ItemMaterial *itemMaterialNew(Item *item) {
    ItemMaterial *self;

    if ((self = malloc(sizeof(ItemMaterial))) == NULL) {
        return NULL;
    }

    if (!itemMaterialInit(self, item)) {
        itemMaterialDestroy(&self);
        error("ItemMaterial failed to initialize.");
        return NULL;
    }

    return self;
}

bool itemMaterialInit(ItemMaterial *self, Item *item) {
    memset(self, 0, sizeof(*self));

    self->item = *item;

    return true;
}

void itemMaterialFree(ItemMaterial *self) {
    itemFree(&self->item);
    free(self->cooldown);
}

void itemMaterialDestroy(ItemMaterial **_self) {
    ItemMaterial *self = *_self;

    if (_self && self) {
        itemMaterialFree(self);
        free(self);
        *_self = NULL;
    }
}

void itemMaterialPrint(ItemMaterial *self) {
    PRINT_STRUCTURE {
        dbg("=== ItemMaterial %p ===", self);
        if (self->cooldown) {
            dbg("cooldown = %f", *self->cooldown);
        }
        itemPrint(&self->item);
    }
}

size_t itemMaterialGetCPacketSize(ItemMaterial *self) {
    size_t size = 0;
    size += propertyFloatGetCPacketSize(self->cooldown);
    return size;
}

size_t itemMaterialGetSPacketSize(ItemMaterial *self) {
    size_t size = 0;
    size += sizeof(ItemMaterialSPacket);
    size += propertyFloatGetSPacketSize(self->cooldown);
    return size;
}

void itemMaterialSerializeCPacket(ItemMaterial *self, PacketStream *stream) {
    propertyFloatSerializeCPacket(ITEM_MATERIAL_PROPERTY_ID_COOLDOWN, self->cooldown, stream);
}

void itemMaterialSerializeSPacket(ItemMaterial *self, PacketStream *stream) {
    propertyFloatSerializeSPacket(ITEM_MATERIAL_PROPERTY_ID_COOLDOWN, self->cooldown, stream);
}

bool itemMaterialUnserializeSPacket(ItemMaterial *self, PacketStream *stream) {

    if (!(propertyFloatUnserializeSPacket(ITEM_MATERIAL_PROPERTY_ID_COOLDOWN, &self->cooldown, stream))) {
        error("Cannot unserialize packet COOLDOWN.");
        return false;
    }

    return true;
}
