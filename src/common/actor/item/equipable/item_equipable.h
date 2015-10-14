/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file item_equipable.h
 * @brief
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#pragma once

// ---------- Includes -------------
#include "R1EMU.h"
#include "common/actor/item/item.h"
#include "common/static_data/fields/item_equip_data.h"

// ---------- Defines -------------

// ------ Structure declaration -------
typedef uint8_t ItemEquipmentSlot_t;
enum {
    EQSLOT_HAT = 0,
    EQSLOT_HAT_L,
    EQSLOT_UNKNOWN1,
    EQSLOT_BODY_ARMOR,
    EQSLOT_GLOVES,
    EQSLOT_BOOTS,
    EQSLOT_HELMET,
    EQSLOT_BRACELET,
    EQSLOT_WEAPON,
    EQSLOT_SHIELD,
    EQSLOT_COSTUME,
    EQSLOT_UNKNOWN3,
    EQSLOT_UNKNOWN4,
    EQSLOT_UNKNOWN5,
    EQSLOT_LEG_ARMOR,
    EQSLOT_UNKNOWN6,
    EQSLOT_UNKNOWN7,
    EQSLOT_RING_LEFT,
    EQSLOT_RING_RIGHT,
    EQSLOT_NECKLACE,
    EQSLOT_COUNT,
    EQSLOT_NOSLOT,
};

typedef struct ItemEquipable {
   Item item;
   ItemEquipmentSlot_t slot;
   ItemEquipData *equipData;
} ItemEquipable;

#pragma pack(push, 1)
typedef struct {
    ItemEquipmentSlot_t slot;
} ItemEquipableSPacket;
#pragma pack(pop)

// ----------- Functions ------------
/**
 * Allocate a new ItemEquipable structure.
 * @return A pointer to an allocated ItemEquipable, or NULL if an error occured.
 */
ItemEquipable *itemEquipableNew(Item *item, ItemEquipData *data);

/**
 * Initialize an allocated ItemEquipable structure.
 * @param self An allocated ItemEquipable to initialize.
 * @return true on success, false otherwise.
 */
bool itemEquipableInit(ItemEquipable *self, Item *item, ItemEquipData *data);

/**
 * Free an allocated ItemEquipable structure.
 * @param self A pointer to an allocated ItemEquipable.
 */
void itemEquipableFree(ItemEquipable *self);

/**
 * Free an allocated ItemEquipable structure and nullify the content of the pointer.
 * @param self A pointer to an allocated ItemEquipable.
 */
void itemEquipableDestroy(ItemEquipable **self);

/**
 * Getters & Setters
 */
inline int itemEquipableGetSlot(ItemEquipable *self) { return self->slot; }
inline void itemEquipableSetEquipData(ItemEquipable *self, ItemEquipData *data) { self->equipData = data; }

/**
 * Serialization / Unserialization
 */
size_t itemEquipableGetSPacketSize(ItemEquipable *self);
void itemEquipableSerializeSPacket(ItemEquipable *self, PacketStream *stream);
bool itemEquipableUnserializeSPacket(ItemEquipable *self, PacketStream *stream);

/**
 * Debugging
 */
void itemEquipablePrint(ItemEquipable *self);
