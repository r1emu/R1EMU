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

ItemCardId itemCardIds[] = {
    FOREACH_ITEM_CARD(GENERATE_PROPERTY_ASSOC)
};

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

void itemCardPrint(ItemCard *self) {
    PRINT_STRUCTURE {
        dbg("=== ItemCard %p ===", self);
        if (self->level) {
            dbg("level = %f", *self->level);
        }
        if (self->cooldown) {
            dbg("cooldown = %f", *self->cooldown);
        }
        if (self->itemExp) {
            dbg("itemExp = %f", *self->itemExp);
        }
        itemPrint(&self->item);
    }
}

size_t itemCardGetCPacketSize(ItemCard *self) {
    size_t size = 0;
    size += propertyFloatGetCPacketSize(self->level);
    size += propertyFloatGetCPacketSize(self->cooldown);
    size += propertyFloatGetCPacketSize(self->itemExp);
    return size;
}

size_t itemCardGetSPacketSize(ItemCard *self) {
    size_t size = 0;
    size += sizeof(ItemCardSPacket);
    size += propertyFloatGetSPacketSize(self->level);
    size += propertyFloatGetSPacketSize(self->cooldown);
    size += propertyFloatGetSPacketSize(self->itemExp);
    return size;
}

void itemCardSerializeCPacket(ItemCard *self, PacketStream *stream) {
    propertyFloatSerializeCPacket(ITEM_CARD_PROPERTY_ID_LEVEL, self->level, stream);
    propertyFloatSerializeCPacket(ITEM_CARD_PROPERTY_ID_COOLDOWN, self->cooldown, stream);
    propertyFloatSerializeCPacket(ITEM_CARD_PROPERTY_ID_ITEM_EXP, self->itemExp, stream);
}

void itemCardSerializeSPacket(ItemCard *self, PacketStream *stream) {
    propertyFloatSerializeSPacket(ITEM_CARD_PROPERTY_ID_LEVEL, self->level, stream);
    propertyFloatSerializeSPacket(ITEM_CARD_PROPERTY_ID_COOLDOWN, self->cooldown, stream);
    propertyFloatSerializeSPacket(ITEM_CARD_PROPERTY_ID_ITEM_EXP, self->itemExp, stream);
}

bool itemCardUnserializeSPacket(ItemCard *self, PacketStream *stream) {

    if (!(propertyFloatUnserializeSPacket(ITEM_CARD_PROPERTY_ID_LEVEL, &self->level, stream))) {
        error("Cannot unserialize packet LEVEL.");
        return false;
    }
    if (!(propertyFloatUnserializeSPacket(ITEM_CARD_PROPERTY_ID_COOLDOWN, &self->cooldown, stream))) {
        error("Cannot unserialize packet COOLDOWN.");
        return false;
    }
    if (!(propertyFloatUnserializeSPacket(ITEM_CARD_PROPERTY_ID_ITEM_EXP, &self->itemExp, stream))) {
        error("Cannot unserialize packet ITEM_EXP.");
        return false;
    }

    return true;
}
