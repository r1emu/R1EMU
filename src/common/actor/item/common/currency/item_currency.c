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

#include "item_currency.h"
extern inline float *itemCurrencyGetCooldown(ItemCurrency *self);

ItemCurrencyId itemCurrencyIds[] = {
    FOREACH_ITEM_CURRENCY(GENERATE_PROPERTY_ASSOC)
};

ItemCurrency *itemCurrencyNew(Item *item) {
    ItemCurrency *self;

    if ((self = malloc(sizeof(ItemCurrency))) == NULL) {
        return NULL;
    }

    if (!itemCurrencyInit(self, item)) {
        itemCurrencyDestroy(&self);
        error("ItemCurrency failed to initialize.");
        return NULL;
    }

    return self;
}

bool itemCurrencyInit(ItemCurrency *self, Item *item) {
    memset(self, 0, sizeof(*self));

    self->item = *item;

    return true;
}

void itemCurrencyFree(ItemCurrency *self) {
    itemFree(&self->item);
    free(self->cooldown);
}

void itemCurrencyDestroy(ItemCurrency **_self) {
    ItemCurrency *self = *_self;

    if (_self && self) {
        itemCurrencyFree(self);
        free(self);
        *_self = NULL;
    }
}

void itemCurrencyPrint(ItemCurrency *self) {
    PRINT_STRUCTURE {
        dbg("=== ItemCurrency %p ===", self);
        if (self->cooldown) {
            dbg("cooldown = %f", *self->cooldown);
        }
        itemPrint(&self->item);
    }
}

size_t itemCurrencyGetCPacketSize(ItemCurrency *self) {
    size_t size = 0;
    size += propertyFloatGetCPacketSize(self->cooldown);
    return size;
}

size_t itemCurrencyGetSPacketSize(ItemCurrency *self) {
    size_t size = 0;
    size += itemChildGetSPacketSize(&self->item);
    size += propertyFloatGetSPacketSize(self->cooldown);
    return size;
}

void itemCurrencySerializeCPacket(ItemCurrency *self, PacketStream *stream) {
    propertyFloatSerializeCPacket(ITEM_CURRENCY_PROPERTY_ID_COOLDOWN, self->cooldown, stream);
}

void itemCurrencySerializeSPacket(ItemCurrency *self, PacketStream *stream) {
    itemChildSerializeSPacket(&self->item, stream);
    propertyFloatSerializeSPacket(ITEM_CURRENCY_PROPERTY_ID_COOLDOWN, self->cooldown, stream);
}

bool itemCurrencyUnserializeSPacket(ItemCurrency *self, PacketStream *stream) {

    if (!(itemChildUnserializeSPacket(&self->item, stream))) {
        error("Cannot unserialize item packet.");
        return false;
    }
    if (!(propertyFloatUnserializeSPacket(ITEM_CURRENCY_PROPERTY_ID_COOLDOWN, &self->cooldown, stream))) {
        error("Cannot unserialize packet COOLDOWN.");
        return false;
    }

    return true;
}
