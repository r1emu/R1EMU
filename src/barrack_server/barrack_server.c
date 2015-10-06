/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file barrack_server.h
 * @brief This module contains the specific actions to do when using a BarrackServer
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#include "barrack_server.h"
#include "common/server/server.h"
#include "common/db/db.h"
#include "common/actor/item/item_factory.h"

/**
 * @brief BarrackServer is the representation of the barrack server system
 */
struct BarrackServer
{
    /** BarrackServer inherits from Server object */
    Server *server;

    /** Connection to the dbSession */
    Db *dbSession;
};

BarrackServer *barrackServerNew(Server *server) {
    BarrackServer *self;

    if ((self = calloc(1, sizeof(BarrackServer))) == NULL) {
        return NULL;
    }

    if (!barrackServerInit(self, server)) {
        barrackServerDestroy(&self);
        error("BarrackServer failed to initialize.");
        return NULL;
    }

    return self;
}

bool barrackServerInit(BarrackServer *self, Server *server) {
    DbInfo dbInfo;

    self->server = server;
    uint16_t routerId = serverGetRouterId(server);

    // Initialize dbSession
    dbInfoInit(&dbInfo, routerId, "dbSession");
    if (!(self->dbSession = dbNew(&dbInfo))) {
        error("Cannot allocate a dbSession.");
        return false;
    }

    return true;
}

bool barrackServerStart(BarrackServer *self) {
    special("======================");
    special("=== Barrack server ===");
    special("======================");

    // Start dbSession
    if (!(dbStart(self->dbSession))) {
        error("Cannot start sessions db.");
        return false;
    }

    // Initialize itemFactory
    if (!(itemFactoryStart(serverGetMySQLInfo(self->server)))) {
        error("Cannot initialize item factory.");
        return false;
    }

    if (!(serverStart (self->server))) {
        error("Cannot start the Server.");
        return false;
    }

    return true;
}

void barrackServerDestroy(BarrackServer **_self) {
    BarrackServer *self = *_self;

    serverDestroy(&self->server);
    free(self);
    *_self = NULL;
}
