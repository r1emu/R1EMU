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


// ------ Structure declaration -------
typedef struct {
    Item item;
    float cooldown;
}   ItemQuest;

enum {
    ITEM_QUEST_PROPERTY_ID_COOLDOWN = 3750
};

// ----------- Functions ------------

/**
 * Allocate a new ItemQuest structure.
 * @return A pointer to an allocated ItemQuest, or NULL if an error occured.
 */
ItemQuest *itemQuestNew(void);

/**
 * Initialize an allocated ItemQuest structure.
 * @param self An allocated ItemQuest to initialize.
 * @return true on success, false otherwise.
 */
bool itemQuestInit(ItemQuest *self);

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
inline float itemQuestGetCooldown(ItemQuest *self) { return self->cooldown; }


/**
 * Serialization / Unserialization
 */
size_t itemQuestGetPropertiesCPacketSize(ItemQuest *self);
void itemQuestGetPropertiesCPacket(ItemQuest *self, PacketStream *stream);

