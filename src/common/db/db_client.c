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

#include "db_client.h"
#include "db.h"

/**
 * @brief DbClient contains connection to a Db server
 *        The methods in this module are used to contact the Db server
 */
struct DbClient
{
    DbClientInfo info;
    zsock_t *connection;
};

/**
 * Add keys to the request message
 */
static bool dbClientAddKeysToMessage(DbClient *self, zmsg_t *request, char **keys, size_t keysCount);

/**
 * Add key+object to the request message
 */
static bool dbClientAddObjectToMessage(DbClient *self, zmsg_t *request, char *key, DbObject *object);

DbClient *dbClientNew(DbClientInfo *startInfo) {
    DbClient *self;

    if ((self = malloc(sizeof(DbClient))) == NULL) {
        return NULL;
    }

    if (!dbClientInit(self, startInfo)) {
        dbClientDestroy(&self);
        error("DbClient failed to initialize.");
        return NULL;
    }

    return self;
}

bool dbClientInit(DbClient *self, DbClientInfo *startInfo) {
    memset(self, 0, sizeof(DbClient));

    memcpy(&self->info, startInfo, sizeof(self->info));

    if (!(self->connection = zsock_new(ZMQ_REQ))) {
        error("%s:%d : Cannot create connection socket.",
              self->info.name, self->info.routerId);
        return false;
    }

    return true;
}

DbClientInfo *dbClientInfoNew(char *name, uint16_t routerId) {
    DbClientInfo *self;

    if ((self = malloc(sizeof(DbClientInfo))) == NULL) {
        return NULL;
    }

    if (!dbClientInfoInit(self, name, routerId)) {
        dbClientInfoDestroy(&self);
        error("DbClientInfo failed to initialize.");
        return NULL;
    }

    return self;
}

bool dbClientInfoInit(DbClientInfo *self, char *name, uint16_t routerId) {
    memset(self, 0, sizeof(DbClientInfo));

    self->name = strdup(name);
    self->routerId = routerId;

    return true;
}

bool dbClientStart(DbClient *self) {

    char *endpointStr = zsys_sprintf(DB_ENDPOINT, self->info.name, self->info.routerId);

    if (zsock_connect(self->connection, endpointStr) != 0) {
        error("%s:%d : Cannot connect to %s.",
              self->info.name, self->info.routerId, endpointStr);
        return false;
    }

    info("%s:%d : %s connected.", self->info.name, self->info.routerId, endpointStr);

    return true;
}

static bool dbClientAddKeysToMessage(DbClient *self, zmsg_t *request, char **keys, size_t keysCount) {

    bool status = false;

    for (size_t keyIdx; keyIdx < keysCount; keyIdx++) {
        // add all the keys requested to the message
        if (zmsg_addstr(request, keys[keyIdx]) != 0) {
            error("%s:%d : Cannot add key to message.",
                  self->info.name, self->info.routerId);
            goto cleanup;
        }
    }

    status = true;

cleanup:
    return status;
}

static bool dbClientAddObjectToMessage(DbClient *self, zmsg_t *request, char *key, DbObject *object) {

    bool status = false;

    if (zmsg_addstr(request, key) != 0) {
        error("%s:%d : Cannot add key to message.", self->info.name, self->info.routerId);
        goto cleanup;
    }

    if (zmsg_addmem(request, object->data, object->dataSize) != 0) {
        error("%s:%d : Cannot add object to message.", self->info.name, self->info.routerId);
        goto cleanup;
    }

    status = true;

cleanup:
    return status;
}

bool dbClientRemoveValues(DbClient *self, char **keys, size_t keysCount) {

    bool status = false;
    zmsg_t *request = NULL;
    zmsg_t *response = NULL;
    zframe_t *responseStatusFrame = NULL;

    if (!(request = zmsg_new())) {
        error("%s:%d : Cannot allocate a new request msg.",
              self->info.name, self->info.routerId);
        goto cleanup;
    }

    // add the header
    if (zmsg_addmem(request, PACKET_HEADER(DB_REMOVE_ARRAY), sizeof(DB_REMOVE_ARRAY) != 0)){
        error("%s:%d : Cannot add DB_REMOVE_ARRAY to dbClient REMOVE message.",
              self->info.name, self->info.routerId);
        goto cleanup;
    }

    // add the keys to the message
    if (!(dbClientAddKeysToMessage(self, request, keys, keysCount))) {
        error("Cannot add keys to the request message.");
        goto cleanup;
    }

    // wait for db answer
    if (!(response = zmsg_recv(self->connection))) {
        error("%s:%d : Cannot receive a message.",
              self->info.name, self->info.routerId);
        goto cleanup;
    }

    // get header status
    if (!(responseStatusFrame = zmsg_pop(response))) {
        error("%s:%d : Cannot read result header.",
              self->info.name, self->info.routerId);
        goto cleanup;
    }

    // check header status
    DbStatus responseStatus = *((typeof(responseStatus) *) zframe_data(responseStatusFrame));
    if (responseStatus != DB_STATUS_SUCCESS) {
        error("%s:%d : Database could not REMOVE keys. Status = %d",
              self->info.name, self->info.routerId, responseStatus);
        goto cleanup;
    }

    status = true;

cleanup:
    zmsg_destroy(&request);
    zframe_destroy(&responseStatusFrame);

    return status;
}

bool dbClientRemoveValue(DbClient *self, char *key) {
    if (!(dbClientRemoveValues(self, (char *[]){key}, 1))) {
        error("Cannot remove value '%s'", key);
        return false;
    }

    return true;
}

bool dbClientRequestValues(DbClient *self, char **keys, size_t keysCount) {

    bool status = false;
    zmsg_t *request = NULL;

    if (!(request = zmsg_new())) {
        error("%s:%d : Cannot allocate a new request msg.",
              self->info.name, self->info.routerId);
        goto cleanup;
    }

    // add the header
    if (zmsg_addmem(request, PACKET_HEADER(DB_GET_ARRAY), sizeof(DB_GET_ARRAY) != 0)) {
        error("%s:%d : Cannot add DB_GET_ARRAY to dbClient GET message.",
              self->info.name, self->info.routerId);
        goto cleanup;
    }

    // add the keys to the message
    if (!(dbClientAddKeysToMessage(self, request, keys, keysCount))) {
        error("Cannot add keys to the request message.");
        goto cleanup;
    }

    // send the request
    if (zmsg_send(&request, self->connection) != 0) {
        error("Cannot send the message to the Db.");
        goto cleanup;
    }

    status = true;

cleanup:
    zmsg_destroy(&request);

    return status;
}

bool dbClientRequestValue(DbClient *self, char *key) {
    if (!(dbClientRequestValues(self, (char *[]){key}, 1))) {
        error("Cannot request value '%s'", key);
        return false;
    }

    return true;
}

bool dbClientGetValues(DbClient *self, zhash_t **_out) {

    bool status = false;
    zmsg_t *response = NULL;
    zframe_t *responseStatusFrame = NULL;
    zframe_t *valueFrame = NULL;
    zhash_t *out = *_out = NULL;

    // wait for db answer
    if (!(response = zmsg_recv(self->connection))) {
        error("%s:%d : Cannot receive a message.",
              self->info.name, self->info.routerId);
        goto cleanup;
    }

    // get header status
    if (!(responseStatusFrame = zmsg_pop(response))) {
        error("%s:%d : Cannot read result header.",
              self->info.name, self->info.routerId);
        goto cleanup;
    }

    // check header status
    DbStatus responseStatus = *((typeof(responseStatus) *) zframe_data(responseStatusFrame));
    if (responseStatus != DB_STATUS_SUCCESS) {
        error("%s:%d : Database could not GET key. Status = %d",
              self->info.name, self->info.routerId, responseStatus);
        goto cleanup;
    }

    // allocate result hashtable
    if (!(out = zhash_new())) {
        error("%s:%d : Cannot allocate a new values hashtable.",
              self->info.name, self->info.routerId);
        goto cleanup;
    }

    char *key;
    while ((key = zmsg_popstr(response)) != NULL) {

        // key is followed by the value associated to the key
        if (!(valueFrame = zmsg_pop(response))) {
            error("%s:%d : Cannot get the object associated to the key '%s'",
                  self->info.name, self->info.routerId, key);
            goto cleanup;
        }

        void *value = zframe_data(valueFrame);
        size_t valueSize = zframe_size(valueFrame);

        // Construct a dbObject from the frame
        DbObject *object = NULL;
        if (!(object = dbObjectNew(valueSize, value))) {
            error("%s:%d : Cannot create an object '%s' of size %d.",
                  self->info.name, self->info.routerId, key, valueSize);
            goto cleanup;
        }

        if (zhash_insert(out, key, object) != 0) {
            error("%s:%d : Cannot insert value in key '%s'",
                  self->info.name, self->info.routerId, key);
            goto cleanup;
        }

        zframe_destroy(&valueFrame);
    }

    status = true;

cleanup:
    if (!status) {
        zhash_destroy(_out);
    }
    zmsg_destroy(&response);
    zframe_destroy(&responseStatusFrame);
    zframe_destroy(&valueFrame);

    return status;
}

bool dbClientGetValue(DbClient *self, DbObject **out) {
    zhash_t *objects = NULL;

    if (!(dbClientGetValues(self, &objects))) {
        error("Cannot get values.");
        return false;
    }

    if (zhash_size(objects) != 1) {
        error("Objects retrieved must be egal to 1.");
        return false;
    }

    DbObject *object = zhash_first(objects);
    *out = object;

    zhash_destroy(&objects);

    return true;
}

bool dbClientUpdateValues(DbClient *self, zhash_t *objects) {

    bool status = false;
    zmsg_t *request = NULL;

    if (!(request = zmsg_new())) {
        error("%s:%d : Cannot allocate a new request msg.",
              self->info.name, self->info.routerId);
        goto cleanup;
    }

    // set the header
    if (zmsg_addmem(request, PACKET_HEADER(DB_UPDATE_ARRAY), sizeof(DB_UPDATE_ARRAY) != 0)) {
        error("%s:%d : Cannot add DB_UPDATE_ARRAY to dbClient message.",
              self->info.name, self->info.routerId);
        goto cleanup;
    }

    for (DbObject *object = zhash_first(objects); object != NULL; object = zhash_next(objects)) {
        char *key = (char *) zhash_cursor(objects);

        // add all the objects to the message
        if (!(dbClientAddObjectToMessage(self, request, key, object))) {
            error("Cannot add object '%s' to message.", key);
            goto cleanup;
        }
    }

    // send the request
    if (zmsg_send(&request, self->connection) != 0) {
        error("Cannot send the message to the Db.");
        goto cleanup;
    }

    status = true;

cleanup:
    zmsg_destroy(&request);

    return status;
}

bool dbClientUpdateValue(DbClient *self, char *key, DbObject *object) {

    bool status = false;
    zhash_t *ht = NULL;

    if (!(ht = zhash_new())) {
        error("Cannot allocate a hashtable.");
        goto cleanup;
    }

    if (zhash_insert(ht, key, object) != 0) {
        error("Cannot insert the object.");
        goto cleanup;
    }

    if (!(dbClientUpdateValues(self, ht))) {
        error("Cannot update value of object '%s'", key);
        goto cleanup;
    }

    status = true;

cleanup:
    zhash_destroy(&ht);

    return status;
}

void dbClientFree(DbClient *self) {
    zsock_destroy(&self->connection);
    dbClientInfoFree(&self->info);
}

void dbClientDestroy(DbClient **_self) {
    DbClient *self = *_self;

    if (_self && self) {
        dbClientFree(self);
        free(self);
        *_self = NULL;
    }
}

void dbClientInfoFree(DbClientInfo *self) {
    free(self->name);
}

void dbClientInfoDestroy(DbClientInfo **_self) {
    DbClientInfo *self = *_self;

    if (_self && self) {
        dbClientInfoFree(self);
        free(self);
        *_self = NULL;
    }
}
