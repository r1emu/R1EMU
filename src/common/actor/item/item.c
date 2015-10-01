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

#include "item.h"

// Inlined functions
extern inline ActorId_t itemGetUId(Item *self);
extern inline ItemId_t itemGetId(Item *self);
extern inline ItemInventoryIndex_t itemGetInventoryIndex(Item *self);
extern inline ItemAmount_t itemGetAmount(Item *self);
extern inline ItemCategory itemGetCategory(Item *self);


Item *itemNew(void) {
    Item *self;

    if ((self = malloc(sizeof(Item))) == NULL) {
        return NULL;
    }

    if (!itemInit(self)) {
        itemDestroy(&self);
        error("Item failed to initialize.");
        return NULL;
    }

    return self;
}

bool itemInit(Item *self) {
    memset(self, 0, sizeof(Item));

    return true;
}

void itemFree(Item *self) {
    // TODO
}

void itemDestroy(Item **_self) {
    Item *self = *_self;

    if (_self && self) {
        itemFree(self);
        free(self);
        *_self = NULL;
    }
}


size_t itemGetAttributesCPacketSize(Item *self) {

    size_t size = 0;

    return size;
}

void itemAttributesGetCPacket(Item *self, PacketStream *stream) {

}
