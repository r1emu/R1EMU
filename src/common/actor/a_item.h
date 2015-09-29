/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file commander_helpers.h
 * @brief CommanderInfo contains all commander helpers functions
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#pragma once

#include "R1EMU.h"
#include "actor.h"

#define ITEM_KEY_SIZE 17

typedef uint8_t ItemKey[ITEM_KEY_SIZE];

typedef struct Item {
   Actor actor;
   uint64_t itemDBId;
   uint32_t itemId;
   uint32_t amount;
   int itemCategory; // StaticData
} Item;

bool itemInit(Item *self);
uint32_t itemGetAmount(Item *self);
uint32_t itemGetItemId(Item *self);
uint32_t itemGetCategory(Item *self);

/**
 * Generate a key for an item from ItemId
 */
void itemGenKey(uint64_t itemIdKey, ItemKey itemKey);

