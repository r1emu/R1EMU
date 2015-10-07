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

#include "item_common_data.h"

ItemCommonData *itemCommonDataNew(MYSQL_ROW row) {
    ItemCommonData *self;

    if ((self = malloc(sizeof(ItemCommonData))) == NULL) {
        return NULL;
    }

    if (!itemCommonDataInit(self, row)) {
        itemCommonDataDestroy(&self);
        error("ItemCommonData failed to initialize.");
        return NULL;
    }

    return self;
}

bool itemCommonDataInit(ItemCommonData *self, MYSQL_ROW row) {
    memset(self, 0, sizeof(*self));

    int index = 0;

    self->ClassID = strtol(row[index++], NULL, 10);
    self->ItemType = strdup(row[index++]);

    return true;
}

void itemCommonDataFree(ItemCommonData *self) {
    free(self->ItemType);
}

void itemCommonDataPrint(ItemCommonData *self) {
    dbg("==== ItemEquipData %p ====", self);
    dbg("ClassID = %d", self->ClassID);
    dbg("ItemType = %s", self->ItemType);
}

void itemCommonDataDestroy(ItemCommonData **_self) {
    ItemCommonData *self = *_self;

    if (_self && self) {
        itemCommonDataFree(self);
        free(self);
        *_self = NULL;
    }
}
