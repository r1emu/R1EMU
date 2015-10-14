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
extern inline void itemEquipableSetEquipData(ItemEquipable *self, ItemEquipData *data);
extern inline int itemEquipableGetSlot(ItemEquipable *self);

ItemEquipable *itemEquipableNew(Item *item, ItemEquipData *data) {
    ItemEquipable *self;

    if ((self = malloc(sizeof(ItemEquipable))) == NULL) {
        return NULL;
    }

    if (!itemEquipableInit(self, item, data)) {
        itemEquipableDestroy(&self);
        error("ItemEquipable failed to initialize.");
        return NULL;
    }

    return self;
}

bool itemEquipableInit(ItemEquipable *self, Item *item, ItemEquipData *data) {
    memset(self, 0, sizeof(*self));

    self->item = *item;
    self->slot = EQSLOT_NOSLOT;
    self->equipData = data;

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
    return sizeof(ItemEquipableSPacket);
}

void itemEquipableSerializeSPacket(ItemEquipable *self, PacketStream *stream) {
    packetStreamDebugStart(stream, itemEquipableGetSPacketSize(self));
    packetStreamIn(stream, &self->slot);
    packetStreamDebugEnd(stream);
}

bool itemEquipableUnserializeSPacket(ItemEquipable *self, PacketStream *stream) {
    packetStreamDebugStart(stream, itemEquipableGetSPacketSize(self));
    packetStreamOut(stream, &self->slot);
    packetStreamDebugEnd(stream);
    return true;
}
