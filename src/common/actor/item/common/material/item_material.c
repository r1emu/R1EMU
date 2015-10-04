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

ItemMaterial *itemMaterialNew(void) {
    ItemMaterial *self;

    if ((self = malloc(sizeof(ItemMaterial))) == NULL) {
        return NULL;
    }

    if (!itemMaterialInit(self)) {
        itemMaterialDestroy(&self);
        error("ItemMaterial failed to initialize.");
        return NULL;
    }

    return self;
}

bool itemMaterialInit(ItemMaterial *self) {
    memset(self, 0, sizeof(*self));

    return true;
}

void itemMaterialFree(ItemMaterial *self) {
    // TODO
}

void itemMaterialDestroy(ItemMaterial **_self) {
    ItemMaterial *self = *_self;

    if (_self && self) {
        itemMaterialFree(self);
        free(self);
        *_self = NULL;
    }
}

size_t itemMaterialGetPropertiesCPacketSize(ItemMaterial *self) {
    size_t size = 0;

    size += propertyFloatGetCPacketSize(); // cooldown

    return size;
}

void itemMaterialGetPropertiesCPacket(ItemMaterial *self, PacketStream *stream) {
    propertyFloatGetCPacket(ITEM_MATERIAL_PROPERTY_ID_COOLDOWN, self->cooldown, stream);
}
