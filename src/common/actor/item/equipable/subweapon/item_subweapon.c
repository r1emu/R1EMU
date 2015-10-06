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

ItemSubWeapon *itemSubWeaponNew(Item *item) {
    ItemSubWeapon *self;

    if ((self = malloc(sizeof(ItemSubWeapon))) == NULL) {
        return NULL;
    }

    if (!itemSubWeaponInit(self, item)) {
        itemSubWeaponDestroy(&self);
        error("ItemSubWeapon failed to initialize.");
        return NULL;
    }

    return self;
}

bool itemSubWeaponInit(ItemSubWeapon *self, Item *item) {
    memset(self, 0, sizeof(*self));

    if (!(itemEquipableInit(&self->equipable, item))) {
        error("Cannot initialize an equipable item.");
        return false;
    }

    return true;
}

void itemSubWeaponFree(ItemSubWeapon *self) {
    itemEquipableFree(&self->equipable);
    free(self->petPosition);
    free(self->cooldown);
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

    size += propertyFloatGetCPacketSize(self->petPosition); // petPosition
    size += propertyFloatGetCPacketSize(self->cooldown); // cooldown

    return size;
}

void itemSubWeaponGetPropertiesCPacket(ItemSubWeapon *self, PacketStream *stream) {
    propertyFloatGetCPacket(ITEM_SUBWEAPON_PROPERTY_ID_PET_POSITION, self->petPosition, stream);
    propertyFloatGetCPacket(ITEM_SUBWEAPON_PROPERTY_ID_COOLDOWN, self->cooldown, stream);
}

void itemSubWeaponPrint(ItemSubWeapon *self) {
    PRINT_STRUCTURE {
        dbg("=== ItemSubWeapon %p ===", self);
        if (self->petPosition) {
            dbg("petPosition = %f", *self->petPosition);
        }
        if (self->cooldown) {
            dbg("cooldown = %f", *self->cooldown);
        }
        itemEquipablePrint(&self->equipable);
    }
}
