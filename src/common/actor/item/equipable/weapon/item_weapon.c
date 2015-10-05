/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#include "item_weapon.h"

extern inline float *itemWeaponGetMaxAtk(ItemWeapon *self);
extern inline float *itemWeaponGetMinAtk(ItemWeapon *self);
extern inline float *itemWeaponGetCooldown(ItemWeapon *self);

ItemWeapon *itemWeaponNew(Item *item) {
    ItemWeapon *self;

    if ((self = malloc(sizeof(ItemWeapon))) == NULL) {
        return NULL;
    }

    if (!itemWeaponInit(self, item)) {
        itemWeaponDestroy(&self);
        error("ItemWeapon failed to initialize.");
        return NULL;
    }

    return self;
}

bool itemWeaponInit(ItemWeapon *self, Item *item) {
    memset(self, 0, sizeof(*self));

    if (!(itemEquipableInit(&self->equipable, item))) {
        error("Cannot initialize an equipable item.");
        return false;
    }

    return true;
}

void itemWeaponFree(ItemWeapon *self) {
    itemEquipableFree(&self->equipable);
    free(self->maxAtk);
    free(self->minAtk);
    free(self->cooldown);
}

void itemWeaponDestroy(ItemWeapon **_self) {
    ItemWeapon *self = *_self;

    if (_self && self) {
        itemWeaponFree(self);
        free(self);
        *_self = NULL;
    }
}


size_t itemWeaponGetPropertiesCPacketSize(ItemWeapon *self) {
    size_t size = 0;

    size += propertyFloatGetCPacketSize(); // maxAtk
    size += propertyFloatGetCPacketSize(); // minAtk
    size += propertyFloatGetCPacketSize(); // cooldown

    return size;
}

void itemWeaponGetPropertiesCPacket(ItemWeapon *self, PacketStream *stream) {
    propertyFloatGetCPacket(ITEM_WEAPON_PROPERTY_ID_MAXATK, self->maxAtk, stream);
    propertyFloatGetCPacket(ITEM_WEAPON_PROPERTY_ID_MINATK, self->minAtk, stream);
    propertyFloatGetCPacket(ITEM_WEAPON_PROPERTY_ID_COOLDOWN, self->cooldown, stream);
}

void itemWeaponPrint(ItemWeapon *self) {
    PRINT_STRUCTURE {
        dbg("=== ItemWeapon %p ===", self);

        if (self->maxAtk) {
            dbg("maxAtk = %f", *self->maxAtk);
        }
        if (self->minAtk) {
            dbg("minAtk = %f", *self->minAtk);
        }
        if (self->cooldown) {
            dbg("cooldown = %f", *self->cooldown);
        }
        itemEquipablePrint(&self->equipable);
    }
}
