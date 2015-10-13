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
#include "common/mysql/mysql.h"
#include "common/static_data/fields/item_common_data.h"
#include "common/static_data/fields/item_equip_data.h"

// ---------- Defines -------------


// ------ Structure declaration -------

// ----------- Functions ------------
Item *itemFactoryCreate(ItemId_t id, ItemAmount_t amount);
bool itemFactoryInit(
    Item *newItem,
    ItemCategory category,
    ItemCommonData *commonData,
    ItemEquipData *equipData,
    ItemId_t id,
    ItemAmount_t amount);
bool itemFactoryStart(MySQLInfo *sqlInfo);

bool itemFactoryGetCategoryFromId(
    ItemId_t id,
    ItemCommonData *commonData,
    ItemEquipData *equipData,
    ItemCategory *category);

void itemFactoryGetStaticData(ItemId_t id, ItemCommonData **commonData, ItemEquipData **equipData);
