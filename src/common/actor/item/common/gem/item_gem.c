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

#include "item_gem.h"

extern inline float *itemGemGetLevel(ItemGem *self);
extern inline float *itemGemGetItemExp(ItemGem *self);
extern inline float *itemGemGetCooldown(ItemGem *self);

ItemGem *itemGemNew(Item *item) {
    ItemGem *self;

    if ((self = malloc(sizeof(ItemGem))) == NULL) {
        return NULL;
    }

    if (!itemGemInit(self, item)) {
        itemGemDestroy(&self);
        error("ItemGem failed to initialize.");
        return NULL;
    }

    return self;
}

bool itemGemInit(ItemGem *self, Item *item) {
    memset(self, 0, sizeof(*self));

    self->item = *item;

    return true;
}

void itemGemFree(ItemGem *self) {
    itemFree(&self->item);
    free(self->level);
    free(self->itemExp);
    free(self->cooldown);
}

void itemGemDestroy(ItemGem **_self) {
    ItemGem *self = *_self;

    if (_self && self) {
        itemGemFree(self);
        free(self);
        *_self = NULL;
    }
}

size_t itemGemGetPropertiesCPacketSize(ItemGem *self) {
    size_t size = 0;

    size += propertyFloatGetCPacketSize(self->level); // level
    size += propertyFloatGetCPacketSize(self->itemExp); // itemExp
    size += propertyFloatGetCPacketSize(self->cooldown); // cooldown

    return size;
}

void itemGemGetPropertiesCPacket(ItemGem *self, PacketStream *stream) {
    propertyFloatGetCPacket(ITEM_GEM_PROPERTY_ID_LEVEL, self->level, stream);
    propertyFloatGetCPacket(ITEM_GEM_PROPERTY_ID_ITEM_EXP, self->itemExp, stream);
    propertyFloatGetCPacket(ITEM_GEM_PROPERTY_ID_COOLDOWN, self->cooldown, stream);
}

void itemGemPrint(ItemGem *self) {
    PRINT_STRUCTURE {
        dbg("=== ItemGem %p ===", self);
        if (self->level) {
            dbg("level = %f", *self->level);
        }
        if (self->itemExp) {
            dbg("itemExp = %f", *self->itemExp);
        }
        if (self->cooldown) {
            dbg("cooldown = %f", *self->cooldown);
        }
        itemPrint(&self->item);
    }
}
