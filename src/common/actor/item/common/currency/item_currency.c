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

ItemCurrency *itemCurrencyNew(void) {
    ItemCurrency *self;

    if ((self = malloc(sizeof(ItemCurrency))) == NULL) {
        return NULL;
    }

    if (!itemCurrencyInit(self)) {
        itemCurrencyDestroy(&self);
        error("ItemCurrency failed to initialize.");
        return NULL;
    }

    return self;
}

bool itemCurrencyInit(ItemCurrency *self) {
    memset(self, 0, sizeof(*self));

    return true;
}

void itemCurrencyFree(ItemCurrency *self) {
    // TODO
}

void itemCurrencyDestroy(ItemCurrency **_self) {
    ItemCurrency *self = *_self;

    if (_self && self) {
        itemCurrencyFree(self);
        free(self);
        *_self = NULL;
    }
}

size_t itemCurrencyGetPropertiesCPacketSize(ItemCurrency *self) {
    size_t size = 0;

    size += propertyFloatGetCPacketSize(); // cooldown

    return size;
}

void itemCurrencyGetPropertiesCPacket(ItemCurrency *self, PacketStream *stream) {
    propertyFloatGetCPacket(ITEM_CURRENCY_PROPERTY_ID_COOLDOWN, self->cooldown, stream);
}
