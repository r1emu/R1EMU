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

#include "item_book.h"

extern inline float *itemBookGetCooldown(ItemBook *self);

ItemBook *itemBookNew(void) {
    ItemBook *self;

    if ((self = malloc(sizeof(ItemBook))) == NULL) {
        return NULL;
    }

    if (!itemBookInit(self)) {
        itemBookDestroy(&self);
        error("ItemBook failed to initialize.");
        return NULL;
    }

    return self;
}

bool itemBookInit(ItemBook *self) {
    memset(self, 0, sizeof(*self));

    return true;
}

void itemBookFree(ItemBook *self) {
    // TODO
}

void itemBookDestroy(ItemBook **_self) {
    ItemBook *self = *_self;

    if (_self && self) {
        itemBookFree(self);
        free(self);
        *_self = NULL;
    }
}

size_t itemBookGetPropertiesCPacketSize(ItemBook *self) {
    size_t size = 0;

    size += propertyFloatGetCPacketSize(); // cooldown

    return size;
}

void itemBookGetPropertiesCPacket(ItemBook *self, PacketStream *stream) {
    propertyFloatGetCPacket(ITEM_BOOK_PROPERTY_ID_COOLDOWN, self->cooldown, stream);
}
