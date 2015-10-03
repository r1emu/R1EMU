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

ItemConsumable *itemConsumableNew(Item *item, float cooldown) {
    ItemConsumable *self;

    if ((self = malloc(sizeof(ItemConsumable))) == NULL) {
        return NULL;
    }

    if (!itemConsumableInit(self, item, cooldown)) {
        itemConsumableDestroy(&self);
        error("ItemConsumable failed to initialize.");
        return NULL;
    }

    return self;
}

bool itemConsumableInit(ItemConsumable *self, Item *item, float cooldown) {
    memset(self, 0, sizeof(*self));

    memcpy(&self->item, item, sizeof(self->item));
    self->cooldown = cooldown;

    return true;
}

void itemConsumableFree(ItemConsumable *self) {
    itemFree(&self->item);
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
