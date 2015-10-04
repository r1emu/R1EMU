/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file item_accessory.h
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
typedef struct {
    Item item;
    float *pr;
    float *cooldown;
}   ItemAccessory;

enum {
    ITEM_ACCESSORY_PROPERTY_ID_PR = 3781,
    ITEM_ACCESSORY_PROPERTY_ID_COOLDOWN = 3843
};

// ----------- Functions ------------

/**
 * Allocate a new ItemAccessory structure.
 * @return A pointer to an allocated ItemAccessory, or NULL if an error occured.
 */
ItemAccessory *itemAccessoryNew(void);

/**
 * Initialize an allocated ItemAccessory structure.
 * @param self An allocated ItemAccessory to initialize.
 * @return true on success, false otherwise.
 */
bool itemAccessoryInit(ItemAccessory *self);

/**
 * Free an allocated ItemAccessory structure.
 * @param self A pointer to an allocated ItemAccessory.
 */
void itemAccessoryFree(ItemAccessory *self);

/**
 * Free an allocated ItemAccessory structure and nullify the content of the pointer.
 * @param self A pointer to an allocated ItemAccessory.
 */
void itemAccessoryDestroy(ItemAccessory **self);


/**
 * Getters & Setters
 */
inline float *itemAccessoryGetPr(ItemAccessory *self) { return self->pr; }
inline float *itemAccessoryGetCooldown(ItemAccessory *self) { return self->cooldown; }


/**
 * Serialization / Unserialization
 */
size_t itemAccessoryGetPropertiesCPacketSize(ItemAccessory *self);
void itemAccessoryGetPropertiesCPacket(ItemAccessory *self, PacketStream *stream);

