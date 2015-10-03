/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file item_equipable.h
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
typedef struct ItemEquipable {
   Item item;
   int slot;
} ItemEquipable;

// ----------- Functions ------------
/**
 * Allocate a new ItemEquipable structure.
 * @return A pointer to an allocated ItemEquipable, or NULL if an error occured.
 */
ItemEquipable *equipableItemNew(void);

/**
 * Initialize an allocated ItemEquipable structure.
 * @param self An allocated ItemEquipable to initialize.
 * @return true on success, false otherwise.
 */
bool equipableItemInit(ItemEquipable *self);

/**
 * Free an allocated ItemEquipable structure.
 * @param self A pointer to an allocated ItemEquipable.
 */
void equipableItemFree(ItemEquipable *self);

/**
 * Free an allocated ItemEquipable structure and nullify the content of the pointer.
 * @param self A pointer to an allocated ItemEquipable.
 */
void equipableItemDestroy(ItemEquipable **self);

/**
 * Getters & Setters
 */
inline int equipableItemGetSlot(ItemEquipable *self) { return self->slot; }
