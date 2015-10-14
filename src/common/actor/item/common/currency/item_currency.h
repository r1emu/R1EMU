/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file item_currency.h
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
#define FOREACH_ITEM_CURRENCY(GENERATOR)       \
    GENERATOR(ITEM_CURRENCY, COOLDOWN, 3750)

typedef enum {
    FOREACH_ITEM_CURRENCY(GENERATE_PROPERTY_ENUM)
    ITEM_CURRENCY_PROPERTY_COUNT
}   ItemCurrencyProperty;

typedef enum {
    FOREACH_ITEM_CURRENCY(GENERATE_PROPERTY_ID)
}   ItemCurrencyId;

// ------ Structure declaration -------
typedef struct {
    Item item;
    float *cooldown;
}   ItemCurrency;

#pragma pack(push, 1)
typedef struct {
    PropertyFloatSPacket cooldown[0];
}   ItemCurrencySPacket;
#pragma pack(pop)

// ----------- Functions ------------

/**
 * Allocate a new ItemCurrency structure.
 * @return A pointer to an allocated ItemCurrency, or NULL if an error occured.
 */
ItemCurrency *itemCurrencyNew(Item *item);

/**
 * Initialize an allocated ItemCurrency structure.
 * @param self An allocated ItemCurrency to initialize.
 * @return true on success, false otherwise.
 */
bool itemCurrencyInit(ItemCurrency *self, Item *item);

/**
 * Free an allocated ItemCurrency structure.
 * @param self A pointer to an allocated ItemCurrency.
 */
void itemCurrencyFree(ItemCurrency *self);

/**
 * Free an allocated ItemCurrency structure and nullify the content of the pointer.
 * @param self A pointer to an allocated ItemCurrency.
 */
void itemCurrencyDestroy(ItemCurrency **self);

/**
 * Getters & Setters
 */
inline float *itemCurrencyGetCooldown(ItemCurrency *self) { return self->cooldown; }

/**
 * Serialization / Unserialization
 */
size_t itemCurrencyGetCPacketSize(ItemCurrency *self);
size_t itemCurrencyGetSPacketSize(ItemCurrency *self);
void itemCurrencySerializeCPacket(ItemCurrency *self, PacketStream *stream);
void itemCurrencySerializeSPacket(ItemCurrency *self, PacketStream *stream);
bool itemCurrencyUnserializeSPacket(ItemCurrency *self, PacketStream *stream);

/**
 * Debugging
 */
void itemCurrencyPrint(ItemCurrency *self);
