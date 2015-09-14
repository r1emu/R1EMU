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
 * @brief DbClient contains
 */
struct DbClient
{
    DbClientInfo info;
    zsock_t *connection;
};

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
        error("Cannot create connection socket.");
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
        error("Cannot connect to %s.", endpointStr);
        return false;
    }

    info("%s connected.", endpointStr);

    return true;
}


bool dbClientGetValue(DbClient *self, DbKey key, char **value) {

    bool status = false;
    zmsg_t *request = zmsg_new();
    zmsg_t *response = NULL;
    zframe_t *responseStatusFrame = NULL;
    zframe_t *valueFrame = NULL;

    if(zmsg_addmem(request, PACKET_HEADER(DB_GET_ARRAY), sizeof(DB_GET_ARRAY) != 0)){
        error("%s:%d : Cannot add DB_GET_ARRAY to dbClient GET message.",
              self->info.name, self->info.routerId);
        goto cleanup;
    }

    if(zmsg_addmem(request, &key, sizeof(key)) != 0){
        error("%s:%d : Cannot add key to dbClient GET message.",
              self->info.name, self->info.routerId);
        goto cleanup;
    }

    zmsg_send(&request, self->connection);

    if(!(response = zmsg_recv(self->connection))){
        error("%s:%d : Cannot receive a message.",
              self->info.name, self->info.routerId);
        goto cleanup;
    }

    if(!(responseStatusFrame = zmsg_pop(response))){
        error("%s:%d : Cannot read result header.",
              self->info.name, self->info.routerId);
        goto cleanup;
    }

    DbStatus responseStatus = *((typeof(responseStatus) *) zframe_data(responseStatusFrame));
    if (responseStatus == DB_STATUS_CANNOT_GET){
        error("%s:%d : Database could not GET key.",
              self->info.name, self->info.routerId);
        goto cleanup;
    }

    if(!(valueFrame = zmsg_pop(response))){
        error("%s:%d : Could not read value from GET response.",
              self->info.name, self->info.routerId);
        goto cleanup;
    }

    *value = strdup((char *) zframe_data(valueFrame));
    status = true;

cleanup:
    // TODO

    return status;
}

void dbClientFree(DbClient *self) {
    // TODO
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
    // TODO
}

void dbClientInfoDestroy(DbClientInfo **_self) {
    DbClientInfo *self = *_self;

    if (_self && self) {
        dbClientInfoFree(self);
        free(self);
        *_self = NULL;
    }
}
