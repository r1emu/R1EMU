/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file item_book.h
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
#define FOREACH_ITEM_BOOK(GENERATOR)       \
    GENERATOR(ITEM_BOOK, COOLDOWN, 3750)

typedef enum {
    FOREACH_ITEM_BOOK(GENERATE_PROPERTY_ENUM)
    ITEM_BOOK_PROPERTY_COUNT
}   ItemBookProperty;

typedef enum {
    FOREACH_ITEM_BOOK(GENERATE_PROPERTY_ID)
}   ItemBookId;

// ------ Structure declaration -------
typedef struct {
    Item item;
    float *cooldown;
}   ItemBook;

typedef struct {
    PropertyFloatSPacket cooldown[0];
}   ItemBookSPacket;

// ----------- Functions ------------

/**
 * Allocate a new ItemBook structure.
 * @return A pointer to an allocated ItemBook, or NULL if an error occured.
 */
ItemBook *itemBookNew(Item *item);

/**
 * Initialize an allocated ItemBook structure.
 * @param self An allocated ItemBook to initialize.
 * @return true on success, false otherwise.
 */
bool itemBookInit(ItemBook *self, Item *item);

/**
 * Free an allocated ItemBook structure.
 * @param self A pointer to an allocated ItemBook.
 */
void itemBookFree(ItemBook *self);

/**
 * Free an allocated ItemBook structure and nullify the content of the pointer.
 * @param self A pointer to an allocated ItemBook.
 */
void itemBookDestroy(ItemBook **self);


/**
 * Getters & Setters
 */
inline float *itemBookGetCooldown(ItemBook *self) { return self->cooldown; }

/**
 * Serialization / Unserialization
 */
size_t itemBookGetCPacketSize(ItemBook *self);
size_t itemBookGetSPacketSize(ItemBook *self);
void itemBookSerializeCPacket(ItemBook *self, PacketStream *stream);
void itemBookSerializeSPacket(ItemBook *self, PacketStream *stream);
bool itemBookUnserializeSPacket(ItemBook *self, PacketStream *stream);

/**
 * Debugging
 */
void itemBookPrint(ItemBook *self);
