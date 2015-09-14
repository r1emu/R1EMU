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

#include "db.h"

/**
 * @brief Db contains a hashtable of generic objects that can be inserted, requested and deleted.
 *        Db is extensible to other modules
 */
struct Db
{
    DbInfo info;

    zsock_t *endpoint;
    zhash_t *hashtable;
};

typedef struct _DbObject
{
    size_t dataSize;
    char data[0];
}   DbObject;

static DbObject *dbObjectNew(size_t dataSize, void *data);
static bool dbObjectInit(DbObject *self, size_t dataSize, void *data);
static void dbObjectFree(DbObject *self);
static void dbObjectDestroy(DbObject **_self);

Db *dbNew(DbInfo *dbInfo) {
    Db *self;

    if ((self = malloc(sizeof(Db))) == NULL) {
        return NULL;
    }

    if (!dbInit(self, dbInfo)) {
        dbDestroy(&self);
        error("Db failed to initialize.");
        return NULL;
    }

    return self;
}

bool dbInit(Db *self, DbInfo *dbInfo) {
    memset(self, 0, sizeof(Db));

    memcpy(&self->info, dbInfo, sizeof(self->info));

    if (!(self->hashtable = zhash_new())) {
        error("Cannot allocate a new object hashtable.");
        return false;
    }

    if (!(self->endpoint = zsock_new (ZMQ_REP))) {
        error("Cannot allocate a new endpoint.");
        return false;
    }

    return true;
}

DbInfo *dbInfoNew(
    uint16_t routerId,
    char *dbName,
    void *heritage,
    DbProcessMsgHandler handler)
{
    DbInfo *self;

    if ((self = malloc(sizeof(DbInfo))) == NULL) {
        return NULL;
    }

    if (!dbInfoInit(self, routerId, dbName, heritage, handler)) {
        dbInfoDestroy(&self);
        error("DbInfo failed to initialize.");
        return NULL;
    }

    return self;
}

bool dbInfoInit(
    DbInfo *self,
    uint16_t routerId,
    char *dbName,
    void *heritage,
    DbProcessMsgHandler handler)
{
    memset(self, 0, sizeof(DbInfo));

    self->routerId = routerId;
    self->name = strdup(dbName);
    self->heritage = heritage;
    self->handler = handler;

    return true;
}

static DbObject *dbObjectNew(size_t dataSize, void *data) {
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

static bool dbObjectInit(DbObject *self, size_t dataSize, void *data) {
    memset(self, 0, sizeof(DbObject));

    self->dataSize = dataSize;
    memcpy(self->data, data, dataSize);

    return true;
}

bool dbUpdateArray(Db *self, zmsg_t *msg, zmsg_t *out) {

    bool status = false;
    zframe_t *objectFrame = NULL;
    zframe_t *keyFrame = NULL;

    // header success
    if ((zmsg_addmem(out, PACKET_HEADER(DB_STATUS_SUCCESS), sizeof(DB_STATUS_SUCCESS))) != 0) {
        error("Cannot add the packet header to the msg.");
        goto cleanup;
    }

    // read object key
    while ((keyFrame = zmsg_pop(msg)) != NULL) {
        uint8_t *key = (uint8_t *) zframe_data(keyFrame);

        // each key is followed by its updated object
        if (!(objectFrame = zmsg_pop(msg))) {
            error("'%s:%d' : Cannot read the object '%s'.", self->info.name, self->info.routerId, key);
            goto cleanup;
        }

        // read object from frame
        void *data = zframe_data(objectFrame);
        size_t dataSize = zframe_size(objectFrame);
        DbObject *dbObject = NULL;

        if (!(dbObject = zhash_lookup(self->hashtable, key))) {

            // dbObject doesn't already exist : create it
            if (!(dbObject = dbObjectNew(dataSize, data))) {
                error("'%s:%d' : Cannot allocate a new object for '%s'.", self->info.name, self->info.routerId, key);
                goto cleanup;
            }

            // insert it
            if (zhash_insert(self->hashtable, key, dbObject) != 0) {
                error("'%s:%d' : Cannot insert the new object in '%s'.", self->info.name, self->info.routerId, key);
                goto cleanup;
            }
        } else {
            // update the data
            memcpy(dbObject->data, data, dataSize);
        }

        zframe_destroy(&keyFrame);
        zframe_destroy(&objectFrame);
    }

    status = true;

cleanup:
    if (!status) {
        zframe_t *frame = NULL;

        // cleanup the message frames
        while ((frame = zmsg_pop(out)) != NULL) {
            zframe_destroy(&frame);
        }

        // fill the msg with an error code
        if ((zmsg_addmem(out, PACKET_HEADER(DB_STATUS_CANNOT_UPDATE), sizeof(DB_STATUS_CANNOT_UPDATE))) != 0) {
            error("Cannot add the packet header to the msg.");
        }
    }

    zframe_destroy(&keyFrame);
    zframe_destroy(&objectFrame);
    return status;
}

bool dbRemoveArray(Db *self, zmsg_t *msg, zmsg_t *out) {

    bool status = false;
    zframe_t *keyFrame = NULL;

    // header success
    if ((zmsg_addmem(out, PACKET_HEADER(DB_STATUS_SUCCESS), sizeof(DB_STATUS_SUCCESS))) != 0) {
        error("Cannot add the packet header to the msg.");
        goto cleanup;
    }

    // read object key
    while ((keyFrame = zmsg_pop(msg)) != NULL) {
        uint8_t *key = (uint8_t *) zframe_data(keyFrame);

        DbObject *dbObject = NULL;

        if (!(dbObject = zhash_lookup(self->hashtable, key))) {
            error("%s:%d : Cannot remove item '%s', because it doesn't exist.",
                  self->info.name, self->info.routerId, key);
        }

        // delete value from hashtable
        zhash_delete(self->hashtable, key);

        // destroy the object
        dbObjectDestroy(&dbObject);

        // cleanup
        zframe_destroy(&keyFrame);
    }

    status = true;

cleanup:
    if (!status) {
        zframe_t *frame = NULL;

        // cleanup the message frames
        while ((frame = zmsg_pop(out)) != NULL) {
            zframe_destroy(&frame);
        }

        // fill the msg with an error code
        if ((zmsg_addmem(out, PACKET_HEADER(DB_STATUS_CANNOT_REMOVE), sizeof(DB_STATUS_CANNOT_REMOVE))) != 0) {
            error("Cannot add the packet header to the msg.");
        }
    }

    zframe_destroy(&keyFrame);
    return status;
}

bool dbGetArray(Db *self, zmsg_t *in, zmsg_t *out) {

    bool status = false;
    DbObject *dbObject = NULL;
    zframe_t *keyFrame = NULL;

    // header success
    if ((zmsg_addmem(out, PACKET_HEADER(DB_STATUS_SUCCESS), sizeof(DB_STATUS_SUCCESS))) != 0) {
        error("Cannot add the packet header to the msg.");
        goto cleanup;
    }

    // read object keys
    while ((keyFrame = zmsg_pop(in)) != NULL) {
        uint8_t *key = (uint8_t *) zframe_data(keyFrame);

        // lookup if the item exists
        if (!(dbObject = zhash_lookup(self->hashtable, key))) {
            error("'%s:%d' : Cannot find the object in '%s'.", self->info.name, self->info.routerId, key);
            goto cleanup;
        }

        // add to the key to the message
        if (zmsg_addstr(out, key) != 0) {
            error("'%s:%d' : Cannot add key '%s' to the message.", self->info.name, self->info.routerId, key);
            goto cleanup;
        }

        // add to the associated object to the message
        if (zmsg_addmem(out, dbObject->data, dbObject->dataSize) != 0) {
            error("'%s:%d' : Cannot add object '%s' to the message.", self->info.name, self->info.routerId, key);
            goto cleanup;
        }

        zframe_destroy(&keyFrame);
    }

    status = true;

cleanup:
    if (!status) {
        zframe_t *frame = NULL;

        // cleanup the message frames
        while ((frame = zmsg_pop(out)) != NULL) {
            zframe_destroy(&frame);
        }

        // fill the msg with an error code
        if ((zmsg_addmem(out, PACKET_HEADER(DB_STATUS_CANNOT_GET), sizeof(DB_STATUS_CANNOT_GET))) != 0) {
            error("Cannot add the packet header to the msg.");
        }
    }

    zframe_destroy(&keyFrame);
    return status;
}

void *dbMainLoop(void *arg) {
    Db *self = (Db *) arg;

    zmsg_t *msg = NULL;
    zframe_t *packetTypeFrame = NULL;

    while (true) {
        msg = zmsg_recv(self->endpoint);

        if (!msg) {
            error("%s:%d : Cannot receive a message.", self->info.name, self->info.routerId);
            break;
        }

        // read packet type
        if (!(packetTypeFrame = zmsg_pop(msg))) {
            error("%s:%d : Cannot read the packet type frame.", self->info.name, self->info.routerId);
            break;
        }
        DbPacketType packetType = *((typeof(packetType) *) zframe_data(packetTypeFrame));

        // create output message
        zmsg_t *out = NULL;
        if (!(out = zmsg_new())) {
            error("Cannot allocate output zmsg.");
            break;
        }

        // handle packet
        switch (packetType)
        {
            // update one object
            case DB_UPDATE_OBJECT:
            case DB_UPDATE_ARRAY:
                if (!(dbUpdateArray(self, msg, out))) {
                    error("%s:%d : Cannot insert new object", self->info.name, self->info.routerId);
                }
                break;

            // get array objects
            case DB_GET_OBJECT:
            case DB_GET_ARRAY:
                if (!(dbGetArray(self, msg, out))) {
                    error("%s:%d : Cannot get object", self->info.name, self->info.routerId);
                }
                break;

            // remove one object
            case DB_REMOVE_OBJECT:
            case DB_REMOVE_ARRAY:
                if (!(dbRemoveArray(self, msg, out))) {
                    error("%s:%d : Cannot remove object", self->info.name, self->info.routerId);
                }
                break;

            default:
                // extended messages
                if (!(self->info.handler) || !(self->info.handler(self->info.heritage, msg, out))) {
                    // message type unhandled
                    error("%s:%d : Cannot process db message type=%d.",
                          self->info.name, self->info.routerId, packetType);
                }
                break;
        }

        // send answer
        if (zmsg_send(&out, self->endpoint) != 0) {
            error("%s:%d : Cannot send back the answer.", self->info.name, self->info.routerId);
            break;
        }

        zframe_destroy(&packetTypeFrame);
        zmsg_destroy(&msg);
    }

    info("%s:%d : Stopped working.", self->info.name, self->info.routerId);
    return NULL;
}

bool dbStart(Db *self) {

    char *endpointStr = zsys_sprintf(DB_ENDPOINT, self->info.name, self->info.routerId);

    // bind connection
    if ((zsock_bind(self->endpoint, endpointStr)) != 0) {
        error("Cannot bind '%s'", endpointStr);
        return false;
    }

    info("%s binded.", endpointStr);

    // start actor thread
    if (zthread_new(dbMainLoop, self) != 0) {
        error("Cannot start a new session manager loop.");
        return false;
    }

    return true;
}

void dbFree(Db *self) {
    zhash_destroy(&self->hashtable);
}

void dbDestroy(Db **_self) {
    Db *self = *_self;

    if (_self && self) {
        dbFree(self);
        free(self);
        *_self = NULL;
    }
}

void dbInfoFree(DbInfo *self) {
    // TODO
}

void dbInfoDestroy(DbInfo **_self) {
    DbInfo *self = *_self;

    if (_self && self) {
        dbInfoFree(self);
        free(self);
        *_self = NULL;
    }
}

static void dbObjectFree(DbObject *self) {
    // TODO
}

static void dbObjectDestroy(DbObject **_self) {
    DbObject *self = *_self;

    if (_self && self) {
        dbObjectFree(self);
        free(self);
        *_self = NULL;
    }
}

