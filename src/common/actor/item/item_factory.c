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

#include "item_factory.h"
#include "common/actor/actor_factory.h"
#include "common/mysql/fields/mysql_item_data.h"
#include "common/static_data/static_data.h"

// Items categories
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

static struct ItemFactory {
    MySQL *sql;
    StaticData *itemDatabase;
} self = {0};

bool itemFactoryStart(MySQLInfo *sqlInfo) {

    if (!(self.sql = mySqlNew(sqlInfo))) {
        error("Cannot start MySQL.");
        return false;
    }

    if (!(mySqlConnect(self.sql))) {
        error("Cannot connect to the SQL.");
        return false;
    }

    if (!(mySqlBuildItemDataDb(self.sql, &self.itemDatabase))) {
        error("Cannot start item static data database.");
        return false;
    }

    staticDataLock(self.itemDatabase);

    return true;
}

// TODO : ItemId_t should be enough to know the correct category
// Once StaticData is available, link them together
Item *itemFactoryCreate(ItemCategory category, ItemId_t id, ItemAmount_t amount) {

    Item *item = NULL;
    size_t itemSize = 0;

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

        case ITEM_CAT_COUNT      : error("Unexpected item category."); return NULL; break;
    }

    if (!(item = malloc(itemSize))) {
        error("Cannot allocate a new item.");
        return NULL;
    }

    if (!(itemFactoryInit(item, category, id, amount))) {
        error("Cannot initialize a new item.");
        free(item);
        return NULL;
    }

    return item;
}

bool itemFactoryInit(Item *newItem, ItemCategory category, ItemId_t id, ItemAmount_t amount) {

    Actor actor;
    Item item;

    // Initialize a unique actor
    if (!(actorFactoryInit(&actor))) {
        error("Cannot initialize a new actor.");
        return false;
    }

    // Initialize the item
    if (!(itemInit(&item, &actor, category, id, amount))) {
        error("Cannot create a new item '%d'.", id);
        return false;
    }

    // Get static data related to the item
    if (!(staticDataGet(self.itemDatabase, id, &item.data))) {
        error("Cannot retrieve static data for object id '%d'", id);
        return false;
    }

    // Initialize the child class
    switch (category) {

        case ITEM_CAT_CONSUMABLE : {
            ItemConsumable *consumable = (ItemConsumable *) newItem;
            if (!(itemConsumableInit(consumable, &item))) {
                error("Cannot initialize a consumable item.");
                return false;
            }
            break;
        }
        case ITEM_CAT_ARMOR      : {
            ItemArmor *armor = (ItemArmor *) newItem;
            if (!(itemArmorInit(armor, &item))) {
                error("Cannot initialize a armor item.");
                return false;
            }
            break;
        }
        case ITEM_CAT_QUEST      : {
            ItemQuest *quest = (ItemQuest *) newItem;
            if (!(itemQuestInit(quest, &item))) {
                error("Cannot initialize a quest item.");
                return false;
            }
            break;
        }
        case ITEM_CAT_BOOK       : {
            ItemBook *book = (ItemBook *) newItem;
            if (!(itemBookInit(book, &item))) {
                error("Cannot initialize a book item.");
                return false;
            }
            break;
        }
        case ITEM_CAT_MATERIAL   : {
            ItemMaterial *material = (ItemMaterial *) newItem;
            if (!(itemMaterialInit(material, &item))) {
                error("Cannot initialize a material item.");
                return false;
            }
            break;
        }
        case ITEM_CAT_GEM        : {
            ItemGem *gem = (ItemGem *) newItem;
            if (!(itemGemInit(gem, &item))) {
                error("Cannot initialize a gem item.");
                return false;
            }
            break;
        }
        case ITEM_CAT_WEAPON     : {
            ItemWeapon *weapon = (ItemWeapon *) newItem;
            if (!(itemWeaponInit(weapon, &item))) {
                error("Cannot initialize a weapon item.");
                return false;
            }
            break;
        }
        case ITEM_CAT_CARD       : {
            ItemCard *card = (ItemCard *) newItem;
            if (!(itemCardInit(card, &item))) {
                error("Cannot initialize a card item.");
                return false;
            }
            break;
        }
        case ITEM_CAT_ACCESSORY  : {
            ItemAccessory *accessory = (ItemAccessory *) newItem;
            if (!(itemAccessoryInit(accessory, &item))) {
                error("Cannot initialize a accessory item.");
                return false;
            }
            break;
        }
        case ITEM_CAT_SUBWEAPON  : {
            ItemSubWeapon *subweapon = (ItemSubWeapon *) newItem;
            if (!(itemSubWeaponInit(subweapon, &item))) {
                error("Cannot initialize a subweapon item.");
                return false;
            }
            break;
        }
        case ITEM_CAT_CURRENCY   : {
            ItemCurrency *currency = (ItemCurrency *) newItem;
            if (!(itemCurrencyInit(currency, &item))) {
                error("Cannot initialize a currency item.");
                return false;
            }
            break;
        }

        case ITEM_CAT_COUNT      : {
            error("Invalid item category");
            return false;
            break;
        }
    }

    return true;
}
