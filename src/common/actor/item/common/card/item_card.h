/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file item_card.h
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
#define FOREACH_ITEM_CARD(GENERATOR)     \
    GENERATOR(ITEM_CARD, LEVEL, 3779)    \
    GENERATOR(ITEM_CARD, COOLDOWN, 3750) \
    GENERATOR(ITEM_CARD, ITEM_EXP, 3768)

typedef enum {
    FOREACH_ITEM_CARD(GENERATE_PROPERTY_ENUM)
    ITEM_CARD_PROPERTY_COUNT
}   ItemCardProperty;

typedef enum {
    FOREACH_ITEM_CARD(GENERATE_PROPERTY_ID)
}   ItemCardId;

// ------ Structure declaration -------
typedef struct {
    Item item;
    float *level;
    float *cooldown;
    float *itemExp;
}   ItemCard;

// ----------- Functions ------------

/**
 * Allocate a new ItemCard structure.
 * @return A pointer to an allocated ItemCard, or NULL if an error occured.
 */
ItemCard *itemCardNew(Item *item);

/**
 * Initialize an allocated ItemCard structure.
 * @param self An allocated ItemCard to initialize.
 * @return true on success, false otherwise.
 */
bool itemCardInit(ItemCard *self, Item *item);

/**
 * Free an allocated ItemCard structure.
 * @param self A pointer to an allocated ItemCard.
 */
void itemCardFree(ItemCard *self);

/**
 * Free an allocated ItemCard structure and nullify the content of the pointer.
 * @param self A pointer to an allocated ItemCard.
 */
void itemCardDestroy(ItemCard **self);


/**
 * Getters & Setters
 */
inline float *itemCardGetLevel(ItemCard *self) { return self->level; }
inline float *itemCardGetCooldown(ItemCard *self) { return self->cooldown; }
inline float *itemCardGetItemExp(ItemCard *self) { return self->itemExp; }


/**
 * Serialization / Unserialization
 */
size_t itemCardGetCPacketSize(ItemCard *self);
size_t itemCardGetSPacketSize(ItemCard *self);
void itemCardSerializeCPacket(ItemCard *self, PacketStream *stream);
void itemCardSerializeSPacket(ItemCard *self, PacketStream *stream);
bool itemCardUnserializeSPacket(ItemCard *self, PacketStream *stream);

/**
 * Debugging
 */
void itemCardPrint(ItemCard *self);
