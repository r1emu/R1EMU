/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file ZoneServer.h
 * @brief This module contains the specific actions to do when using a ZoneServer
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

// ---------- Includes ------------
#include "ZoneServer.h"
#include "Common/Server/Server.h"


// ------ Structure declaration -------
/**
 * @brief ZoneServer is the representation of the zone server system
 */
struct ZoneServer
{
    /** ZoneServer inherits from Server object */
    Server *server;
};


// ------ Static declaration ------

// ------ Extern function implementation ------

ZoneServer *
ZoneServer_new (
    Server *server
) {
    ZoneServer *self;

    if ((self = calloc (1, sizeof (ZoneServer))) == NULL) {
        return NULL;
    }

    if (!ZoneServer_init (self, server)) {
        ZoneServer_destroy (&self);
        error ("ZoneServer failed to initialize.");
        return NULL;
    }

    return self;
}

bool
ZoneServer_init (
    ZoneServer *self,
    Server *server
) {
    self->server = server;

    return true;
}

bool
ZoneServer_start (
    ZoneServer *self
) {
    special ("=====================");
    special ("=== Zone server %d ===", Server_getRouterId (self->server));
    special ("=====================");

    if (!(Server_start (self->server))) {
        error ("Cannot start the Server.");
        return false;
    }

    return true;
}

void
ZoneServer_destroy (
    ZoneServer **_self
) {
    ZoneServer *self = *_self;

    free (self);
    *_self = NULL;
}
