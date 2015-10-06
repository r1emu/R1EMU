/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file item.h
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
#include "common/property/property.h"
#include "common/actor/actor.h"
#include "common/packet/packet_stream.h"
#include "common/static_data/fields/item_data.h"

// ---------- Defines -------------


// ------ Structure declaration -------
/** Item fields packet size */
typedef uint32_t ItemId_t;
typedef uint32_t ItemAmount_t;
typedef uint32_t ItemPrice_t;
typedef uint32_t ItemInventoryIndex_t;

/** Item categories */
typedef enum {
    ITEM_CAT_CONSUMABLE,
    ITEM_CAT_ARMOR,
    ITEM_CAT_QUEST,
    ITEM_CAT_BOOK,
    ITEM_CAT_MATERIAL,
    ITEM_CAT_GEM,
    ITEM_CAT_WEAPON,
    ITEM_CAT_CARD,
    ITEM_CAT_ACCESSORY,
    ITEM_CAT_SUBWEAPON,
    ITEM_CAT_CURRENCY,
    ITEM_CAT_COUNT,
}   ItemCategory;

/**
 * @brief Item is the server structure of an item
 */
typedef struct {
    Actor actor;
    ItemId_t id;
    ItemAmount_t amount;
    ItemCategory category;
    ItemData *data;
} Item;

/**
 * @brief ItemCPacket is the client packet structure of an item
 */
typedef struct {
    ActorId_t uid;
    ItemId_t id;
    ItemAmount_t amount;
    ItemInventoryIndex_t inventoryIndex;
} ItemCPacket;

/**
 * @brief ItemCPacket is the client packet structure of an item
 */
typedef struct {
    ActorSPacket actor;
    ItemId_t id;
    ItemAmount_t amount;
    ItemCategory category;
} ItemSPacket;

// ----------- Functions ------------
/**
 * Allocate a new Item structure.
 * @return A pointer to an allocated Item, or NULL if an error occured.
 */
Item *itemNew(Actor *actor, ItemCategory category, ItemId_t id, ItemAmount_t amount);

/**
 * Initialize an allocated Item structure.
 * @param self An allocated Item to initialize.
 * @return true on success, false otherwise.
 */
bool itemInit(Item *self, Actor *actor, ItemCategory category, ItemId_t id, ItemAmount_t amount);

/**
 * Free an allocated Item structure.
 * @param self A pointer to an allocated Item.
 */
void itemFree(Item *self);

/**
 * Free an allocated Item structure and nullify the content of the pointer.
 * @param self A pointer to an allocated Item.
 */
void itemDestroy(Item **self);

/**
 * Getters & Setters
 */
inline ItemId_t itemGetId(Item *self) { return self->id; }
inline ItemAmount_t itemGetAmount(Item *self) { return self->amount; }
inline ItemCategory itemGetCategory(Item *self) { return self->category; }

/**
 * Serialization / Unserialization
 */
size_t itemGetPropertiesCPacketSize(Item *self);
void itemPropertiesGetCPacket(Item *self, PacketStream *stream);

/**
 * Debugging
 */
void itemPrint(Item *self);
void itemChildPrint(Item *self);
