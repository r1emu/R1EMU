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
#include "common/actor/item/item_factory.h"
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
extern inline void itemSetCommonData(Item *self, ItemCommonData *data);

Item *itemNew(Actor *actor, ItemCategory category, ItemId_t id, ItemAmount_t amount, ItemCommonData *data) {
    Item *self;

    if ((self = malloc(sizeof(Item))) == NULL) {
        error("Cannot allocate a new Item.");
        return NULL;
    }

    if (!itemInit(self, actor, category, id, amount, data)) {
        itemDestroy(&self);
        error("Item failed to initialize.");
        return NULL;
    }

    return self;
}

bool itemInit(Item *self, Actor *actor, ItemCategory category, ItemId_t id, ItemAmount_t amount, ItemCommonData *data) {
    memset(self, 0, sizeof(Item));

    memcpy(&self->actor, actor, sizeof(self->actor));
    self->category = category;
    self->id = id;
    self->amount = amount;
    self->commonData = data;

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

ItemHandle *itemHandleNew(Item *item) {
     ItemHandle *self = NULL;

     if (!(self = malloc(sizeof(ItemHandle)))) {
        error("Cannot allocate a new item handle.");
        return NULL;
     }

     *self = item;
     return self;
}

void itemHandleFree(ItemHandle *self) {
    Item *item = *self;
    itemDestroy(&item);
}

void itemHandleDestroy(ItemHandle **_self) {
    ItemHandle *self = *_self;

    if (_self && self) {
        itemHandleFree(self);
        free(self);
        *_self = NULL;
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

size_t itemGetCPacketSize(Item *self) {
    size_t size = 0;

    switch (self->category) {

        case ITEM_CAT_CONSUMABLE : size += itemConsumableGetCPacketSize((ItemConsumable *) self); break;
        case ITEM_CAT_ARMOR      : size += itemArmorGetCPacketSize((ItemArmor *) self); break;
        case ITEM_CAT_QUEST      : size += itemQuestGetCPacketSize((ItemQuest *) self); break;
        case ITEM_CAT_BOOK       : size += itemBookGetCPacketSize((ItemBook *) self); break;
        case ITEM_CAT_MATERIAL   : size += itemMaterialGetCPacketSize((ItemMaterial *) self); break;
        case ITEM_CAT_GEM        : size += itemGemGetCPacketSize((ItemGem *) self); break;
        case ITEM_CAT_WEAPON     : size += itemWeaponGetCPacketSize((ItemWeapon *) self); break;
        case ITEM_CAT_CARD       : size += itemCardGetCPacketSize((ItemCard *) self); break;
        case ITEM_CAT_ACCESSORY  : size += itemAccessoryGetCPacketSize((ItemAccessory *) self); break;
        case ITEM_CAT_SUBWEAPON  : size += itemSubWeaponGetCPacketSize((ItemSubWeapon *) self); break;
        case ITEM_CAT_CURRENCY   : size += itemCurrencyGetCPacketSize((ItemCurrency *) self); break;

        case ITEM_CAT_COUNT      : error("Unexpected item category."); break;
    }

    return size;
}

void itemSerializeCPacket(Item *self, PacketStream *stream) {

    switch (self->category) {

        case ITEM_CAT_CONSUMABLE : itemConsumableSerializeCPacket((ItemConsumable *) self, stream); break;
        case ITEM_CAT_ARMOR      : itemArmorSerializeCPacket((ItemArmor *) self, stream); break;
        case ITEM_CAT_QUEST      : itemQuestSerializeCPacket((ItemQuest *) self, stream); break;
        case ITEM_CAT_BOOK       : itemBookSerializeCPacket((ItemBook *) self, stream); break;
        case ITEM_CAT_MATERIAL   : itemMaterialSerializeCPacket((ItemMaterial *) self, stream); break;
        case ITEM_CAT_GEM        : itemGemSerializeCPacket((ItemGem *) self, stream); break;
        case ITEM_CAT_WEAPON     : itemWeaponSerializeCPacket((ItemWeapon *) self, stream); break;
        case ITEM_CAT_CARD       : itemCardSerializeCPacket((ItemCard *) self, stream); break;
        case ITEM_CAT_ACCESSORY  : itemAccessorySerializeCPacket((ItemAccessory *) self, stream); break;
        case ITEM_CAT_SUBWEAPON  : itemSubWeaponSerializeCPacket((ItemSubWeapon *) self, stream); break;
        case ITEM_CAT_CURRENCY   : itemCurrencySerializeCPacket((ItemCurrency *) self, stream); break;

        case ITEM_CAT_COUNT      : error("Unexpected item category."); break;
    }
}

size_t itemGetSPacketSize(Item *self) {
    size_t size = 0;

    switch (self->category) {

        case ITEM_CAT_CONSUMABLE :
            size += actorGetSPacketSize(&self->actor);
            size += itemChildGetSPacketSize((Item *) self);
            size += itemConsumableGetSPacketSize((ItemConsumable *) self);
        break;

        case ITEM_CAT_ARMOR :
            size += actorGetSPacketSize(&self->actor);
            size += itemChildGetSPacketSize((Item *) self);
            size += itemEquipableGetSPacketSize((ItemEquipable *) self);
            size += itemArmorGetSPacketSize((ItemArmor *) self);
        break;

        case ITEM_CAT_QUEST :
            size += actorGetSPacketSize(&self->actor);
            size += itemChildGetSPacketSize((Item *) self);
            size += itemQuestGetSPacketSize((ItemQuest *) self);
        break;

        case ITEM_CAT_BOOK :
            size += actorGetSPacketSize(&self->actor);
            size += itemChildGetSPacketSize((Item *) self);
            size += itemBookGetSPacketSize((ItemBook *) self);
        break;

        case ITEM_CAT_MATERIAL :
            size += actorGetSPacketSize(&self->actor);
            size += itemChildGetSPacketSize((Item *) self);
            size += itemMaterialGetSPacketSize((ItemMaterial *) self);
        break;

        case ITEM_CAT_GEM :
            size += actorGetSPacketSize(&self->actor);
            size += itemChildGetSPacketSize((Item *) self);
            size += itemGemGetSPacketSize((ItemGem *) self);
        break;

        case ITEM_CAT_WEAPON :
            size += actorGetSPacketSize(&self->actor);
            size += itemChildGetSPacketSize((Item *) self);
            size += itemEquipableGetSPacketSize((ItemEquipable *) self);
            size += itemWeaponGetSPacketSize((ItemWeapon *) self);
        break;

        case ITEM_CAT_CARD :
            size += actorGetSPacketSize(&self->actor);
            size += itemChildGetSPacketSize((Item *) self);
            size += itemCardGetSPacketSize((ItemCard *) self);
        break;

        case ITEM_CAT_ACCESSORY :
            size += actorGetSPacketSize(&self->actor);
            size += itemChildGetSPacketSize((Item *) self);
            size += itemEquipableGetSPacketSize((ItemEquipable *) self);
            size += itemAccessoryGetSPacketSize((ItemAccessory *) self);
        break;

        case ITEM_CAT_SUBWEAPON :
            size += actorGetSPacketSize(&self->actor);
            size += itemChildGetSPacketSize((Item *) self);
            size += itemEquipableGetSPacketSize((ItemEquipable *) self);
            size += itemSubWeaponGetSPacketSize((ItemSubWeapon *) self);
        break;

        case ITEM_CAT_CURRENCY :
            size += actorGetSPacketSize(&self->actor);
            size += itemChildGetSPacketSize((Item *) self);
            size += itemCurrencyGetSPacketSize((ItemCurrency *) self);
        break;

        case ITEM_CAT_COUNT      : error("Unexpected item category."); break;
    }

    return size;
}

size_t itemChildGetSPacketSize(Item *self) {
    size_t size = 0;
    size += sizeof(ItemSPacket);
    return size;
}

void itemChildSerializeSPacket(Item *self, ItemInventoryIndex_t index, PacketStream *stream) {
    packetStreamIn(stream, &self->id);
    packetStreamIn(stream, &self->amount);
    packetStreamIn(stream, &self->category);
    packetStreamIn(stream, &index);
}

bool itemChildUnserializeSPacket(Item *self, ItemInventoryIndex_t *index, PacketStream *stream) {
    packetStreamOut(stream, &self->id);
    packetStreamOut(stream, &self->amount);
    packetStreamOut(stream, &self->category);
    packetStreamOut(stream, index);
    return true;
}

void itemSerializeSPacket(Item *self, ItemInventoryIndex_t index, PacketStream *stream) {

    switch (self->category) {

        case ITEM_CAT_CONSUMABLE :
            actorSerializeSPacket(&self->actor, stream);
            itemChildSerializeSPacket((Item *) self, index, stream);
            itemConsumableSerializeSPacket((ItemConsumable *) self, stream);
        break;

        case ITEM_CAT_ARMOR :
            actorSerializeSPacket(&self->actor, stream);
            itemChildSerializeSPacket((Item *) self, index, stream);
            itemEquipableSerializeSPacket((ItemEquipable *) self, stream);
            itemArmorSerializeSPacket((ItemArmor *) self, stream);
        break;

        case ITEM_CAT_QUEST :
            actorSerializeSPacket(&self->actor, stream);
            itemChildSerializeSPacket((Item *) self, index, stream);
            itemQuestSerializeSPacket((ItemQuest *) self, stream);
        break;

        case ITEM_CAT_BOOK :
            actorSerializeSPacket(&self->actor, stream);
            itemChildSerializeSPacket((Item *) self, index, stream);
            itemBookSerializeSPacket((ItemBook *) self, stream);
        break;

        case ITEM_CAT_MATERIAL :
            actorSerializeSPacket(&self->actor, stream);
            itemChildSerializeSPacket((Item *) self, index, stream);
            itemMaterialSerializeSPacket((ItemMaterial *) self, stream);
        break;

        case ITEM_CAT_GEM :
            actorSerializeSPacket(&self->actor, stream);
            itemChildSerializeSPacket((Item *) self, index, stream);
            itemGemSerializeSPacket((ItemGem *) self, stream);
        break;

        case ITEM_CAT_WEAPON :
            actorSerializeSPacket(&self->actor, stream);
            itemChildSerializeSPacket((Item *) self, index, stream);
            itemEquipableSerializeSPacket((ItemEquipable *) self, stream);
            itemWeaponSerializeSPacket((ItemWeapon *) self, stream);
        break;

        case ITEM_CAT_CARD :
            actorSerializeSPacket(&self->actor, stream);
            itemChildSerializeSPacket((Item *) self, index, stream);
            itemCardSerializeSPacket((ItemCard *) self, stream);
        break;

        case ITEM_CAT_ACCESSORY :
            actorSerializeSPacket(&self->actor, stream);
            itemChildSerializeSPacket((Item *) self, index, stream);
            itemEquipableSerializeSPacket((ItemEquipable *) self, stream);
            itemAccessorySerializeSPacket((ItemAccessory *) self, stream);
        break;

        case ITEM_CAT_SUBWEAPON :
            actorSerializeSPacket(&self->actor, stream);
            itemChildSerializeSPacket((Item *) self, index, stream);
            itemEquipableSerializeSPacket((ItemEquipable *) self, stream);
            itemSubWeaponSerializeSPacket((ItemSubWeapon *) self, stream);
        break;

        case ITEM_CAT_CURRENCY :
            actorSerializeSPacket(&self->actor, stream);
            itemChildSerializeSPacket((Item *) self, index, stream);
            itemCurrencySerializeSPacket((ItemCurrency *) self, stream);
        break;

        case ITEM_CAT_COUNT      : error("Unexpected item category."); break;
    }
}

bool itemUnserializeSPacket(Item *self, ItemInventoryIndex_t *index, PacketStream *stream) {

    bool status = false;

    if (!(actorUnserializeSPacket(&self->actor, stream))) {
        error("Cannot unserialize actor.");
        return false;
    }

    if (!(itemChildUnserializeSPacket((Item *) self, index, stream))) {
        error("Cannot unserialize item child.");
        return false;
    }

    ItemCategory category = self->category;

    // Get static data
    ItemCommonData *commonData = NULL;
    ItemEquipData *equipData = NULL;
    itemFactoryGetStaticData(self->id, &commonData, &equipData);

    itemSetCommonData(self, commonData);

    switch (category) {

        case ITEM_CAT_CONSUMABLE :
            status = itemConsumableUnserializeSPacket((ItemConsumable *) self, stream);
        break;

        case ITEM_CAT_ARMOR :
            status = (
                    itemEquipableUnserializeSPacket((ItemEquipable *) self, stream)
                &&  itemArmorUnserializeSPacket((ItemArmor *) self, stream)
            );
            itemEquipableSetEquipData((ItemEquipable *) self, equipData);
        break;

        case ITEM_CAT_QUEST :
            status = itemQuestUnserializeSPacket((ItemQuest *) self, stream);
        break;

        case ITEM_CAT_BOOK :
            status = itemBookUnserializeSPacket((ItemBook *) self, stream);
        break;

        case ITEM_CAT_MATERIAL :
            status = itemMaterialUnserializeSPacket((ItemMaterial *) self, stream);
        break;

        case ITEM_CAT_GEM :
            status = itemGemUnserializeSPacket((ItemGem *) self, stream);
        break;

        case ITEM_CAT_WEAPON :
            status = (
                    itemEquipableUnserializeSPacket((ItemEquipable *) self, stream)
                &&  itemWeaponUnserializeSPacket((ItemWeapon *) self, stream)
            );
            itemEquipableSetEquipData((ItemEquipable *) self, equipData);
        break;

        case ITEM_CAT_CARD :
            status = itemCardUnserializeSPacket((ItemCard *) self, stream);
        break;

        case ITEM_CAT_ACCESSORY :
            status = (
                    itemEquipableUnserializeSPacket((ItemEquipable *) self, stream)
                &&  itemAccessoryUnserializeSPacket((ItemAccessory *) self, stream)
            );
            itemEquipableSetEquipData((ItemEquipable *) self, equipData);
        break;

        case ITEM_CAT_SUBWEAPON :
            status = (
                    itemEquipableUnserializeSPacket((ItemEquipable *) self, stream)
                &&  itemSubWeaponUnserializeSPacket((ItemSubWeapon *) self, stream)
            );
            itemEquipableSetEquipData((ItemEquipable *) self, equipData);
        break;

        case ITEM_CAT_CURRENCY :
            status = itemCurrencyUnserializeSPacket((ItemCurrency *) self, stream);
        break;

        case ITEM_CAT_COUNT      : error("Unexpected item category '%d'.", category); break;
    }

    if (!status) {
        error("Cannot unserialize the sub packet item '%x' ('%llx')", self->id, self->actor.uid);
        return false;
    }

    return true;
}
