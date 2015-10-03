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

ItemArmor *itemArmorNew(
    ItemEquipable *equipable,
    float def,
    float cooldown,
    float reinforce,
    char *memo,
    char *customName,
    char *maker)
{
    ItemArmor *self;

    if ((self = malloc(sizeof(ItemArmor))) == NULL) {
        return NULL;
    }

    if (!itemArmorInit(self, equipable, def, cooldown, reinforce, memo, customName, maker)) {
        itemArmorDestroy(&self);
        error("ItemArmor failed to initialize.");
        return NULL;
    }

    return self;
}

bool itemArmorInit(
    ItemArmor *self,
    ItemEquipable *equipable,
    float def,
    float cooldown,
    float reinforce,
    char *memo,
    char *customName,
    char *maker)
{
    memset(self, 0, sizeof(*self));

    memcpy(&self->equipable, equipable, sizeof(self->equipable));
    self->def = floatdup(def);
    self->cooldown = floatdup(cooldown);
    self->reinforce = floatdup(reinforce);
    self->memo = strdup(memo);
    self->customName = strdup(customName);
    self->maker = strdup(maker);

    return true;
}

void itemArmorFree(ItemArmor *self) {
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

size_t itemArmorGetPropertiesCPacketSize(ItemArmor *self) {
    size_t size = 0;

    size += propertyFloatGetCPacketSize(); // def
    size += propertyFloatGetCPacketSize(); // cooldown
    size += propertyFloatGetCPacketSize(); // reinforce
    size += propertyStringGetCPacketSize(self->memo); // memo
    size += propertyStringGetCPacketSize(self->customName); // customName
    size += propertyStringGetCPacketSize(self->maker); // maker

    return size;
}

void itemArmorGetPropertiesCPacket(ItemArmor *self, PacketStream *stream) {
    propertyFloatGetCPacket(ITEM_ARMOR_PROPERTY_ID_DEF, self->def, stream);
    propertyFloatGetCPacket(ITEM_ARMOR_PROPERTY_ID_COOLDOWN, self->cooldown, stream);
    propertyFloatGetCPacket(ITEM_ARMOR_PROPERTY_ID_REINFORCE, self->reinforce, stream);
    propertyStringGetCPacket(ITEM_ARMOR_PROPERTY_ID_MEMO, self->memo, stream);
    propertyStringGetCPacket(ITEM_ARMOR_PROPERTY_ID_CUSTOM_NAME, self->customName, stream);
    propertyStringGetCPacket(ITEM_ARMOR_PROPERTY_ID_MAKER, self->maker, stream);
}
