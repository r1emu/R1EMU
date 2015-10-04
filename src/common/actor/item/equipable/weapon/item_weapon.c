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

ItemWeapon *itemWeaponNew(void) {
    ItemWeapon *self;

    if ((self = malloc(sizeof(ItemWeapon))) == NULL) {
        return NULL;
    }

    if (!itemWeaponInit(self)) {
        itemWeaponDestroy(&self);
        error("ItemWeapon failed to initialize.");
        return NULL;
    }

    return self;
}

bool itemWeaponInit(ItemWeapon *self) {
    memset(self, 0, sizeof(*self));

    return true;
}

void itemWeaponFree(ItemWeapon *self) {
    // TODO
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
