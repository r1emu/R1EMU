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
#include "common/actor/item/common/consumable/item_consumable.h"
#include "common/actor/item/common/quest/item_quest.h"
#include "common/actor/item/common/material/item_material.h"
#include "common/actor/item/common/book/item_book.h"
#include "common/actor/item/common/card/item_card.h"
#include "common/actor/item/common/currency/item_currency.h"
#include "common/actor/item/common/gem/item_gem.h"
#include "common/actor/item/equipable/armor/item_armor.h"
#include "common/actor/item/equipable/weapon/item_weapon.h"
#include "common/actor/item/equipable/subweapon/item_subweapon.h"
#include "common/actor/item/equipable/accessory/item_accessory.h"

// Inlined getters & setters
extern inline ItemId_t itemGetId(Item *self);
extern inline ItemAmount_t itemGetAmount(Item *self);
extern inline ItemCategory itemGetCategory(Item *self);

Item *itemNew(Actor *actor, ItemCategory category, ItemId_t id, ItemAmount_t amount) {
    Item *self;

    if ((self = malloc(sizeof(Item))) == NULL) {
        return NULL;
    }

    if (!itemInit(self, actor, category, id, amount)) {
        itemDestroy(&self);
        error("Item failed to initialize.");
        return NULL;
    }

    return self;
}

bool itemInit(Item *self, Actor *actor, ItemCategory category, ItemId_t id, ItemAmount_t amount) {
    memset(self, 0, sizeof(Item));

    memcpy(&self->actor, actor, sizeof(self->actor));
    self->category = category;
    self->id = id;
    self->amount = amount;

    return true;
}

void itemFree(Item *self) {
    actorFree(&self->actor);
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

        case ITEM_CAT_CONSUMABLE : size += itemConsumableGetPropertiesCPacketSize((ItemConsumable *) self); break;
        case ITEM_CAT_ARMOR      : size += itemArmorGetPropertiesCPacketSize((ItemArmor *) self); break;
        case ITEM_CAT_QUEST      : size += itemQuestGetPropertiesCPacketSize((ItemQuest *) self); break;
        case ITEM_CAT_BOOK       : size += itemBookGetPropertiesCPacketSize((ItemBook *) self); break;
        case ITEM_CAT_MATERIAL   : size += itemMaterialGetPropertiesCPacketSize((ItemMaterial *) self); break;
        case ITEM_CAT_GEM        : size += itemGemGetPropertiesCPacketSize((ItemGem *) self); break;
        case ITEM_CAT_WEAPON     : size += itemWeaponGetPropertiesCPacketSize((ItemWeapon *) self); break;
        case ITEM_CAT_CARD       : size += itemCardGetPropertiesCPacketSize((ItemCard *) self); break;
        case ITEM_CAT_ACCESSORY  : size += itemAccessoryGetPropertiesCPacketSize((ItemAccessory *) self); break;
        case ITEM_CAT_SUBWEAPON  : size += itemSubWeaponGetPropertiesCPacketSize((ItemSubWeapon *) self); break;
        case ITEM_CAT_CURRENCY   : size += itemCurrencyGetPropertiesCPacketSize((ItemCurrency *) self); break;

        case ITEM_CAT_COUNT      : error("Unexpected item category."); break;
    }

    return size;
}

void itemPropertiesGetCPacket(Item *self, PacketStream *stream) {

    switch (self->category) {

        case ITEM_CAT_CONSUMABLE : itemConsumableGetPropertiesCPacket((ItemConsumable *) self, stream); break;
        case ITEM_CAT_ARMOR      : itemArmorGetPropertiesCPacket((ItemArmor *) self, stream); break;
        case ITEM_CAT_QUEST      : itemQuestGetPropertiesCPacket((ItemQuest *) self, stream); break;
        case ITEM_CAT_BOOK       : itemBookGetPropertiesCPacket((ItemBook *) self, stream); break;
        case ITEM_CAT_MATERIAL   : itemMaterialGetPropertiesCPacket((ItemMaterial *) self, stream); break;
        case ITEM_CAT_GEM        : itemGemGetPropertiesCPacket((ItemGem *) self, stream); break;
        case ITEM_CAT_WEAPON     : itemWeaponGetPropertiesCPacket((ItemWeapon *) self, stream); break;
        case ITEM_CAT_CARD       : itemCardGetPropertiesCPacket((ItemCard *) self, stream); break;
        case ITEM_CAT_ACCESSORY  : itemAccessoryGetPropertiesCPacket((ItemAccessory *) self, stream); break;
        case ITEM_CAT_SUBWEAPON  : itemSubWeaponGetPropertiesCPacket((ItemSubWeapon *) self, stream); break;
        case ITEM_CAT_CURRENCY   : itemCurrencyGetPropertiesCPacket((ItemCurrency *) self, stream); break;

        case ITEM_CAT_COUNT      : error("Unexpected item category."); break;
    }
}

void itemPrint(Item *self) {
    PRINT_STRUCTURE {
        dbg("=== Item %p ===", self);
        dbg("ItemID = %d (%#x)", self->id, self->id);
        dbg("Amount = %d (%#x)", self->amount, self->amount);
        dbg("Category = %d (%#x)", self->category, self->category);
        actorPrint(&self->actor);
        itemCommonDataPrint(self->commonData);
    }
}

void itemChildPrint(Item *self) {

    switch (self->category) {

        case ITEM_CAT_CONSUMABLE : itemConsumablePrint((ItemConsumable *) self); break;
        case ITEM_CAT_ARMOR      : itemArmorPrint((ItemArmor *) self); break;
        case ITEM_CAT_QUEST      : itemQuestPrint((ItemQuest *) self); break;
        case ITEM_CAT_BOOK       : itemBookPrint((ItemBook *) self); break;
        case ITEM_CAT_MATERIAL   : itemMaterialPrint((ItemMaterial *) self); break;
        case ITEM_CAT_GEM        : itemGemPrint((ItemGem *) self); break;
        case ITEM_CAT_WEAPON     : itemWeaponPrint((ItemWeapon *) self); break;
        case ITEM_CAT_CARD       : itemCardPrint((ItemCard *) self); break;
        case ITEM_CAT_ACCESSORY  : itemAccessoryPrint((ItemAccessory *) self); break;
        case ITEM_CAT_SUBWEAPON  : itemSubWeaponPrint((ItemSubWeapon *) self); break;
        case ITEM_CAT_CURRENCY   : itemCurrencyPrint((ItemCurrency *) self); break;

        case ITEM_CAT_COUNT      : error("Unexpected item category."); break;
    }
}
