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
#include "common/static_data/fields/item_common_data.h"
#include "common/static_data/fields/item_equip_data.h"

// ---------- Defines -------------


// ------ Structure declaration -------
/** Item fields packet size */
typedef uint32_t ItemId_t;
typedef uint32_t ItemAmount_t;
typedef uint32_t ItemPrice_t;
typedef uint32_t ItemInventoryIndex_t;

/** Item categories */
typedef uint8_t ItemCategory_t;
enum {
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
    // ITEM_CAT_PETWEAPON = 13,
    // ITEM_CAT_PETARMOR = 14,
    ITEM_CAT_CURRENCY = 15,
    ITEM_CAT_COUNT,
};

/**
 * @brief Item is the server structure of an item
 */
typedef struct {
    Actor actor;
    ItemId_t id;
    ItemAmount_t amount;
    ItemCategory_t category;
    ItemCommonData *commonData;
} Item;
typedef Item* ItemHandle;

/**
 * @brief ItemCPacket is the client packet structure of an item
 */
#pragma pack(push, 1)
typedef struct {
    ItemId_t id;
    ItemAmount_t amount;
    ItemCategory_t category;
} ItemSPacket;
#pragma pack(pop)

// ----------- Functions ------------
/**
 * Allocate a new Item structure.
 * @return A pointer to an allocated Item, or NULL if an error occured.
 */
Item *itemNew(
    Actor *actor,
    ItemCategory_t category,
    ItemId_t id,
    ItemAmount_t amount,
    ItemCommonData *commonData,
    ItemEquipData *equipData);

/**
 * Initialize an allocated Item structure.
 * @param self An allocated Item to initialize.
 * @return true on success, false otherwise.
 */
bool itemInit(
    Item *self,
    Actor *actor,
    ItemCategory_t category,
    ItemId_t id,
    ItemAmount_t amount,
    ItemCommonData *commonData,
    ItemEquipData *equipData);

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
 * @brief Get the real size of an Item (including the subclass Item size)
 */
bool itemGetSize(ItemCategory_t category, size_t *size);

/**
 * Getters & Setters
 */
inline ItemId_t itemGetId(Item *self) { return self->id; }
inline ItemAmount_t itemGetAmount(Item *self) { return self->amount; }
inline ItemCategory_t itemGetCategory(Item *self) { return self->category; }
inline void itemSetCommonData(Item *self, ItemCommonData *data) { self->commonData = data; }

/**
 * Serialization / Unserialization
 */
size_t itemGetCPacketSize(Item *self);
void itemSerializeCPacket(Item *self, PacketStream *stream);

size_t itemGetSPacketSize(Item *self);
void itemSerializeSPacket(Item *self, PacketStream *stream);
bool itemUnserializeSPacket(Item **_self, PacketStream *stream);

size_t itemChildGetSPacketSize(Item *self);
void itemChildSerializeSPacket(Item *self, PacketStream *stream);
bool itemChildUnserializeSPacket(Item *self, PacketStream *stream);

/**
 * Debugging
 */
void itemPrint(Item *self);
void itemChildPrint(Item *self);


ItemHandle *itemHandleNew(Item *item);
void itemHandleFree(ItemHandle *self);
void itemHandleDestroy(ItemHandle **_self);
