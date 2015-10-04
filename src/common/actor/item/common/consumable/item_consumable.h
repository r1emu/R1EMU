/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file item_consumable.h
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
#include "common/property/property.h"

// ---------- Defines -------------


// ------ Structure declaration -------
enum {
    ITEM_CONSUMABLE_PROPERTY_ID_COOLDOWN = 3750
};

typedef struct {
    Item item;
    float *cooldown;
}   ItemConsumable;

// ----------- Functions ------------

/**
 * @return A pointer to an allocated ItemConsumable, or NULL if an error occured.
 * Allocate a new ItemConsumable structure.
 */
ItemConsumable *itemConsumableNew(Item *item);

/**
 * Initialize an allocated ItemConsumable structure.
 * @param self An allocated ItemConsumable to initialize.
 * @return true on success, false otherwise.
 */
bool itemConsumableInit(ItemConsumable *self, Item *item);

/**
 * Free an allocated ItemConsumable structure.
 * @param self A pointer to an allocated ItemConsumable.
 */
void itemConsumableFree(ItemConsumable *self);

/**
 * Free an allocated ItemConsumable structure and nullify the content of the pointer.
 * @param self A pointer to an allocated ItemConsumable.
 */
void itemConsumableDestroy(ItemConsumable **self);

/**
 * Getters & Setters
 */
inline float *itemConsumableGetCooldown(ItemConsumable *self) { return self->cooldown; }

/**
 * Serialization / Unserialization
 */
size_t itemConsumableGetPropertiesCPacketSize(ItemConsumable *self);
void itemConsumableGetPropertiesCPacket(ItemConsumable *self, PacketStream *stream);
