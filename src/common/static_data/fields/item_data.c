/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @brief
 *
 *
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

// ---------- Includes ------------
#include "item_data.h"

/**
 * @brief
 */
struct ItemData {
};

ItemData *itemDataNew(void) {

    ItemData *self;

    if ((self = malloc(sizeof(ItemData))) == NULL) {
        return NULL;
    }

    if (!itemDataInit(self)) {
        itemDataDestroy(&self);
        error("ItemData failed to initialize.");
        return NULL;
    }

    return self;
}

bool itemDataInit(ItemData *self) {

    memset(self, 0, sizeof(ItemData));

    return true;
}

void itemDataPrint(ItemData *self) {
    dbg("==== ItemData %p ====", self);
}

void itemDataFree(ItemData *self) {
}

void itemDataDestroy(ItemData **_self) {
    ItemData *self = *_self;

    if (_self && self) {
        itemDataFree(self);
        free(self);
        *_self = NULL;
    }
}
