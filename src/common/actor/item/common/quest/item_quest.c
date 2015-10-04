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

size_t itemQuestGetPropertiesCPacketSize(ItemQuest *self) {
    size_t size = 0;

    size += propertyFloatGetCPacketSize(); // cooldown

    return size;
}

void itemQuestGetPropertiesCPacket(ItemQuest *self, PacketStream *stream) {
    propertyFloatGetCPacket(ITEM_QUEST_PROPERTY_ID_COOLDOWN, self->cooldown, stream);
}
