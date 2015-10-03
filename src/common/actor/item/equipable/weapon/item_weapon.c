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

/**
 * @brief ItemWeapon contains
 */
struct ItemWeapon
{

};

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
