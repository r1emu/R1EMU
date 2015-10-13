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
// Define properties ID
#define FOREACH_ITEM_WEAPON(GENERATOR)      \
    GENERATOR(ITEM_WEAPON, MAXATK , 3796)   \
    GENERATOR(ITEM_WEAPON, MINATK , 3858)   \
    GENERATOR(ITEM_WEAPON, COOLDOWN, 3843)

typedef enum {
    FOREACH_ITEM_WEAPON(GENERATE_PROPERTY_ENUM)
    ITEM_WEAPON_PROPERTY_COUNT
}   ItemWeaponProperty;

typedef enum {
    FOREACH_ITEM_WEAPON(GENERATE_PROPERTY_ID)
}   ItemWeaponId;

// ------ Structure declaration -------
typedef struct {
    ItemEquipable equipable;
    float *maxAtk;
    float *minAtk;
    float *cooldown;
}   ItemWeapon;

typedef struct {
    PropertyFloatSPacket maxAtk[0];
    PropertyFloatSPacket minAtk[0];
    PropertyFloatSPacket cooldown[0];
}   ItemWeaponSPacket;

// ----------- Functions ------------

/**
 * Allocate a new ItemWeapon structure.
 * @return A pointer to an allocated ItemWeapon, or NULL if an error occured.
 */
ItemWeapon *itemWeaponNew(Item *item, ItemEquipData *data);

/**
 * Initialize an allocated ItemWeapon structure.
 * @param self An allocated ItemWeapon to initialize.
 * @return true on success, false otherwise.
 */
bool itemWeaponInit(ItemWeapon *self, Item *item, ItemEquipData *data);

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
size_t itemWeaponGetCPacketSize(ItemWeapon *self);
size_t itemWeaponGetSPacketSize(ItemWeapon *self);
void itemWeaponSerializeCPacket(ItemWeapon *self, PacketStream *stream);
void itemWeaponSerializeSPacket(ItemWeapon *self, PacketStream *stream);
bool itemWeaponUnserializeSPacket(ItemWeapon *self, PacketStream *stream);

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
