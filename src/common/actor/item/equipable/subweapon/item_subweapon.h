/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file item_subweapon.h
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
#define FOREACH_ITEM_SUBWEAPON(GENERATOR)          \
    GENERATOR(ITEM_SUBWEAPON, PET_POSITION , 3703) \
    GENERATOR(ITEM_SUBWEAPON, COOLDOWN, 3843)

typedef enum {
    FOREACH_ITEM_SUBWEAPON(GENERATE_PROPERTY_ENUM)
    ITEM_SUBWEAPON_PROPERTY_COUNT
}   ItemSubWeaponProperty;

typedef enum {
    FOREACH_ITEM_SUBWEAPON(GENERATE_PROPERTY_ID)
}   ItemSubWeaponId;

// ------ Structure declaration -------
typedef struct {
    ItemEquipable equipable;
    float *petPosition;
    float *cooldown;
}   ItemSubWeapon;

// ----------- Functions ------------

/**
 * Allocate a new ItemSubWeapon structure.
 * @return A pointer to an allocated ItemSubWeapon, or NULL if an error occured.
 */
ItemSubWeapon *itemSubWeaponNew(Item *item);

/**
 * Initialize an allocated ItemSubWeapon structure.
 * @param self An allocated ItemSubWeapon to initialize.
 * @return true on success, false otherwise.
 */
bool itemSubWeaponInit(ItemSubWeapon *self, Item *item);

/**
 * Free an allocated ItemSubWeapon structure.
 * @param self A pointer to an allocated ItemSubWeapon.
 */
void itemSubWeaponFree(ItemSubWeapon *self);

/**
 * Free an allocated ItemSubWeapon structure and nullify the content of the pointer.
 * @param self A pointer to an allocated ItemSubWeapon.
 */
void itemSubWeaponDestroy(ItemSubWeapon **self);

/**
 * Serialization / Unserialization
 */
size_t itemSubWeaponGetCPacketSize(ItemSubWeapon *self);
size_t itemSubWeaponGetSPacketSize(ItemSubWeapon *self);
void itemSubWeaponSerializeCPacket(ItemSubWeapon *self, PacketStream *stream);
void itemSubWeaponSerializeSPacket(ItemSubWeapon *self, PacketStream *stream);
bool itemSubWeaponUnserializeSPacket(ItemSubWeapon *self, PacketStream *stream);

/**
 * Getters and Setters
 */
inline float *itemSubWeaponGetPetPosition(ItemSubWeapon *self) { return self->petPosition; }
inline float *itemSubWeaponGetCooldown(ItemSubWeapon *self) { return self->cooldown; }

/**
 * Debugging
 */
void itemSubWeaponPrint(ItemSubWeapon *self);
