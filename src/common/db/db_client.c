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
