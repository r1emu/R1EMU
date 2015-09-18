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

DbObject *dbObjectNew(size_t dataSize, void *data, bool sharedMemory) {
    DbObject *self;

    if ((self = malloc(sizeof_struct_member(DbObject, dataSize) + dataSize)) == NULL) {
        return NULL;
    }

    if (!dbObjectInit(self, dataSize, data, sharedMemory)) {
        dbObjectDestroy(&self);
        error("DbObject failed to initialize.");
        return NULL;
    }

    return self;
}

bool dbObjectInit(DbObject *self, size_t dataSize, void *data, bool sharedMemory) {

    bool status = false;

    memset(self, 0, sizeof(DbObject));

    self->dataSize = dataSize;
    self->sharedMemory = sharedMemory;

    if (!sharedMemory) {
        // allocate memory for a copy
        if (!(self->data = malloc (dataSize))) {
            error("Cannot allocate data (size = %d)", dataSize);
            goto cleanup;
        }
        memcpy(self->data, data, dataSize);
    } else {
        // share memory
        self->data = data;
    }

    status = true;

cleanup:

    return status;
}


void dbObjectFree(DbObject *self) {
    if (!self->sharedMemory) {
        free(self->data);
    }
}

void dbObjectDestroy(DbObject **_self) {
    DbObject *self = *_self;

    if (_self && self) {
        dbObjectFree(self);
        free(self);
        *_self = NULL;
    }
}

