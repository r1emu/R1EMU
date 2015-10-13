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

ItemBookId itemBookIds[] = {
    FOREACH_ITEM_BOOK(GENERATE_PROPERTY_ASSOC)
};

ItemBook *itemBookNew(Item *item) {
    ItemBook *self;

    if ((self = malloc(sizeof(ItemBook))) == NULL) {
        return NULL;
    }

    if (!itemBookInit(self, item)) {
        itemBookDestroy(&self);
        error("ItemBook failed to initialize.");
        return NULL;
    }

    return self;
}

bool itemBookInit(ItemBook *self, Item *item) {
    memset(self, 0, sizeof(*self));

    self->item = *item;

    return true;
}

void itemBookFree(ItemBook *self) {
    itemFree(&self->item);
    free(self->cooldown);
}

void itemBookDestroy(ItemBook **_self) {
    ItemBook *self = *_self;

    if (_self && self) {
        itemBookFree(self);
        free(self);
        *_self = NULL;
    }
}

void itemBookPrint(ItemBook *self) {
    PRINT_STRUCTURE {
        dbg("=== ItemBook %p ===", self);
        if (self->cooldown) {
            dbg("Cooldown = %f", *self->cooldown);
        }
        itemPrint(&self->item);
    }
}

size_t itemBookGetCPacketSize(ItemBook *self) {
    size_t size = 0;
    size += propertyFloatGetCPacketSize(self->cooldown);
    return size;
}

size_t itemBookGetSPacketSize(ItemBook *self) {
    size_t size = 0;
    size += itemChildGetSPacketSize(&self->item);
    size += propertyFloatGetSPacketSize(self->cooldown);
    return size;
}

void itemBookSerializeCPacket(ItemBook *self, PacketStream *stream) {
    propertyFloatSerializeCPacket(ITEM_BOOK_PROPERTY_ID_COOLDOWN, self->cooldown, stream);
}

void itemBookSerializeSPacket(ItemBook *self, PacketStream *stream) {
    itemChildSerializeSPacket(&self->item, stream);
    propertyFloatSerializeSPacket(ITEM_BOOK_PROPERTY_ID_COOLDOWN, self->cooldown, stream);
}

bool itemBookUnserializeSPacket(ItemBook *self, PacketStream *stream) {

    if (!(itemChildUnserializeSPacket(&self->item, stream))) {
        error("Cannot unserialize item packet.");
        return false;
    }
    if (!(propertyFloatUnserializeSPacket(ITEM_BOOK_PROPERTY_ID_COOLDOWN, &self->cooldown, stream))) {
        error("Cannot unserialize packet COOLDOWN.");
        return false;
    }

    return true;
}
