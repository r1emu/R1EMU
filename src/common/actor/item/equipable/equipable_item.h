/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file equipable_item.h
 * @brief
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#pragma once

// ---------- Includes -------------
#include "R1EMU.h"
#include "common/actor/item/item.h"

// ---------- Defines -------------

// ------ Structure declaration -------
typedef struct EquipableItem {
   Item item;
   int slot;
} EquipableItem;

// ----------- Functions ------------
/**
 * Allocate a new EquipableItem structure.
 * @return A pointer to an allocated EquipableItem, or NULL if an error occured.
 */
EquipableItem *equipableItemNew(void);

/**
 * Initialize an allocated EquipableItem structure.
 * @param self An allocated EquipableItem to initialize.
 * @return true on success, false otherwise.
 */
bool equipableItemInit(EquipableItem *self);

/**
 * Free an allocated EquipableItem structure.
 * @param self A pointer to an allocated EquipableItem.
 */
void equipableItemFree(EquipableItem *self);

/**
 * Free an allocated EquipableItem structure and nullify the content of the pointer.
 * @param self A pointer to an allocated EquipableItem.
 */
void equipableItemDestroy(EquipableItem **self);

/**
 * Getters & Setters
 */
inline ItemId_t equipableItemGetId(EquipableItem *self) {
    return itemGetId(&self->item);
}

inline ItemId_t equipableItemGetUId(EquipableItem *self) {
    return itemGetUId(&self->item);
}

inline int equipableItemGetSlot(EquipableItem *self) {
    return self->slot;
}

