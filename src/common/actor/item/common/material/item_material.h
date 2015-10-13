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
// Define properties ID
#define FOREACH_ITEM_MATERIAL(GENERATOR)       \
    GENERATOR(ITEM_MATERIAL, COOLDOWN, 3750)

typedef enum {
    FOREACH_ITEM_MATERIAL(GENERATE_PROPERTY_ENUM)
    ITEM_MATERIAL_PROPERTY_COUNT
}   ItemMaterialProperty;

typedef enum {
    FOREACH_ITEM_MATERIAL(GENERATE_PROPERTY_ID)
}   ItemMaterialId;

// ------ Structure declaration -------
typedef struct {
    Item item;
    float *cooldown;
}   ItemMaterial;

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
size_t itemMaterialGetCPacketSize(ItemMaterial *self);
size_t itemMaterialGetSPacketSize(ItemMaterial *self);
void itemMaterialSerializeCPacket(ItemMaterial *self, PacketStream *stream);
void itemMaterialSerializeSPacket(ItemMaterial *self, PacketStream *stream);
bool itemMaterialUnserializeSPacket(ItemMaterial *self, PacketStream *stream);

/**
 * Debugging
 */
void itemMaterialPrint(ItemMaterial *self);
