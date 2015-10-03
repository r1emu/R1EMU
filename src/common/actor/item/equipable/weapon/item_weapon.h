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

// ---------- Defines -------------


// ------ Structure declaration -------
typedef struct ItemWeapon ItemWeapon;

// ----------- Functions ------------

/**
 * Allocate a new ItemWeapon structure.
 * @return A pointer to an allocated ItemWeapon, or NULL if an error occured.
 */
ItemWeapon *itemWeaponNew(void);

/**
 * Initialize an allocated ItemWeapon structure.
 * @param self An allocated ItemWeapon to initialize.
 * @return true on success, false otherwise.
 */
bool itemWeaponInit(ItemWeapon *self);

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

