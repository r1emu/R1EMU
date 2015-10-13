/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file item_armor.h
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
#include "common/actor/item/equipable/item_equipable.h"

// ---------- Defines -------------
// Define properties ID
#define FOREACH_ITEM_ARMOR(GENERATOR)        \
    GENERATOR(ITEM_ARMOR, DEF, 3792)         \
    GENERATOR(ITEM_ARMOR, COOLDOWN, 3843)    \
    GENERATOR(ITEM_ARMOR, REINFORCE, 3852)   \
    GENERATOR(ITEM_ARMOR, MEMO, 3972)        \
    GENERATOR(ITEM_ARMOR, CUSTOM_NAME, 3975) \
    GENERATOR(ITEM_ARMOR, MAKER, 3978)

typedef enum {
    FOREACH_ITEM_ARMOR(GENERATE_PROPERTY_ENUM)
    ITEM_ARMOR_PROPERTY_COUNT
}   ItemArmorProperty;

typedef enum {
    FOREACH_ITEM_ARMOR(GENERATE_PROPERTY_ID)
}   ItemArmorId;


// ------ Structure declaration -------
/**
 * @brief ItemArmor contains information about an armor item
 *        Inherits from ItemEquipable
 */
typedef struct
{
    ItemEquipable equipable;

    float *def;
    float *cooldown;
    float *reinforce;
    char *memo;
    char *customName;
    char *maker;

}   ItemArmor;

typedef struct
{
    PropertyFloatSPacket def[0];
    PropertyFloatSPacket cooldown[0];
    PropertyFloatSPacket reinforce[0];
    PropertyStringSPacket memo[0];
    PropertyStringSPacket customName[0];
    PropertyStringSPacket maker[0];
}   ItemArmorSPacket;

// ----------- Functions ------------

/**
 * Allocate a new ItemArmor structure.
 * @return A pointer to an allocated ItemArmor, or NULL if an error occured.
 */
ItemArmor *itemArmorNew(Item *item, ItemEquipData *data);

/**
 * Initialize an allocated ItemArmor structure.
 * @param self An allocated ItemArmor to initialize.
 * @return true on success, false otherwise.
 */
bool itemArmorInit(ItemArmor *self, Item *item, ItemEquipData *data);

/**
 * Free an allocated ItemArmor structure.
 * @param self A pointer to an allocated ItemArmor.
 */
void itemArmorFree(ItemArmor *self);

/**
 * Free an allocated ItemArmor structure and nullify the content of the pointer.
 * @param self A pointer to an allocated ItemArmor.
 */
void itemArmorDestroy(ItemArmor **self);

/**
 * Serialization / Unserialization
 */
size_t itemArmorGetCPacketSize(ItemArmor *self);
size_t itemArmorGetSPacketSize(ItemArmor *self);
void itemArmorSerializeCPacket(ItemArmor *self, PacketStream *stream);
void itemArmorSerializeSPacket(ItemArmor *self, PacketStream *stream);
bool itemArmorUnserializeSPacket(ItemArmor *self, PacketStream *stream);

/**
 * Getters and Setters
 */
inline float *itemArmorGetDef(ItemArmor *self) { return self->def; }
inline float *itemArmorGetCooldown(ItemArmor *self) { return self->cooldown; }
inline float *itemArmorGetReinforce(ItemArmor *self) { return self->reinforce; }
inline char *itemArmorGetMemo(ItemArmor *self) { return self->memo; }
inline char *itemArmorGetCustomName(ItemArmor *self) { return self->customName; }
inline char *itemArmorGetMaker(ItemArmor *self) { return self->maker; }

/**
 * Debugging
 */
void itemArmorPrint(ItemArmor *self);
