/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file BarrackServer.h
 * @brief This module contains the specific actions to do when using a BarrackServer
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

// ---------- Includes ------------
#include "BarrackServer.h"
#include "Common/Server/Server.h"


// ------ Structure declaration -------
/**
 * @brief BarrackServer is the representation of the barrack server system
 */
struct BarrackServer
{
    /** BarrackServer inherits from Server object */
    Server *server;
};


// ------ Static declaration ------

// ------ Extern function implementation ------

BarrackServer *
BarrackServer_new (
    Server *server
) {
    BarrackServer *self;

    if ((self = calloc (1, sizeof (BarrackServer))) == NULL) {
        return NULL;
    }

    if (!BarrackServer_init (self, server)) {
        BarrackServer_destroy (&self);
        error ("BarrackServer failed to initialize.");
        return NULL;
    }

    return self;
}

bool
BarrackServer_init (
    BarrackServer *self,
    Server *server
) {
    self->server = server;

    return true;
}

bool
BarrackServer_start (
    BarrackServer *self
) {
    special ("======================");
    special ("=== Barrack server ===");
    special ("======================");

    if (!(Server_start (self->server))) {
        error ("Cannot start the Server.");
        return false;
    }

    return true;
}

void
BarrackServer_destroy (
    BarrackServer **_self
) {
    BarrackServer *self = *_self;

    Server_destroy (&self->server);

    free (self);
    *_self = NULL;
}
