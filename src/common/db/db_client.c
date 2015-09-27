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
static bool dbClientIsSuccess(DbClient *self, zmsg_t *msg);

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
        dbClientError(self, "Cannot create connection socket.");
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

bool dbClientConnect(DbClient *self) {

    char *endpointStr = zsys_sprintf(DB_ENDPOINT, self->info.name, self->info.routerId);

    if (zsock_connect(self->connection, endpointStr) != 0) {
        dbClientError(self, "Cannot connect to %s.", endpointStr);
        return false;
    }

    dbClientInfo(self, "%s connected.", endpointStr);

    return true;
}

static bool dbClientAddKeysToMessage(DbClient *self, zmsg_t *request, char **keys, size_t keysCount) {

    bool status = false;

    for (size_t keyIdx; keyIdx < keysCount; keyIdx++) {
        // add all the keys requested to the message
        if (zmsg_addstr(request, keys[keyIdx]) != 0) {
            dbClientError(self, "Cannot add key to message.");
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
        dbClientError(self, "Cannot add key to message.");
        goto cleanup;
    }

    if (zmsg_addmem(request, object->data, object->dataSize) != 0) {
        dbClientError(self, "Cannot add object to message.");
        goto cleanup;
    }

    status = true;

cleanup:
    return status;
}

bool dbClientRemoveObjects(DbClient *self, char **keys, size_t keysCount) {

    bool status = false;
    zmsg_t *request = NULL;
    zmsg_t *answer = NULL;

    if (!(request = zmsg_new())) {
        dbClientError(self, "Cannot allocate a new request msg.");
        goto cleanup;
    }

    // add the header
    if (zmsg_addmem(request, PACKET_HEADER(DB_REMOVE_ARRAY), sizeof(DB_REMOVE_ARRAY) != 0)){
        dbClientError(self, "Cannot add DB_REMOVE_ARRAY to dbClient REMOVE message.");
        goto cleanup;
    }

    // add the keys to the message
    if (!(dbClientAddKeysToMessage(self, request, keys, keysCount))) {
        dbClientError(self, "Cannot add keys to the request message.");
        goto cleanup;
    }

    if (zmsg_send(&request, self->connection) != 0) {
        dbClientError(self, "Cannot send remove request.");
        goto cleanup;
    }

    // wait for db answer
    if (!(answer = zmsg_recv(self->connection))) {
        dbClientError(self, "Cannot receive a message.");
        goto cleanup;
    }

    // check header status
    if (!(dbClientIsSuccess(self, answer))) {
        dbClientError(self, "Cannot get object from db.");
        goto cleanup;
    }

    status = true;

cleanup:
    zmsg_destroy(&request);

    return status;
}

bool dbClientRemoveObject(DbClient *self, char *key) {
    if (!(dbClientRemoveObjects(self, (char *[]){key}, 1))) {
        dbClientError(self, "Cannot remove value '%s'", key);
        return false;
    }

    return true;
}

bool dbClientGetObjectsSync(DbClient *self, char **keys, size_t keysCount, zhash_t **out) {

    bool status = false;

    if (!(dbClientRequestObjects(self, keys, keysCount))) {
        dbClientError(self, "Cannot request objects.");
        goto cleanup;
    }

    if (!(dbClientGetObjects(self, out))) {
        dbClientError(self, "Cannot get objects.");
        goto cleanup;
    }

    status = true;

cleanup:

    return status;
}

bool dbClientGetObjectSync(DbClient *self, char *key, DbObject **out) {

    bool status = false;
    zhash_t *objects = NULL;

    if (!(dbClientGetObjectsSync(self, (char *[]) {key}, 1, &objects))) {
        dbClientError(self, "Cannot get object synchronously.");
        goto cleanup;
    }
    status = true;

    if (zhash_size(objects) != 1 && zhash_size(objects) != 0) {
        dbClientError(self, "Objects count retrieved must be egal to 0 or 1.");
        goto cleanup;
    }

    *out = zhash_first(objects);

cleanup:
    if (!status) {
        if (objects) {
                for (DbObject *o = zhash_first(objects); o != NULL; o = zhash_next(objects)) {
                dbObjectDestroy(&o);
            }
        }
    }

    zhash_destroy(&objects);
    return status;
}

bool dbClientRequestObjects(DbClient *self, char **keys, size_t keysCount) {

    bool status = false;
    zmsg_t *request = NULL;

    if (!(request = zmsg_new())) {
        dbClientError(self, "Cannot allocate a new request msg.");
        goto cleanup;
    }

    // add the header
    if (zmsg_addmem(request, PACKET_HEADER(DB_GET_ARRAY), sizeof(DB_GET_ARRAY) != 0)) {
        dbClientError(self, "Cannot add DB_GET_ARRAY to dbClient GET message.");
        goto cleanup;
    }

    // add the keys to the message
    if (!(dbClientAddKeysToMessage(self, request, keys, keysCount))) {
        dbClientError(self, "Cannot add keys to the request message.");
        goto cleanup;
    }

    // send the request
    if (zmsg_send(&request, self->connection) != 0) {
        dbClientError(self, "Cannot send the message to the Db.");
        goto cleanup;
    }

    status = true;

cleanup:
    zmsg_destroy(&request);

    return status;
}

bool dbClientRequestObject(DbClient *self, char *key) {
    if (!(dbClientRequestObjects(self, (char *[]){key}, 1))) {
        dbClientError(self, "Cannot request value '%s'", key);
        return false;
    }

    return true;
}

static bool dbClientIsSuccess(DbClient *self, zmsg_t *msg) {

    bool status = false;
    zframe_t *answerStatusFrame = NULL;

    // get header status
    if (!(answerStatusFrame = zmsg_pop(msg))) {
        dbClientError(self, "Cannot read result header.");
        goto cleanup;
    }

    // check header status
    DbStatus answerStatus = *((typeof(answerStatus) *) zframe_data(answerStatusFrame));
    if (answerStatus != DB_STATUS_SUCCESS) {
        dbClientError(self, "Error status. Status = %d", answerStatus);
        goto cleanup;
    }

    status = true;

cleanup:
    zframe_destroy(&answerStatusFrame);

    return status;
}

bool dbClientGetObjects(DbClient *self, zhash_t **_out) {

    bool status = false;
    zmsg_t *answer = NULL;
    zframe_t *valueFrame = NULL;
    zhash_t *out = *_out = NULL;

    // wait for db answer
    if (!(answer = zmsg_recv(self->connection))) {
        dbClientError(self, "Cannot receive a message.");
        goto cleanup;
    }

    // check header status
    if (!(dbClientIsSuccess(self, answer))) {
        dbClientError(self, "Cannot get object from db.");
        goto cleanup;
    }

    // allocate result hashtable
    if (!(out = zhash_new())) {
        dbClientError(self, "Cannot allocate a new values hashtable.");
        goto cleanup;
    }

    char *key;
    while ((key = zmsg_popstr(answer)) != NULL) {

        // key is followed by the value associated to the key
        if (!(valueFrame = zmsg_pop(answer))) {
            dbClientError(self, "Cannot get the object associated to the key '%s'", key);
            goto cleanup;
        }

        void *value = zframe_data(valueFrame);
        size_t valueSize = zframe_size(valueFrame);

        // Construct a dbObject from the frame
        DbObject *object = NULL;
        if (!(object = dbObjectNew(valueSize, value, false))) {
            dbClientError(self, "Cannot create an object '%s' of size %d.", key, valueSize);
            goto cleanup;
        }

        if (zhash_insert(out, key, object) != 0) {
            dbClientError(self, "Cannot insert value in key '%s'", key);
            goto cleanup;
        }

        zframe_destroy(&valueFrame);
    }

    *_out = out;
    status = true;

cleanup:
    if (!status) {
        zhash_destroy(_out);
    }
    zmsg_destroy(&answer);
    zframe_destroy(&valueFrame);

    return status;
}

bool dbClientGetObject(DbClient *self, DbObject **out) {

    bool status = false;
    zhash_t *objects = NULL;

    if (!(dbClientGetObjects(self, &objects))) {
        dbClientError(self, "Cannot get values.");
        goto cleanup;
    }

    if (!objects) {
        dbClientError(self, "Cannot get object hashtable.");
        goto cleanup;
    }

    if (zhash_size(objects) != 1 && zhash_size(objects) != 0) {
        dbClientError(self, "Objects count retrieved must be egal to 0 or 1.");
        goto cleanup;
    }

    *out = zhash_first(objects);

    status = true;

cleanup:
    if (!status) {
        if (objects) {
                for (DbObject *o = zhash_first(objects); o != NULL; o = zhash_next(objects)) {
                dbObjectDestroy(&o);
            }
        }
    }

    zhash_destroy(&objects);

    return status;
}

bool dbClientUpdateObjects(DbClient *self, zhash_t *objects) {

    bool status = false;
    zmsg_t *request = NULL;
    zmsg_t *answer = NULL;

    if (!(request = zmsg_new())) {
        dbClientError(self, "Cannot allocate a new request msg.");
        goto cleanup;
    }

    // set the header
    if (zmsg_addmem(request, PACKET_HEADER(DB_UPDATE_ARRAY), sizeof(DB_UPDATE_ARRAY) != 0)) {
        dbClientError(self, "Cannot add DB_UPDATE_ARRAY to dbClient message.");
        goto cleanup;
    }

    for (DbObject *object = zhash_first(objects); object != NULL; object = zhash_next(objects)) {
        char *key = (char *) zhash_cursor(objects);

        // add all the objects to the message
        if (!(dbClientAddObjectToMessage(self, request, key, object))) {
            dbClientError(self, "Cannot add object '%s' to message.", key);
            goto cleanup;
        }
    }

    // send the request
    if (zmsg_send(&request, self->connection) != 0) {
        dbClientError(self, "Cannot send the message to the Db.");
        goto cleanup;
    }

    // wait for the answer
    if (!(answer = zmsg_recv(self->connection))) {
        dbClientError(self, "Cannot receive a message from the Db.");
        goto cleanup;
    }

    // check header status
    if (!(dbClientIsSuccess(self, answer))) {
        dbClientError(self, "Cannot get object from db.");
        goto cleanup;
    }

    status = true;

cleanup:
    zmsg_destroy(&request);
    zmsg_destroy(&answer);

    return status;
}

bool dbClientUpdateObject(DbClient *self, char *key, DbObject *object) {

    bool status = false;
    zhash_t *ht = NULL;

    if (!(ht = zhash_new())) {
        dbClientError(self, "Cannot allocate a hashtable.");
        goto cleanup;
    }

    if (zhash_insert(ht, key, object) != 0) {
        dbClientError(self, "Cannot insert the object.");
        goto cleanup;
    }

    if (!(dbClientUpdateObjects(self, ht))) {
        dbClientError(self, "Cannot update value of object '%s'", key);
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
