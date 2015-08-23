/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file SocialServer.h
 * @brief This module contains the specific actions to do when using a SocialServer
 *
 * @license <license placeholder>
 */

// ---------- Includes ------------
#include "SocialServer.h"
#include "Common/Server/Server.h"


// ------ Structure declaration -------
/**
 * @brief SocialServer is the representation of the social server system
 */
struct SocialServer
{
    /** SocialServer inherits from Server object */
    Server *server;
};


// ------ Static declaration ------

// ------ Extern function implementation ------

SocialServer *
SocialServer_new (
    Server *server
) {
    SocialServer *self;

    if ((self = calloc (1, sizeof (SocialServer))) == NULL) {
        return NULL;
    }

    if (!SocialServer_init (self, server)) {
        SocialServer_destroy (&self);
        error ("SocialServer failed to initialize.");
        return NULL;
    }

    return self;
}

bool
SocialServer_init (
    SocialServer *self,
    Server *server
) {
    self->server = server;

    return true;
}

bool
SocialServer_start (
    SocialServer *self
) {
    special ("======================");
    special ("==== Social server ===");
    special ("======================");

    if (!(Server_start (self->server))) {
        error ("Cannot start the Server.");
        return false;
    }

    return true;
}

void
SocialServer_destroy (
    SocialServer **_self
) {
    SocialServer *self = *_self;

    free (self);
    *_self = NULL;
}
