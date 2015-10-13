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

#include "item_equipable.h"

// Inlined functions
extern inline int itemEquipableGetSlot(ItemEquipable *self);

ItemEquipable *itemEquipableNew(Item *item) {
    ItemEquipable *self;

    if ((self = malloc(sizeof(ItemEquipable))) == NULL) {
        return NULL;
    }

    if (!itemEquipableInit(self, item)) {
        itemEquipableDestroy(&self);
        error("ItemEquipable failed to initialize.");
        return NULL;
    }

    return self;
}

bool itemEquipableInit(ItemEquipable *self, Item *item) {
    memset(self, 0, sizeof(*self));

    self->item = *item;
    self->slot = EQSLOT_NOSLOT;

    return true;
}

void itemEquipableFree(ItemEquipable *self) {
    itemFree(&self->item);
}

void itemEquipableDestroy(ItemEquipable **_self) {
    ItemEquipable *self = *_self;

    if (_self && self) {
        itemEquipableFree(self);
        free(self);
        *_self = NULL;
    }
}

void itemEquipablePrint(ItemEquipable *self) {
    PRINT_STRUCTURE {
        dbg("=== ItemEquipable %p ===", self);
        dbg("slot = %d", self->slot);
        itemPrint(&self->item);
    }
}

size_t itemEquipableGetSPacketSize(ItemEquipable *self) {
    size_t size = 0;
    size += itemChildGetSPacketSize(&self->item);
    size += sizeof(self->slot);
    return size;
}

void itemEquipableSerializeSPacket(ItemEquipable *self, PacketStream *stream) {
    itemChildSerializeSPacket((Item *) self, stream);
    packetStreamIn(stream, &self->slot);
}

bool itemEquipableUnserializeSPacket(ItemEquipable *self, PacketStream *stream) {
    if (!(itemChildUnserializeSPacket((Item *) self, stream))) {
        error("Cannot unserialize item packet.");
        return false;
    }
    packetStreamOut(stream, &self->slot);

    return true;
}
