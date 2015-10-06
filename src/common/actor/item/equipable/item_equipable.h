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
typedef enum {
    EQSLOT_HAT = 0,
    EQSLOT_HAT_L,
    EQSLOT_UNKOWN1,
    EQSLOT_BODY_ARMOR,
    EQSLOT_GLOVES,
    EQSLOT_BOOTS,
    EQSLOT_HELMET,
    EQSLOT_BRACELET,
    EQSLOT_WEAPON,
    EQSLOT_SHIELD,
    EQSLOT_COSTUME,
    EQSLOT_UNKOWN3,
    EQSLOT_UNKOWN4,
    EQSLOT_UNKOWN5,
    EQSLOT_LEG_ARMOR,
    EQSLOT_UNKOWN6,
    EQSLOT_UNKOWN7,
    EQSLOT_RING_LEFT,
    EQSLOT_RING_RIGHT,
    EQSLOT_NECKLACE,
    EQSLOT_COUNT,
    EQSLOT_NOSLOT,
} ItemEquipmentSlot;

typedef struct ItemEquipable {
   Item item;
   ItemEquipmentSlot slot;
} ItemEquipable;

// ----------- Functions ------------
/**
 * Allocate a new ItemEquipable structure.
 * @return A pointer to an allocated ItemEquipable, or NULL if an error occured.
 */
ItemEquipable *itemEquipableNew(Item *item);

/**
 * Initialize an allocated ItemEquipable structure.
 * @param self An allocated ItemEquipable to initialize.
 * @return true on success, false otherwise.
 */
bool itemEquipableInit(ItemEquipable *self, Item *item);

/**
 * Free an allocated ItemEquipable structure.
 * @param self A pointer to an allocated ItemEquipable.
 */
void itemEquipableFree(ItemEquipable *self);

/**
 * Free an allocated ItemEquipable structure and nullify the content of the pointer.
 * @param self A pointer to an allocated ItemEquipable.
 */
void itemEquipableDestroy(ItemEquipable **self);

/**
 * Getters & Setters
 */
inline int itemEquipableGetSlot(ItemEquipable *self) { return self->slot; }

/**
 * Debugging
 */
void itemEquipablePrint(ItemEquipable *self);
