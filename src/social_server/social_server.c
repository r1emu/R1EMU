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
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#include "social_server.h"
#include "common/server/server.h"

/**
 * @brief SocialServer is the representation of the social server system
 */
struct SocialServer
{
    // SocialServer inherits from Server object
    Server *server;
};

SocialServer *socialServerNew(Server *server) {
    SocialServer *self;

    if ((self = calloc(1, sizeof(SocialServer))) == NULL) {
        return NULL;
    }

    if (!socialServerInit(self, server)) {
        socialServerDestroy(&self);
        error("SocialServer failed to initialize.");
        return NULL;
    }

    return self;
}

bool socialServerInit(SocialServer *self, Server *server) {
    self->server = server;
    return true;
}

bool socialServerStart(SocialServer *self) {
    special("======================");
    special("==== Social server ===");
    special("======================");

    if (!(serverStart(self->server))) {
        error ("Cannot start the Server.");
        return false;
    }

    return true;
}

void socialServerDestroy(SocialServer **_self) {
    SocialServer *self = *_self;

    free (self);
    *_self = NULL;
}
