/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file item_factory.h
 * @brief
 *
 *
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#pragma once

// ---------- Includes ------------
#include "R1EMU.h"
#include "item.h"
#include "equipable/item_equipable.h"
#include "common/mysql/mysql.h"
#include "common/static_data/fields/item_common_data.h"
#include "common/static_data/fields/item_equip_data.h"

// ---------- Defines -------------


// ------ Structure declaration -------

// ----------- Functions ------------
/**
 * @brief Create a new item actor with a unique ID.
 */
Item *itemFactoryCreate(ItemId_t id, ItemAmount_t amount);
/**
 * @brief Create a new equipable item actor with a unique ID.
 */
ItemEquipable *itemFactoryCreateEquipable(ItemId_t id, ItemAmount_t amount, ItemEquipmentSlot_t slot);

bool itemFactoryStart(MySQLInfo *sqlInfo);

bool itemFactoryGetCategoryFromId(
    ItemId_t id,
    ItemCommonData *commonData,
    ItemEquipData *equipData,
    ItemCategory_t *category);

void itemFactoryGetStaticData(ItemId_t id, ItemCommonData **commonData, ItemEquipData **equipData);
