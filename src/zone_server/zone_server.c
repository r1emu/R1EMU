/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file zone_server.c
 * @brief This module contains the specific actions to do when using a ZoneServer
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#include "zone_server.h"
#include "zone_handler/admin_cmd.h"
#include "common/server/server.h"
#include "common/db/db.h"

/**
 * @brief ZoneServer is the representation of the zone server system
 */
struct ZoneServer
{
    // ZoneServer inherits from Server object
    Server *server;

    // Items db
    Db *itemsDb;
};

ZoneServer *zoneServerNew(Server *server) {
    ZoneServer *self;

    if ((self = calloc(1, sizeof(ZoneServer))) == NULL) {
        return NULL;
    }

    if (!zoneServerInit(self, server)) {
        zoneServerDestroy(&self);
        error("ZoneServer failed to initialize.");
        return NULL;
    }

    return self;
}

bool zoneServerInit(ZoneServer *self, Server *server) {

    self->server = server;

    // Initialize itemsDb
    DbInfo dbInfo;
    dbInfoInit(&dbInfo, serverGetRouterId(server), "itemsDb", NULL, NULL);
    if (!(self->itemsDb = dbNew(&dbInfo))) {
        error("Cannot initialize itemsDb");
        return false;
    }

    return true;
}

bool zoneServerStart(ZoneServer *self) {
    special("=====================");
    special("=== Zone server %d ===", serverGetRouterId(self->server));
    special("=====================");

    // Start itemsDb actor
    if (!(dbStart(self->itemsDb))) {
        error("Cannot start itemsDb actor.");
        return false;
    }

    // Initialize admin commands module
    if (!(adminCmdInit())) {
        error ("Cannot initialize admin commands module.");
        return false;
    }

    if (!(serverStart(self->server))) {
        error("Cannot start the Server.");
        return false;
    }

    return true;
}

void zoneServerDestroy(ZoneServer **_self) {
    ZoneServer *self = *_self;

    free(self);
    *_self = NULL;
}
