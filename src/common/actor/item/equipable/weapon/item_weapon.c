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

ItemWeaponId itemWeaponIds[] = {
    FOREACH_ITEM_WEAPON(GENERATE_PROPERTY_ASSOC)
};

ItemWeapon *itemWeaponNew(Item *item, ItemEquipData *data) {
    ItemWeapon *self;

    if ((self = malloc(sizeof(ItemWeapon))) == NULL) {
        return NULL;
    }

    if (!itemWeaponInit(self, item, data)) {
        itemWeaponDestroy(&self);
        error("ItemWeapon failed to initialize.");
        return NULL;
    }

    return self;
}

bool itemWeaponInit(ItemWeapon *self, Item *item, ItemEquipData *data) {
    memset(self, 0, sizeof(*self));

    if (!(itemEquipableInit(&self->equipable, item, data))) {
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

size_t itemWeaponGetCPacketSize(ItemWeapon *self) {
    size_t size = 0;
    size += propertyFloatGetCPacketSize(self->maxAtk);
    size += propertyFloatGetCPacketSize(self->minAtk);
    size += propertyFloatGetCPacketSize(self->cooldown);
    return size;
}

size_t itemWeaponGetSPacketSize(ItemWeapon *self) {
    size_t size = 0;
    size += sizeof(ItemWeaponSPacket);
    size += propertyFloatGetSPacketSize(self->maxAtk);
    size += propertyFloatGetSPacketSize(self->minAtk);
    size += propertyFloatGetSPacketSize(self->cooldown);
    return size;
}

void itemWeaponSerializeCPacket(ItemWeapon *self, PacketStream *stream) {
    propertyFloatSerializeCPacket(ITEM_WEAPON_PROPERTY_ID_MAXATK, self->maxAtk, stream);
    propertyFloatSerializeCPacket(ITEM_WEAPON_PROPERTY_ID_MINATK, self->minAtk, stream);
    propertyFloatSerializeCPacket(ITEM_WEAPON_PROPERTY_ID_COOLDOWN, self->cooldown, stream);
}

void itemWeaponSerializeSPacket(ItemWeapon *self, PacketStream *stream) {
    propertyFloatSerializeSPacket(ITEM_WEAPON_PROPERTY_ID_MAXATK, self->maxAtk, stream);
    propertyFloatSerializeSPacket(ITEM_WEAPON_PROPERTY_ID_MINATK, self->minAtk, stream);
    propertyFloatSerializeSPacket(ITEM_WEAPON_PROPERTY_ID_COOLDOWN, self->cooldown, stream);
}

bool itemWeaponUnserializeSPacket(ItemWeapon *self, PacketStream *stream) {

    if (!(propertyFloatUnserializeSPacket(ITEM_WEAPON_PROPERTY_ID_MAXATK, &self->maxAtk, stream))) {
        error("Cannot unserialize packet PET_POSITION.");
        return false;
    }
    if (!(propertyFloatUnserializeSPacket(ITEM_WEAPON_PROPERTY_ID_MINATK, &self->minAtk, stream))) {
        error("Cannot unserialize packet COOLDOWN.");
        return false;
    }
    if (!(propertyFloatUnserializeSPacket(ITEM_WEAPON_PROPERTY_ID_COOLDOWN, &self->cooldown, stream))) {
        error("Cannot unserialize packet COOLDOWN.");
        return false;
    }

    return true;
}
