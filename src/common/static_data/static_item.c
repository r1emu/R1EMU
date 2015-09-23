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
#include "static_item.h"

#define STATIC_DATA_NAME "StaticItem"

/**
 * @brief
 */
struct StaticItem {
    StaticData *staticData;
};

StaticItem *staticItemNew(void) {

    StaticItem *self;

    if ((self = malloc(sizeof(StaticItem))) == NULL) {
        return NULL;
    }

    if (!staticItemInit(self)) {
        staticItemDestroy(&self);
        error("StaticItem failed to initialize.");
        return NULL;
    }

    return self;
}

bool staticItemInit(StaticItem *self) {

    memset(self, 0, sizeof(StaticItem));

    if (!(self->staticData = staticDataNew(STATIC_DATA_NAME))) {
        error("Cannot allocate '%s'", STATIC_DATA_NAME);
        return false;
    }

    return true;
}

bool staticItemAdd(StaticItem *self, StaticDataId id, ItemData *item) {

    if (!(staticDataAdd(self->staticData, id, item))) {
        error("Cannot add ItemData.");
        return false;
    }

    return true;
}

void staticItemPrint(StaticItem *self) {
    dbg("==== StaticItem %p ====", self);
}

void staticItemFree(StaticItem *self) {
}

void staticItemDestroy(StaticItem **_self) {
    StaticItem *self = *_self;

    if (_self && self) {
        staticItemFree(self);
        free(self);
        *_self = NULL;
    }
}
