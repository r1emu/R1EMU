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

ItemGemId itemGemIds[] = {
    FOREACH_ITEM_GEM(GENERATE_PROPERTY_ASSOC)
};

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

size_t itemGemGetCPacketSize(ItemGem *self) {
    size_t size = 0;
    size += propertyFloatGetCPacketSize(self->level);
    size += propertyFloatGetCPacketSize(self->itemExp);
    size += propertyFloatGetCPacketSize(self->cooldown);
    return size;
}

size_t itemGemGetSPacketSize(ItemGem *self) {
    size_t size = 0;
    size += itemChildGetSPacketSize(&self->item);
    size += propertyFloatGetSPacketSize(self->level);
    size += propertyFloatGetSPacketSize(self->itemExp);
    size += propertyFloatGetSPacketSize(self->cooldown);
    return size;
}

void itemGemSerializeCPacket(ItemGem *self, PacketStream *stream) {
    propertyFloatSerializeCPacket(ITEM_GEM_PROPERTY_ID_LEVEL, self->level, stream);
    propertyFloatSerializeCPacket(ITEM_GEM_PROPERTY_ID_ITEM_EXP, self->itemExp, stream);
    propertyFloatSerializeCPacket(ITEM_GEM_PROPERTY_ID_COOLDOWN, self->cooldown, stream);
}

void itemGemSerializeSPacket(ItemGem *self, PacketStream *stream) {
    itemChildSerializeSPacket(&self->item, stream);
    propertyFloatSerializeSPacket(ITEM_GEM_PROPERTY_ID_LEVEL, self->level, stream);
    propertyFloatSerializeSPacket(ITEM_GEM_PROPERTY_ID_ITEM_EXP, self->itemExp, stream);
    propertyFloatSerializeSPacket(ITEM_GEM_PROPERTY_ID_COOLDOWN, self->cooldown, stream);
}

bool itemGemUnserializeSPacket(ItemGem *self, PacketStream *stream) {

    if (!(itemChildUnserializeSPacket(&self->item, stream))) {
        error("Cannot unserialize item packet.");
        return false;
    }
    if (!(propertyFloatUnserializeSPacket(ITEM_GEM_PROPERTY_ID_LEVEL, &self->level, stream))) {
        error("Cannot unserialize packet LEVEL.");
        return false;
    }
    if (!(propertyFloatUnserializeSPacket(ITEM_GEM_PROPERTY_ID_ITEM_EXP, &self->itemExp, stream))) {
        error("Cannot unserialize packet ITEM_EXP.");
        return false;
    }
    if (!(propertyFloatUnserializeSPacket(ITEM_GEM_PROPERTY_ID_COOLDOWN, &self->cooldown, stream))) {
        error("Cannot unserialize packet COOLDOWN.");
        return false;
    }

    return true;
}
