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

#include "item_armor.h"
extern inline float *itemArmorGetDef(ItemArmor *self);
extern inline float *itemArmorGetCooldown(ItemArmor *self);
extern inline float *itemArmorGetReinforce(ItemArmor *self);
extern inline char *itemArmorGetMemo(ItemArmor *self);
extern inline char *itemArmorGetCustomName(ItemArmor *self);
extern inline char *itemArmorGetMaker(ItemArmor *self);

ItemArmorId itemArmorIds[] = {
    FOREACH_ITEM_ARMOR(GENERATE_PROPERTY_ASSOC)
};

ItemArmor *itemArmorNew(Item *item)
{
    ItemArmor *self;

    if ((self = malloc(sizeof(ItemArmor))) == NULL) {
        return NULL;
    }

    if (!itemArmorInit(self, item)) {
        itemArmorDestroy(&self);
        error("ItemArmor failed to initialize.");
        return NULL;
    }

    return self;
}

bool itemArmorInit(ItemArmor *self, Item *item) {
    memset(self, 0, sizeof(*self));

    if (!(itemEquipableInit(&self->equipable, item))) {
        error("Cannot initialize an equipable item.");
        return false;
    }

    return true;
}

void itemArmorFree(ItemArmor *self) {
    itemEquipableFree(&self->equipable);
    free(self->def);
    free(self->cooldown);
    free(self->reinforce);
    free(self->memo);
    free(self->customName);
    free(self->maker);
}

void itemArmorDestroy(ItemArmor **_self) {
    ItemArmor *self = *_self;

    if (_self && self) {
        itemArmorFree(self);
        free(self);
        *_self = NULL;
    }
}

void itemArmorPrint(ItemArmor *self) {
    PRINT_STRUCTURE {
        dbg("=== ItemArmor %p ===", self);

        if (self->def) {
            dbg("def = %f", *self->def);
        }
        if (self->cooldown) {
            dbg("cooldown = %f", *self->cooldown);
        }
        if (self->reinforce) {
            dbg("reinforce = %f", *self->reinforce);
        }
        if (self->memo) {
            dbg("memo = %s", self->memo);
        }
        if (self->customName) {
            dbg("customName = %s", self->customName);
        }
        if (self->maker) {
            dbg("maker = %s", self->maker);
        }

        itemEquipablePrint(&self->equipable);
    }
}

size_t itemArmorGetCPacketSize(ItemArmor *self) {
    size_t size = 0;
    size += propertyFloatGetCPacketSize(self->def);
    size += propertyFloatGetCPacketSize(self->cooldown);
    size += propertyFloatGetCPacketSize(self->reinforce);
    size += propertyStringGetCPacketSize(self->memo);
    size += propertyStringGetCPacketSize(self->customName);
    size += propertyStringGetCPacketSize(self->maker);
    return size;
}

size_t itemArmorGetSPacketSize(ItemArmor *self) {
    size_t size = 0;
    size += itemEquipableGetSPacketSize(&self->equipable);
    size += propertyFloatGetSPacketSize(self->def);
    size += propertyFloatGetSPacketSize(self->cooldown);
    size += propertyFloatGetSPacketSize(self->reinforce);
    size += propertyStringGetSPacketSize(self->memo);
    size += propertyStringGetSPacketSize(self->customName);
    size += propertyStringGetSPacketSize(self->maker);
    return size;
}

void itemArmorSerializeCPacket(ItemArmor *self, PacketStream *stream) {
    propertyFloatSerializeCPacket(ITEM_ARMOR_PROPERTY_ID_DEF, self->def, stream);
    propertyFloatSerializeCPacket(ITEM_ARMOR_PROPERTY_ID_COOLDOWN, self->cooldown, stream);
    propertyFloatSerializeCPacket(ITEM_ARMOR_PROPERTY_ID_REINFORCE, self->reinforce, stream);
    propertyStringSerializeCPacket(ITEM_ARMOR_PROPERTY_ID_MEMO, self->memo, stream);
    propertyStringSerializeCPacket(ITEM_ARMOR_PROPERTY_ID_CUSTOM_NAME, self->customName, stream);
    propertyStringSerializeCPacket(ITEM_ARMOR_PROPERTY_ID_MAKER, self->maker, stream);
}

void itemArmorSerializeSPacket(ItemArmor *self, PacketStream *stream) {
    itemEquipableSerializeSPacket(&self->equipable, stream);
    propertyFloatSerializeSPacket(ITEM_ARMOR_PROPERTY_ID_DEF, self->def, stream);
    propertyFloatSerializeSPacket(ITEM_ARMOR_PROPERTY_ID_COOLDOWN, self->cooldown, stream);
    propertyFloatSerializeSPacket(ITEM_ARMOR_PROPERTY_ID_REINFORCE, self->reinforce, stream);
    propertyStringSerializeSPacket(ITEM_ARMOR_PROPERTY_ID_MEMO, self->memo, stream);
    propertyStringSerializeSPacket(ITEM_ARMOR_PROPERTY_ID_CUSTOM_NAME, self->customName, stream);
    propertyStringSerializeSPacket(ITEM_ARMOR_PROPERTY_ID_MAKER, self->maker, stream);
}

bool itemArmorUnserializeSPacket(ItemArmor *self, PacketStream *stream) {

    if (!(itemEquipableUnserializeSPacket(&self->equipable, stream))) {
        error("Cannot unserialize equipable packet.");
        return false;
    }
    if (!(propertyFloatUnserializeSPacket(ITEM_ARMOR_PROPERTY_ID_DEF, &self->def, stream))) {
        error("Cannot unserialize packet DEF.");
        return false;
    }
    if (!(propertyFloatUnserializeSPacket(ITEM_ARMOR_PROPERTY_ID_COOLDOWN, &self->cooldown, stream))) {
        error("Cannot unserialize packet COOLDOWN.");
        return false;
    }
    if (!(propertyFloatUnserializeSPacket(ITEM_ARMOR_PROPERTY_ID_REINFORCE, &self->reinforce, stream))) {
        error("Cannot unserialize packet REINFORCE.");
        return false;
    }
    if (!(propertyStringUnserializeSPacket(ITEM_ARMOR_PROPERTY_ID_MEMO, &self->memo, stream))) {
        error("Cannot unserialize packet MEMO.");
        return false;
    }
    if (!(propertyStringUnserializeSPacket(ITEM_ARMOR_PROPERTY_ID_CUSTOM_NAME, &self->customName, stream))) {
        error("Cannot unserialize packet CUSTOM NAME.");
        return false;
    }
    if (!(propertyStringUnserializeSPacket(ITEM_ARMOR_PROPERTY_ID_MAKER, &self->maker, stream))) {
        error("Cannot unserialize packet MAKER.");
        return false;
    }

    return true;
}
