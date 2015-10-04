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

#include "item_subweapon.h"

extern inline float *itemSubWeaponGetPetPosition(ItemSubWeapon *self);
extern inline float *itemSubWeaponGetCooldown(ItemSubWeapon *self);

ItemSubWeapon *itemSubWeaponNew(void) {
    ItemSubWeapon *self;

    if ((self = malloc(sizeof(ItemSubWeapon))) == NULL) {
        return NULL;
    }

    if (!itemSubWeaponInit(self)) {
        itemSubWeaponDestroy(&self);
        error("ItemSubWeapon failed to initialize.");
        return NULL;
    }

    return self;
}

bool itemSubWeaponInit(ItemSubWeapon *self) {
    memset(self, 0, sizeof(*self));

    return true;
}

void itemSubWeaponFree(ItemSubWeapon *self) {
    // TODO
}

void itemSubWeaponDestroy(ItemSubWeapon **_self) {
    ItemSubWeapon *self = *_self;

    if (_self && self) {
        itemSubWeaponFree(self);
        free(self);
        *_self = NULL;
    }
}


size_t itemSubWeaponGetPropertiesCPacketSize(ItemSubWeapon *self) {
    size_t size = 0;

    size += propertyFloatGetCPacketSize(); // petPosition
    size += propertyFloatGetCPacketSize(); // cooldown

    return size;
}

void itemSubWeaponGetPropertiesCPacket(ItemSubWeapon *self, PacketStream *stream) {
    propertyFloatGetCPacket(ITEM_SUBWEAPON_PROPERTY_ID_PET_POSITION, self->petPosition, stream);
    propertyFloatGetCPacket(ITEM_SUBWEAPON_PROPERTY_ID_COOLDOWN, self->cooldown, stream);
}
