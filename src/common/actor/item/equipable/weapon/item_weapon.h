/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file item_weapon.h
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
#include "common/actor/item/equipable/item_equipable.h"

// ---------- Defines -------------


// ------ Structure declaration -------
typedef struct {
    ItemEquipable equipable;
    float *maxAtk;
    float *minAtk;
    float *cooldown;
}   ItemWeapon;

enum {
    ITEM_WEAPON_PROPERTY_ID_MAXATK = 3796,
    ITEM_WEAPON_PROPERTY_ID_MINATK = 3858,
    ITEM_WEAPON_PROPERTY_ID_COOLDOWN = 3843
};

// ----------- Functions ------------

/**
 * Allocate a new ItemWeapon structure.
 * @return A pointer to an allocated ItemWeapon, or NULL if an error occured.
 */
ItemWeapon *itemWeaponNew(Item *item);

/**
 * Initialize an allocated ItemWeapon structure.
 * @param self An allocated ItemWeapon to initialize.
 * @return true on success, false otherwise.
 */
bool itemWeaponInit(ItemWeapon *self, Item *item);

/**
 * Free an allocated ItemWeapon structure.
 * @param self A pointer to an allocated ItemWeapon.
 */
void itemWeaponFree(ItemWeapon *self);

/**
 * Free an allocated ItemWeapon structure and nullify the content of the pointer.
 * @param self A pointer to an allocated ItemWeapon.
 */
void itemWeaponDestroy(ItemWeapon **self);

/**
 * Serialization / Unserialization
 */
size_t itemWeaponGetPropertiesCPacketSize(ItemWeapon *self);
void itemWeaponGetPropertiesCPacket(ItemWeapon *self, PacketStream *stream);

/**
 * Getters and Setters
 */
inline float *itemWeaponGetMaxAtk(ItemWeapon *self) { return self->maxAtk; }
inline float *itemWeaponGetMinAtk(ItemWeapon *self) { return self->minAtk; }
inline float *itemWeaponGetCooldown(ItemWeapon *self) { return self->cooldown; }

/**
 * Debugging
 */
void itemWeaponPrint(ItemWeapon *self);
