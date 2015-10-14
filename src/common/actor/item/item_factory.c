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
#include "common/mysql/fields/mysql_item_equip_data.h"
#include "common/mysql/fields/mysql_item_common_data.h"
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
    StaticData *itemCommonDatabase;
    StaticData *itemEquipDatabase;
    zhash_t *categoryEquipTable;
    zhash_t *categoryCommonTable;
    ItemCategory_t *categories;
} self = {0};

static bool itemFactoryInitCategories(void) {

    if (!(self.categories = malloc(sizeof(*self.categories) * ITEM_CAT_COUNT))) {
        error("Cannot allocate categories.");
        return false;
    }

    for (ItemCategory_t cat = 0; cat < ITEM_CAT_COUNT; cat++) {
        self.categories[cat] = cat;
    }

    return true;
}

static bool itemFactoryInitCategoryEquipTable(void) {

    bool status = false;

    if (!(self.categoryEquipTable = zhash_new())) {
        error("Cannot allocate category table.");
        goto cleanup;
    }

    if ((zhash_insert(self.categoryEquipTable, "Armor", &self.categories[ITEM_CAT_ARMOR]) != 0)
    ||  (zhash_insert(self.categoryEquipTable, "Helmet", &self.categories[ITEM_CAT_ARMOR]) != 0)
    ||  (zhash_insert(self.categoryEquipTable, "Weapon", &self.categories[ITEM_CAT_WEAPON]) != 0)
    ||  (zhash_insert(self.categoryEquipTable, "Armband", &self.categories[ITEM_CAT_ARMOR]) != 0)
    ||  (zhash_insert(self.categoryEquipTable, "SubWeapon", &self.categories[ITEM_CAT_SUBWEAPON]) != 0))
    {
        error("Cannot insert new category in table.");
        goto cleanup;
    }

    status = true;
cleanup:
    if (!status) {
        zhash_destroy(&self.categoryEquipTable);
    }

    return status;
}

static bool itemFactoryInitCategoryCommonTable(void) {

    bool status = false;

    if (!(self.categoryCommonTable = zhash_new())) {
        error("Cannot allocate category table.");
        goto cleanup;
    }

    if ((zhash_insert(self.categoryCommonTable, "Consume", &self.categories[ITEM_CAT_CONSUMABLE]) != 0)
    ||  (zhash_insert(self.categoryCommonTable, "Equip", &self.categories[ITEM_CAT_ARMOR]) != 0)
    ||  (zhash_insert(self.categoryCommonTable, "Etc", &self.categories[ITEM_CAT_ACCESSORY]) != 0)
    ||  (zhash_insert(self.categoryCommonTable, "Quest", &self.categories[ITEM_CAT_QUEST]) != 0)
    ||  (zhash_insert(self.categoryCommonTable, "Unused", &self.categories[ITEM_CAT_CURRENCY]) != 0))
    {
        error("Cannot insert new category in table.");
        goto cleanup;
    }

    status = true;
cleanup:
    if (!status) {
        zhash_destroy(&self.categoryCommonTable);
    }

    return status;
}

bool itemFactoryStart(MySQLInfo *sqlInfo) {

    if (!(self.sql = mySqlNew(sqlInfo))) {
        error("Cannot start MySQL.");
        return false;
    }

    if (!(mySqlConnect(self.sql))) {
        error("Cannot connect to the SQL.");
        return false;
    }

    if (!(itemFactoryInitCategories())) {
        error("Cannot initialize factory categories.");
        return false;
    }

    if (!(mySqlBuildItemEquipDataDb(self.sql, &self.itemEquipDatabase))) {
        error("Cannot start equipable item static database.");
        return false;
    }

    if (!(mySqlBuildItemCommonDataDb(self.sql, &self.itemCommonDatabase))) {
        error("Cannot start common item static database.");
        return false;
    }

    if (!(itemFactoryInitCategoryEquipTable())) {
        error("Cannot initialize category equip table");
        return false;
    }

    if (!(itemFactoryInitCategoryCommonTable())) {
        error("Cannot initialize category common table");
        return false;
    }

    staticDataLock(self.itemEquipDatabase);
    staticDataLock(self.itemCommonDatabase);

    return true;
}

bool itemFactoryGetCategoryFromId(
    ItemId_t id,
    ItemCommonData *commonData,
    ItemEquipData *equipData,
    ItemCategory_t *_category)
{
    ItemCategory_t *category = NULL;

    /**
     * We might want to refactor this function once we got a full item database
     */
    if (!commonData) {
        // This information is maybe contained in equipData
        if (!equipData) {
            error("Cannot get category for id '%d'", id);
            return false;
        }

        if (!(category = zhash_lookup(self.categoryEquipTable, equipData->GroupName))) {
            error("Cannot get the category for the groupName '%s'", equipData->GroupName);
            return false;
        }
    }
    else {
        if (!(strcmp(commonData->ItemType, "Equip"))) {
            // Equipment item, find out more information
            if (equipData) {
                if (!(category = zhash_lookup(self.categoryEquipTable, equipData->GroupName))) {
                    error("Cannot get the category for the groupName '%s'", equipData->GroupName);
                    return false;
                }
            } else {
                warning("ItemID '%d' has not been found in equipData. Guess category 'Armor'", id);
                category = &self.categories[ITEM_CAT_ARMOR];
            }
        } else {
            if (!(category = zhash_lookup(self.categoryCommonTable, commonData->ItemType))) {
                error("Cannot get the category for the ItemType '%s'", commonData->ItemType);
                return false;
            }
        }
    }

    *_category = *category;

    return true;
}

void itemFactoryGetStaticData(ItemId_t id, ItemCommonData **commonData, ItemEquipData **equipData) {
    // Get static data related to the item
    staticDataGet(self.itemCommonDatabase, id, commonData, false);
    staticDataGet(self.itemEquipDatabase, id, equipData, false);
}

ItemEquipable *itemFactoryCreateEquipable(ItemId_t id, ItemAmount_t amount, ItemEquipmentSlot_t slot) {
    ItemEquipable *item = NULL;

    if (!(item = (ItemEquipable *) itemFactoryCreate(id, amount))) {
        error("Cannot create a new item.");
        return NULL;
    }

    item->slot = slot;
    return item;
}

Item *itemFactoryCreate(ItemId_t id, ItemAmount_t amount) {

    Item *item = NULL;
    bool status = false;

    ItemCategory_t category;
    ItemCommonData *commonData = NULL;
    ItemEquipData *equipData = NULL;

    itemFactoryGetStaticData(id, &commonData, &equipData);

    if (!(itemFactoryGetCategoryFromId(id, commonData, equipData, &category))) {
        error("Cannot get category from ID '%d'", id);
        goto cleanup;
    }

    // Initialize a unique actor
    Actor actor;
    if (!(actorFactoryInit(&actor))) {
        error("Cannot initialize a new actor.");
        goto cleanup;
    }

    if (!(item = itemNew(&actor, category, id, amount, commonData, equipData))) {
        error("Cannot allocate a new item.");
        goto cleanup;
    }

    status = true;

cleanup:
    if (!status) {
        itemDestroy(&item);
    }

    return status ? item : NULL;
}
