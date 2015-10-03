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


// ------ Structure declaration -------
/**
 * @brief ItemArmor contains information about an armor item
 *        Inherits from ItemEquipable
 */
typedef struct
{
    ItemEquipable equipable;

    float def;
    float cooldown;
    float reinforce;
    char *memo;
    char *customName;
    char *maker;

}   ItemArmor;

enum {
    ITEM_ARMOR_PROPERTY_ID_DEF = 3792,
    ITEM_ARMOR_PROPERTY_ID_COOLDOWN = 3843,
    ITEM_ARMOR_PROPERTY_ID_REINFORCE = 3852,
    ITEM_ARMOR_PROPERTY_ID_MEMO = 3972,
    ITEM_ARMOR_PROPERTY_ID_CUSTOM_NAME = 3975,
    ITEM_ARMOR_PROPERTY_ID_MAKER = 3978
};

// ----------- Functions ------------

/**
 * Allocate a new ItemArmor structure.
 * @return A pointer to an allocated ItemArmor, or NULL if an error occured.
 */
ItemArmor *itemArmorNew(
    ItemEquipable *equipable,
    float def,
    float cooldown,
    float reinforce,
    char *memo,
    char *customName,
    char *maker
);

/**
 * Initialize an allocated ItemArmor structure.
 * @param self An allocated ItemArmor to initialize.
 * @return true on success, false otherwise.
 */
bool itemArmorInit(
    ItemArmor *self,
    ItemEquipable *equipable,
    float def,
    float cooldown,
    float reinforce,
    char *memo,
    char *customName,
    char *maker);

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
 * Getters & Setters
 */
inline float itemArmorGetDef(ItemArmor *self) { return self->def; }
inline float itemArmorGetCooldown(ItemArmor *self) { return self->cooldown; }
inline float itemArmorGetReinforce(ItemArmor *self) { return self->reinforce; }
inline char *itemArmorGetMemo(ItemArmor *self) { return self->memo; }
inline char *itemArmorGetCustomName(ItemArmor *self) { return self->customName; }
inline char *itemArmorGetMaker(ItemArmor *self) { return self->maker; }

/**
 * Serialization / Unserialization
 */
size_t itemArmorGetPropertiesCPacketSize(ItemArmor *self);
void itemArmorGetPropertiesCPacket(ItemArmor *self, PacketStream *stream);
