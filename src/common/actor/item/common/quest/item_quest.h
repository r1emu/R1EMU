/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file item_quest.h
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
#define FOREACH_ITEM_QUEST(GENERATOR)       \
    GENERATOR(ITEM_QUEST, COOLDOWN, 3750)

typedef enum {
    FOREACH_ITEM_QUEST(GENERATE_PROPERTY_ENUM)
    ITEM_QUEST_PROPERTY_COUNT
}   ItemQuestProperty;

typedef enum {
    FOREACH_ITEM_QUEST(GENERATE_PROPERTY_ID)
}   ItemQuestId;

// ------ Structure declaration -------
typedef struct {
    Item item;
    float *cooldown;
}   ItemQuest;

typedef struct {
    PropertyFloatSPacket cooldown[0];
}   ItemQuestSPacket;

// ----------- Functions ------------

/**
 * Allocate a new ItemQuest structure.
 * @return A pointer to an allocated ItemQuest, or NULL if an error occured.
 */
ItemQuest *itemQuestNew(Item *item);

/**
 * Initialize an allocated ItemQuest structure.
 * @param self An allocated ItemQuest to initialize.
 * @return true on success, false otherwise.
 */
bool itemQuestInit(ItemQuest *self, Item *item);

/**
 * Free an allocated ItemQuest structure.
 * @param self A pointer to an allocated ItemQuest.
 */
void itemQuestFree(ItemQuest *self);

/**
 * Free an allocated ItemQuest structure and nullify the content of the pointer.
 * @param self A pointer to an allocated ItemQuest.
 */
void itemQuestDestroy(ItemQuest **self);


/**
 * Getters & Setters
 */
inline float *itemQuestGetCooldown(ItemQuest *self) { return self->cooldown; }


/**
 * Serialization / Unserialization
 */
size_t itemQuestGetCPacketSize(ItemQuest *self);
size_t itemQuestGetSPacketSize(ItemQuest *self);
void itemQuestSerializeCPacket(ItemQuest *self, PacketStream *stream);
void itemQuestSerializeSPacket(ItemQuest *self, PacketStream *stream);
bool itemQuestUnserializeSPacket(ItemQuest *self, PacketStream *stream);

/**
 * Debugging
 */
void itemQuestPrint(ItemQuest *self);
