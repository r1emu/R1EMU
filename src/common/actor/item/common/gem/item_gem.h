/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file item_gem.h
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
#define FOREACH_ITEM_GEM(GENERATOR)       \
    GENERATOR(ITEM_GEM, LEVEL, 3779)      \
    GENERATOR(ITEM_GEM, ITEM_EXP, 3768)   \
    GENERATOR(ITEM_GEM, COOLDOWN, 3843)

typedef enum {
    FOREACH_ITEM_GEM(GENERATE_PROPERTY_ENUM)
    ITEM_GEM_PROPERTY_COUNT
}   ItemGemProperty;

typedef enum {
    FOREACH_ITEM_GEM(GENERATE_PROPERTY_ID)
}   ItemGemId;

// ------ Structure declaration -------
typedef struct {
    Item item;
    float *level;
    float *itemExp;
    float *cooldown;
}   ItemGem;

typedef struct {
    PropertyFloatSPacket level[0];
    PropertyFloatSPacket itemExp[0];
    PropertyFloatSPacket cooldown[0];
}   ItemGemSPacket;

// ----------- Functions ------------

/**
 * Allocate a new ItemGem structure.
 * @return A pointer to an allocated ItemGem, or NULL if an error occured.
 */
ItemGem *itemGemNew(Item *item);

/**
 * Initialize an allocated ItemGem structure.
 * @param self An allocated ItemGem to initialize.
 * @return true on success, false otherwise.
 */
bool itemGemInit(ItemGem *self, Item *item);

/**
 * Free an allocated ItemGem structure.
 * @param self A pointer to an allocated ItemGem.
 */
void itemGemFree(ItemGem *self);

/**
 * Free an allocated ItemGem structure and nullify the content of the pointer.
 * @param self A pointer to an allocated ItemGem.
 */
void itemGemDestroy(ItemGem **self);

/**
 * Getters & Setters
 */
inline float *itemGemGetLevel(ItemGem *self) { return self->level; }
inline float *itemGemGetItemExp(ItemGem *self) { return self->itemExp; }
inline float *itemGemGetCooldown(ItemGem *self) { return self->cooldown; }

/**
 * Serialization / Unserialization
 */
size_t itemGemGetCPacketSize(ItemGem *self);
size_t itemGemGetSPacketSize(ItemGem *self);
void itemGemSerializeCPacket(ItemGem *self, PacketStream *stream);
void itemGemSerializeSPacket(ItemGem *self, PacketStream *stream);
bool itemGemUnserializeSPacket(ItemGem *self, PacketStream *stream);

/**
 * Debugging
 */
void itemGemPrint(ItemGem *self);
