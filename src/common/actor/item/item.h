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
#include "common/actor/actor.h"
#include "common/packet/packet_stream.h"

// ---------- Defines -------------


// ------ Structure declaration -------
typedef uint32_t ItemId_t;
typedef uint32_t ItemAmount_t;
typedef uint32_t ItemInventoryIndex_t;
typedef uint32_t ItemPrice_t;

typedef enum {
    ITEM_CAT_WEAPON = 1,
    ITEM_CAT_ARMOR = 2,
    ITEM_CAT_SUBWEAPON = 3,
    ITEM_CAT_COSTUME = 4,
    ITEM_CAT_ACCESSORY = 5,
    ITEM_CAT_CONSUMABLE = 6,
    ITEM_CAT_GEM = 7,
    ITEM_CAT_MATERIAL = 8,
    ITEM_CAT_CARD = 9,
    ITEM_CAT_COLLECTION = 10,
    ITEM_CAT_BOOK = 11,
    ITEM_CAT_QUEST = 12,
    ITEM_CAT_PETWEAPON = 13,
    ITEM_CAT_PETARMOR = 14,
    ITEM_CAT_COUNT,
}   ItemCategory;

/**
 * @brief Item is the server structure of item attributes
 */
typedef struct ItemAttributes {


}   ItemAttributes;

/**
 * @brief Item is the server structure of an item
 */
typedef struct Item {
   Actor actor;
   ItemId_t id;
   ItemAmount_t amount;
   ItemCategory category;
   ItemAttributes attributes;
   ItemInventoryIndex_t inventoryIndex;
} Item;

/**
 * @brief ItemCPacket is the client packet structure of an item
 */
typedef struct ItemCPacket {
    ActorId_t uid;
    ItemId_t id;
    ItemAmount_t amount;
    ItemInventoryIndex_t inventoryIndex;
} ItemCPacket;

#define DEFINE_EquippedItemCPacket(attrSize) \
typedef struct EquippedItemCPacket {         \
    ItemId_t id;                             \
    uint16_t sizeOfAttributes;               \
    uint16_t unk1;                           \
    ActorId_t uid;                           \
    uint8_t eqSlotIndex;                     \
    uint8_t unk2;                            \
    uint16_t unk3;                           \
    uint32_t unk4;                           \
    uint8_t attributes[attrSize];            \
}   EquippedItemCPacket;

// ----------- Functions ------------

/**
 * Allocate a new Item structure.
 * @return A pointer to an allocated Item, or NULL if an error occured.
 */
Item *itemNew(void);

/**
 * Initialize an allocated Item structure.
 * @param self An allocated Item to initialize.
 * @return true on success, false otherwise.
 */
bool itemInit(Item *self);

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
inline ActorId_t itemGetUId(Item *self) {
    return actorGetUId(&self->actor);
}

inline ItemId_t itemGetId(Item *self) {
    return self->id;
}

inline ItemInventoryIndex_t itemGetInventoryIndex(Item *self) {
    return self->inventoryIndex;
}

inline ItemAmount_t itemGetAmount(Item *self) {
    return self->amount;
}

inline ItemCategory itemGetCategory(Item *self) {
    return self->category;
}


/**
 * Serialization / Unserialization
 */
size_t itemGetAttributesCPacketSize(Item *self);
void itemAttributesGetCPacket(Item *self, PacketStream *stream);
