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

#include "item_card.h"

extern inline float *itemCardGetLevel(ItemCard *self);
extern inline float *itemCardGetCooldown(ItemCard *self);
extern inline float *itemCardGetItemExp(ItemCard *self);

ItemCard *itemCardNew(Item *item) {
    ItemCard *self;

    if ((self = malloc(sizeof(ItemCard))) == NULL) {
        return NULL;
    }

    if (!itemCardInit(self, item)) {
        itemCardDestroy(&self);
        error("ItemCard failed to initialize.");
        return NULL;
    }

    return self;
}

bool itemCardInit(ItemCard *self, Item *item) {
    memset(self, 0, sizeof(*self));

    self->item = *item;

    return true;
}

void itemCardFree(ItemCard *self) {
    itemFree(&self->item);
    free(self->level);
    free(self->cooldown);
    free(self->itemExp);
}

void itemCardDestroy(ItemCard **_self) {
    ItemCard *self = *_self;

    if (_self && self) {
        itemCardFree(self);
        free(self);
        *_self = NULL;
    }
}

size_t itemCardGetPropertiesCPacketSize(ItemCard *self) {
    size_t size = 0;

    size += propertyFloatGetCPacketSize(); // level
    size += propertyFloatGetCPacketSize(); // cooldown
    size += propertyFloatGetCPacketSize(); // itemExp

    return size;
}

void itemCardGetPropertiesCPacket(ItemCard *self, PacketStream *stream) {
    propertyFloatGetCPacket(ITEM_CARD_PROPERTY_ID_LEVEL, self->level, stream);
    propertyFloatGetCPacket(ITEM_CARD_PROPERTY_ID_COOLDOWN, self->cooldown, stream);
    propertyFloatGetCPacket(ITEM_CARD_PROPERTY_ID_ITEM_EXP, self->itemExp, stream);
}
