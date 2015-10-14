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
#include "common/actor/item/equipable/item_equipable.h"
#include "common/property/property.h"

// ---------- Defines -------------
// Define properties ID
#define FOREACH_ITEM_ACCESSORY(GENERATOR)       \
    GENERATOR(ITEM_ACCESSORY, PR, 3781)         \
    GENERATOR(ITEM_ACCESSORY, COOLDOWN, 3843)

typedef enum {
    FOREACH_ITEM_ACCESSORY(GENERATE_PROPERTY_ENUM)
    ITEM_ACCESSORY_PROPERTY_COUNT
}   ItemAccessoryProperty;

typedef enum {
    FOREACH_ITEM_ACCESSORY(GENERATE_PROPERTY_ID)
}   ItemAccessoryId;

// ------ Structure declaration -------
typedef struct {
    ItemEquipable equipable;
    float *pr;
    float *cooldown;
}   ItemAccessory;

#pragma pack(push, 1)
typedef struct {
    PropertyFloatSPacket pr[0];
    PropertyFloatSPacket cooldown[0];
}   ItemAccessorySPacket;
#pragma pack(pop)

// ----------- Functions ------------

/**
 * Allocate a new ItemAccessory structure.
 * @return A pointer to an allocated ItemAccessory, or NULL if an error occured.
 */
ItemAccessory *itemAccessoryNew(Item *item, ItemEquipData *data);

/**
 * Initialize an allocated ItemAccessory structure.
 * @param self An allocated ItemAccessory to initialize.
 * @return true on success, false otherwise.
 */
bool itemAccessoryInit(ItemAccessory *self, Item *item, ItemEquipData *data);

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
size_t itemAccessoryGetCPacketSize(ItemAccessory *self);
size_t itemAccessoryGetSPacketSize(ItemAccessory *self);
void itemAccessorySerializeCPacket(ItemAccessory *self, PacketStream *stream);
void itemAccessorySerializeSPacket(ItemAccessory *self, PacketStream *stream);
size_t itemAccessoryUnserializeSPacket(ItemAccessory *self, PacketStream *stream);

/**
 * Debugging
 */
void itemAccessoryPrint(ItemAccessory *self);
