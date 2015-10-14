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
extern inline ItemCategory_t itemGetCategory(Item *self);
extern inline void itemSetCommonData(Item *self, ItemCommonData *data);

Item *itemNew(
    Actor *actor,
    ItemCategory_t category,
    ItemId_t id,
    ItemAmount_t amount,
    ItemCommonData *commonData,
    ItemEquipData *equipData)
{
    Item *self;

    size_t itemSize;
    if (!(itemGetSize(category, &itemSize))) {
        error("Cannot determine the item size.");
        return NULL;
    }

    if ((self = malloc(itemSize)) == NULL) {
        error("Cannot allocate a new Item.");
        return NULL;
    }

    if (!itemInit(self, actor, category, id, amount, commonData, equipData)) {
        itemDestroy(&self);
        error("Item failed to initialize.");
        return NULL;
    }

    return self;
}

bool itemInit(
    Item *self,
    Actor *actor,
    ItemCategory_t category,
    ItemId_t id,
    ItemAmount_t amount,
    ItemCommonData *commonData,
    ItemEquipData *equipData)
{
    bool status = false;
    memset(self, 0, sizeof(Item));

    memcpy(&self->actor, actor, sizeof(self->actor));
    self->category = category;
    self->id = id;
    self->amount = amount;
    self->commonData = commonData;

    // Copy the item data
    Item itemTmp = *self;

    // Initialize the child class
    switch (category) {

        case ITEM_CAT_CONSUMABLE : {
            ItemConsumable *consumable = (ItemConsumable *) self;
            if (!(itemConsumableInit(consumable, &itemTmp))) {
                error("Cannot initialize a consumable item.");
                goto cleanup;
            }
            break;
        }
        case ITEM_CAT_ARMOR      : {
            ItemArmor *armor = (ItemArmor *) self;
            if (!(itemArmorInit(armor, &itemTmp, equipData))) {
                error("Cannot initialize a armor item.");
                goto cleanup;
            }
            break;
        }
        case ITEM_CAT_QUEST      : {
            ItemQuest *quest = (ItemQuest *) self;
            if (!(itemQuestInit(quest, &itemTmp))) {
                error("Cannot initialize a quest item.");
                goto cleanup;
            }
            break;
        }
        case ITEM_CAT_BOOK       : {
            ItemBook *book = (ItemBook *) self;
            if (!(itemBookInit(book, &itemTmp))) {
                error("Cannot initialize a book item.");
                goto cleanup;
            }
            break;
        }
        case ITEM_CAT_MATERIAL   : {
            ItemMaterial *material = (ItemMaterial *) self;
            if (!(itemMaterialInit(material, &itemTmp))) {
                error("Cannot initialize a material item.");
                goto cleanup;
            }
            break;
        }
        case ITEM_CAT_GEM        : {
            ItemGem *gem = (ItemGem *) self;
            if (!(itemGemInit(gem, &itemTmp))) {
                error("Cannot initialize a gem item.");
                goto cleanup;
            }
            break;
        }
        case ITEM_CAT_WEAPON     : {
            ItemWeapon *weapon = (ItemWeapon *) self;
            if (!(itemWeaponInit(weapon, &itemTmp, equipData))) {
                error("Cannot initialize a weapon item.");
                goto cleanup;
            }
            break;
        }
        case ITEM_CAT_CARD       : {
            ItemCard *card = (ItemCard *) self;
            if (!(itemCardInit(card, &itemTmp))) {
                error("Cannot initialize a card item.");
                goto cleanup;
            }
            break;
        }
        case ITEM_CAT_ACCESSORY  : {
            ItemAccessory *accessory = (ItemAccessory *) self;
            if (!(itemAccessoryInit(accessory, &itemTmp, equipData))) {
                error("Cannot initialize a accessory item.");
                goto cleanup;
            }
            break;
        }
        case ITEM_CAT_SUBWEAPON  : {
            ItemSubWeapon *subweapon = (ItemSubWeapon *) self;
            if (!(itemSubWeaponInit(subweapon, &itemTmp, equipData))) {
                error("Cannot initialize a subweapon item.");
                goto cleanup;
            }
            break;
        }
        case ITEM_CAT_CURRENCY   : {
            ItemCurrency *currency = (ItemCurrency *) self;
            if (!(itemCurrencyInit(currency, &itemTmp))) {
                error("Cannot initialize a currency item.");
                goto cleanup;
            }
            break;
        }

        case ITEM_CAT_COUNT      : {
            error("Invalid item category");
            goto cleanup;
            break;
        }
    }

    status = true;

cleanup:
    return status;
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

bool itemGetSize(ItemCategory_t category, size_t *size) {

    size_t itemSize;

    switch (category) {

        case ITEM_CAT_CONSUMABLE : itemSize = sizeof(ItemConsumable); break;
        case ITEM_CAT_ARMOR      : itemSize = sizeof(ItemArmor); break;
        case ITEM_CAT_QUEST      : itemSize = sizeof(ItemQuest); break;
        case ITEM_CAT_BOOK       : itemSize = sizeof(ItemBook); break;
        case ITEM_CAT_MATERIAL   : itemSize = sizeof(ItemMaterial); break;
        case ITEM_CAT_GEM        : itemSize = sizeof(ItemGem); break;
        case ITEM_CAT_WEAPON     : itemSize = sizeof(ItemWeapon); break;
        case ITEM_CAT_CARD       : itemSize = sizeof(ItemCard); break;
        case ITEM_CAT_ACCESSORY  : itemSize = sizeof(ItemAccessory); break;
        case ITEM_CAT_SUBWEAPON  : itemSize = sizeof(ItemSubWeapon); break;
        case ITEM_CAT_CURRENCY   : itemSize = sizeof(ItemCurrency); break;

        case ITEM_CAT_COUNT      : error("Unexpected item category."); return false; break;
    }

    *size = itemSize;

    return true;
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

    size += actorGetSPacketSize(&self->actor);
    size += itemChildGetSPacketSize((Item *) self);

    switch (self->category) {

        case ITEM_CAT_CONSUMABLE :
            size += itemConsumableGetSPacketSize((ItemConsumable *) self);
        break;

        case ITEM_CAT_ARMOR :
            size += itemEquipableGetSPacketSize((ItemEquipable *) self);
            size += itemArmorGetSPacketSize((ItemArmor *) self);
        break;

        case ITEM_CAT_QUEST :
            size += itemQuestGetSPacketSize((ItemQuest *) self);
        break;

        case ITEM_CAT_BOOK :
            size += itemBookGetSPacketSize((ItemBook *) self);
        break;

        case ITEM_CAT_MATERIAL :
            size += itemMaterialGetSPacketSize((ItemMaterial *) self);
        break;

        case ITEM_CAT_GEM :
            size += itemGemGetSPacketSize((ItemGem *) self);
        break;

        case ITEM_CAT_WEAPON :
            size += itemEquipableGetSPacketSize((ItemEquipable *) self);
            size += itemWeaponGetSPacketSize((ItemWeapon *) self);
        break;

        case ITEM_CAT_CARD :
            size += itemCardGetSPacketSize((ItemCard *) self);
        break;

        case ITEM_CAT_ACCESSORY :
            size += itemEquipableGetSPacketSize((ItemEquipable *) self);
            size += itemAccessoryGetSPacketSize((ItemAccessory *) self);
        break;

        case ITEM_CAT_SUBWEAPON :
            size += itemEquipableGetSPacketSize((ItemEquipable *) self);
            size += itemSubWeaponGetSPacketSize((ItemSubWeapon *) self);
        break;

        case ITEM_CAT_CURRENCY :
            size += itemCurrencyGetSPacketSize((ItemCurrency *) self);
        break;

        case ITEM_CAT_COUNT :
            error("Unexpected item category.");
        break;
    }

    return size;
}

size_t itemChildGetSPacketSize(Item *self) {
    size_t size = 0;
    size += sizeof(ItemSPacket);
    return size;
}

void itemChildSerializeSPacket(Item *self, PacketStream *stream) {
    packetStreamDebugStart(stream, itemChildGetSPacketSize(self));
    packetStreamIn(stream, &self->id);
    packetStreamIn(stream, &self->amount);
    packetStreamIn(stream, &self->category);
    packetStreamDebugEnd(stream);
}

bool itemChildUnserializeSPacket(Item *self, PacketStream *stream) {
    packetStreamDebugStart(stream, itemChildGetSPacketSize(self));
    packetStreamOut(stream, &self->id);
    packetStreamOut(stream, &self->amount);
    packetStreamOut(stream, &self->category);
    packetStreamDebugEnd(stream);
    return true;
}

void itemSerializeSPacket(Item *self, PacketStream *stream) {

    actorSerializeSPacket(&self->actor, stream);
    itemChildSerializeSPacket((Item *) self, stream);

    switch (self->category) {

        case ITEM_CAT_CONSUMABLE :
            itemConsumableSerializeSPacket((ItemConsumable *) self, stream);
        break;

        case ITEM_CAT_ARMOR :
            itemEquipableSerializeSPacket((ItemEquipable *) self, stream);
            itemArmorSerializeSPacket((ItemArmor *) self, stream);
        break;

        case ITEM_CAT_QUEST :
            itemQuestSerializeSPacket((ItemQuest *) self, stream);
        break;

        case ITEM_CAT_BOOK :
            itemBookSerializeSPacket((ItemBook *) self, stream);
        break;

        case ITEM_CAT_MATERIAL :
            itemMaterialSerializeSPacket((ItemMaterial *) self, stream);
        break;

        case ITEM_CAT_GEM :
            itemGemSerializeSPacket((ItemGem *) self, stream);
        break;

        case ITEM_CAT_WEAPON :
            itemEquipableSerializeSPacket((ItemEquipable *) self, stream);
            itemWeaponSerializeSPacket((ItemWeapon *) self, stream);
        break;

        case ITEM_CAT_CARD :
            itemCardSerializeSPacket((ItemCard *) self, stream);
        break;

        case ITEM_CAT_ACCESSORY :
            itemEquipableSerializeSPacket((ItemEquipable *) self, stream);
            itemAccessorySerializeSPacket((ItemAccessory *) self, stream);
        break;

        case ITEM_CAT_SUBWEAPON :
            itemEquipableSerializeSPacket((ItemEquipable *) self, stream);
            itemSubWeaponSerializeSPacket((ItemSubWeapon *) self, stream);
        break;

        case ITEM_CAT_CURRENCY :
            itemCurrencySerializeSPacket((ItemCurrency *) self, stream);
        break;

        case ITEM_CAT_COUNT :
            error("Unexpected item category.");
        break;
    }
}

bool itemUnserializeSPacket(Item **_self, PacketStream *stream) {

    bool status = false;
    Item itemTmp;
    Actor actorTmp;
    Item *self = NULL;

    if (!(actorUnserializeSPacket(&actorTmp, stream))) {
        error("Cannot unserialize actor.");
        return false;
    }

    if (!(itemChildUnserializeSPacket(&itemTmp, stream))) {
        error("Cannot unserialize item child.");
        return false;
    }

    ItemCategory_t category = itemTmp.category;
    ItemId_t id             = itemTmp.id;
    ItemAmount_t amount     = itemTmp.amount;

    // Get static data
    ItemCommonData *commonData = NULL;
    ItemEquipData *equipData = NULL;
    itemFactoryGetStaticData(itemTmp.id, &commonData, &equipData);

    if (!(self = itemNew(&actorTmp, category, id, amount, commonData, equipData))) {
        error("Cannot create a new item.");
        goto cleanup;
    }

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

    *_self = self;
    status = true;

cleanup:
    if (!status) {
        error("Cannot unserialize the sub packet item '%x' ('%llx')", self->id, self->actor.uid);
        itemDestroy(&self);
    }

    return status;
}
