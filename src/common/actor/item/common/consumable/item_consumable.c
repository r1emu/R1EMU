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

size_t itemConsumableGetPropertiesCPacketSize(ItemConsumable *self) {
    size_t size = 0;

    size += propertyFloatGetCPacketSize(); // cooldown

    return size;
}

void itemConsumableGetPropertiesCPacket(ItemConsumable *self, PacketStream *stream) {

    propertyFloatGetCPacket(ITEM_CONSUMABLE_PROPERTY_ID_COOLDOWN, self->cooldown, stream);
}
