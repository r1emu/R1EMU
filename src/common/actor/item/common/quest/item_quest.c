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

#include "item_quest.h"

ItemQuestId itemQuestIds[] = {
    FOREACH_ITEM_QUEST(GENERATE_PROPERTY_ASSOC)
};

ItemQuest *itemQuestNew(Item *item) {
    ItemQuest *self;

    if ((self = malloc(sizeof(ItemQuest))) == NULL) {
        return NULL;
    }

    if (!itemQuestInit(self, item)) {
        itemQuestDestroy(&self);
        error("ItemQuest failed to initialize.");
        return NULL;
    }

    return self;
}

bool itemQuestInit(ItemQuest *self, Item *item) {
    memset(self, 0, sizeof(*self));

    self->item = *item;

    return true;
}

void itemQuestFree(ItemQuest *self) {
    itemFree(&self->item);
    free(self->cooldown);
}

void itemQuestDestroy(ItemQuest **_self) {
    ItemQuest *self = *_self;

    if (_self && self) {
        itemQuestFree(self);
        free(self);
        *_self = NULL;
    }
}

void itemQuestPrint(ItemQuest *self) {
    PRINT_STRUCTURE {
        dbg("=== ItemQuest %p ===", self);
        if (self->cooldown) {
            dbg("cooldown = %f", *self->cooldown);
        }
        itemPrint(&self->item);
    }
}

size_t itemQuestGetCPacketSize(ItemQuest *self) {
    size_t size = 0;
    size += propertyFloatGetCPacketSize(self->cooldown);
    return size;
}

size_t itemQuestGetSPacketSize(ItemQuest *self) {
    size_t size = 0;
    size += itemChildGetSPacketSize(&self->item);
    size += propertyFloatGetSPacketSize(self->cooldown);
    return size;
}

void itemQuestSerializeCPacket(ItemQuest *self, PacketStream *stream) {
    propertyFloatSerializeCPacket(ITEM_QUEST_PROPERTY_ID_COOLDOWN, self->cooldown, stream);
}

void itemQuestSerializeSPacket(ItemQuest *self, PacketStream *stream) {
    itemChildSerializeSPacket(&self->item, stream);
    propertyFloatSerializeSPacket(ITEM_QUEST_PROPERTY_ID_COOLDOWN, self->cooldown, stream);
}

bool itemQuestUnserializeSPacket(ItemQuest *self, PacketStream *stream) {

    if (!(itemChildUnserializeSPacket(&self->item, stream))) {
        error("Cannot unserialize item packet.");
        return false;
    }
    if (!(propertyFloatUnserializeSPacket(ITEM_QUEST_PROPERTY_ID_COOLDOWN, &self->cooldown, stream))) {
        error("Cannot unserialize packet COOLDOWN.");
        return false;
    }

    return true;
}
