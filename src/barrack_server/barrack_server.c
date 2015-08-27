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

/**
 * @brief BarrackServer is the representation of the barrack server system
 */
struct BarrackServer
{
    // BarrackServer inherits from Server object
    Server *server;
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
    self->server = server;
    return true;
}

bool barrackServerStart(BarrackServer *self) {
    special("======================");
    special("=== Barrack server ===");
    special("======================");

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
