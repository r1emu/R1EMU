/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file main.c
 * @brief Entry point of the Global Server
 *
 * Entry point of the Global Server
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

// configuration environment
#define DEFAULT_SERVER_CONF_PATH "../cfg/server.cfg"

#include "common/server/server_factory.h"
#include "global_server/global_server.h"

int main(int argc, char **argv)
{
     // Get the configuration file
    char *confFilePath = NULL;

    GlobalServer *globalServer = NULL;
    GlobalServerStartupInfo info;

    // force the initialization of the CZMQ layer here.
    if (!(zsys_init ())) {
        error("Cannot init CZMQ.");
        goto cleanup;
    }

    if (argc >= 2) {
        confFilePath = argv[1];
    } else {
        confFilePath = DEFAULT_SERVER_CONF_PATH;
    }

    if (!(globalServerStartupInfoInit(&info, confFilePath))) {
        error("Cannot initialize GlobalServer init information. (%s)", confFilePath);
        goto cleanup;
    }

    // initialize the Global Server
    if (!(globalServer = globalServerNew(&info))) {
        error("Cannot initialize the GlobalServer properly.");
        goto cleanup;
    }

    // flush the Redis server
    if (!(globalServerFlushRedis(globalServer))) {
        error("Cannot flush the Redis server properly.");
        goto cleanup;
    }

    // start the Global Server
    else if(!globalServerStart(globalServer)) {
        error("Cannot start the GlobalServer properly.");
        goto cleanup;
    }

cleanup:
    // unload the Global Server properly
    globalServerDestroy(&globalServer);

    // shutdown the CZMQ layer properly
    zsys_shutdown();

    pause();

    return 0;
}
