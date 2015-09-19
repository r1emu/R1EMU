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

#include "db_object.h"

DbObject *dbObjectNew(size_t dataSize, void *data) {
    DbObject *self;

    if ((self = malloc(sizeof_struct_member(DbObject, dataSize) + dataSize)) == NULL) {
        return NULL;
    }

    if (!dbObjectInit(self, dataSize, data)) {
        dbObjectDestroy(&self);
        error("DbObject failed to initialize.");
        return NULL;
    }

    return self;
}

bool dbObjectInit(DbObject *self, size_t dataSize, void *data) {
    memset(self, 0, sizeof(DbObject));

    self->dataSize = dataSize;
    memcpy(self->data, data, dataSize);

    return true;
}


void dbObjectFree(DbObject *self) {
}

void dbObjectDestroy(DbObject **_self) {
    DbObject *self = *_self;

    if (_self && self) {
        dbObjectFree(self);
        free(self);
        *_self = NULL;
    }
}

