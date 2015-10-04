/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file item_material.h
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
    float *cooldown;
}   ItemMaterial;

enum {
    ITEM_MATERIAL_PROPERTY_ID_COOLDOWN = 3750
};

// ----------- Functions ------------

/**
 * Allocate a new ItemMaterial structure.
 * @return A pointer to an allocated ItemMaterial, or NULL if an error occured.
 */
ItemMaterial *itemMaterialNew(Item *item);

/**
 * Initialize an allocated ItemMaterial structure.
 * @param self An allocated ItemMaterial to initialize.
 * @return true on success, false otherwise.
 */
bool itemMaterialInit(ItemMaterial *self, Item *item);

/**
 * Free an allocated ItemMaterial structure.
 * @param self A pointer to an allocated ItemMaterial.
 */
void itemMaterialFree(ItemMaterial *self);

/**
 * Free an allocated ItemMaterial structure and nullify the content of the pointer.
 * @param self A pointer to an allocated ItemMaterial.
 */
void itemMaterialDestroy(ItemMaterial **self);


/**
 * Getters & Setters
 */
inline float *itemMaterialGetCooldown(ItemMaterial *self) { return self->cooldown; }


/**
 * Serialization / Unserialization
 */
size_t itemMaterialGetPropertiesCPacketSize(ItemMaterial *self);
void itemMaterialGetPropertiesCPacket(ItemMaterial *self, PacketStream *stream);

