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


// ------ Structure declaration -------
typedef struct {
    Item item;
    float *level;
    float *cooldown;
    float *itemExp;
}   ItemCard;

enum {
    ITEM_CARD_PROPERTY_ID_LEVEL = 3779,
    ITEM_CARD_PROPERTY_ID_COOLDOWN = 3750,
    ITEM_CARD_PROPERTY_ID_ITEM_EXP = 3768
};

// ----------- Functions ------------

/**
 * Allocate a new ItemCard structure.
 * @return A pointer to an allocated ItemCard, or NULL if an error occured.
 */
ItemCard *itemCardNew(void);

/**
 * Initialize an allocated ItemCard structure.
 * @param self An allocated ItemCard to initialize.
 * @return true on success, false otherwise.
 */
bool itemCardInit(ItemCard *self);

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
size_t itemCardGetPropertiesCPacketSize(ItemCard *self);
void itemCardGetPropertiesCPacket(ItemCard *self, PacketStream *stream);

