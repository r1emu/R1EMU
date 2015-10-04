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
#include "common/actor/item/item.h"

// ---------- Defines -------------


// ------ Structure declaration -------
typedef struct {
    Item item;
    float *petPosition;
    float *cooldown;
}   ItemSubWeapon;

enum {
    ITEM_SUBWEAPON_PROPERTY_ID_PET_POSITION = 3703,
    ITEM_SUBWEAPON_PROPERTY_ID_COOLDOWN = 3843
};

// ----------- Functions ------------

/**
 * Allocate a new ItemSubWeapon structure.
 * @return A pointer to an allocated ItemSubWeapon, or NULL if an error occured.
 */
ItemSubWeapon *itemSubWeaponNew(void);

/**
 * Initialize an allocated ItemSubWeapon structure.
 * @param self An allocated ItemSubWeapon to initialize.
 * @return true on success, false otherwise.
 */
bool itemSubWeaponInit(ItemSubWeapon *self);

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
size_t itemSubWeaponGetPropertiesCPacketSize(ItemSubWeapon *self);
void itemSubWeaponGetPropertiesCPacket(ItemSubWeapon *self, PacketStream *stream);

/**
 * Getters and Setters
 */
inline float *itemSubWeaponGetPetPosition(ItemSubWeapon *self) { return self->petPosition; }
inline float *itemSubWeaponGetCooldown(ItemSubWeapon *self) { return self->cooldown; }
