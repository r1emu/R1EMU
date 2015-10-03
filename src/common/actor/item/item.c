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
#include "common/consumable/item_consumable.h"
#include "common/quest/item_quest.h"
#include "equipable/armor/item_armor.h"

// Inlined getters & setters
extern inline ItemId_t itemGetId(Item *self);
extern inline ItemAmount_t itemGetAmount(Item *self);
extern inline ItemCategory itemGetCategory(Item *self);
extern inline ItemInventoryIndex_t itemGetInventoryIndex(Item *self);

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

size_t itemGetPropertiesCPacketSize(Item *self) {
    size_t size = 0;

    switch (self->category) {
        case ITEM_CAT_CONSUMABLE:
            size += itemConsumableGetPropertiesCPacketSize((ItemConsumable *) self);
        break;

        case ITEM_CAT_ARMOR:
            size += itemArmorGetPropertiesCPacketSize((ItemArmor *) self);
        break;

        case ITEM_CAT_QUEST:
            size += itemQuestGetPropertiesCPacketSize((ItemQuest *) self);
        break;

        case ITEM_CAT_BOOK:
        break;

        case ITEM_CAT_MATERIAL:
        break;

        case ITEM_CAT_GEM:
        break;

        case ITEM_CAT_WEAPON:
        break;

        case ITEM_CAT_CARD:
        break;

        case ITEM_CAT_ACCESSORY:
        break;

        case ITEM_CAT_SUBWEAPON:
        break;

        case ITEM_CAT_CURRENCY:
        break;

        case ITEM_CAT_COUNT: error("Unexpected item category."); break;
    }

    return size;
}

void itemPropertiesGetCPacket(Item *self, PacketStream *stream) {

    switch (self->category) {
        case ITEM_CAT_CONSUMABLE:
            itemConsumableGetPropertiesCPacket((ItemConsumable *) self, stream);
        break;

        case ITEM_CAT_ARMOR:
            itemArmorGetPropertiesCPacket((ItemArmor *) self, stream);
        break;

        case ITEM_CAT_QUEST:
            itemQuestGetPropertiesCPacket((ItemQuest *) self, stream);
        break;

        case ITEM_CAT_BOOK:
        break;

        case ITEM_CAT_MATERIAL:
        break;

        case ITEM_CAT_GEM:
        break;

        case ITEM_CAT_WEAPON:
        break;

        case ITEM_CAT_CARD:
        break;

        case ITEM_CAT_ACCESSORY:
        break;

        case ITEM_CAT_SUBWEAPON:
        break;

        case ITEM_CAT_CURRENCY:
        break;

        case ITEM_CAT_COUNT: error("Unexpected item category."); break;
    }

}
