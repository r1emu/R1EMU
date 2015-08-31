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
 * @brief Entry point of the Servers
 *
 * Entry point of the Servers
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

// ---------- Includes ------------
#include "zone_server/zone_server.h"
#include "zone_server/zone_event_server.h"
#include "barrack_server/barrack_server.h"
#include "barrack_server/barrack_event_server.h"
#include "social_server/social_server.h"
#include "social_server/social_event_server.h"
#include "common/server/event_server.h"
#include "common/server/server_factory.h"

int main (int argc, char **argv)
{
    if (argc <= 1) {
        info("Please launch GlobalServer instead of ZoneServer. (argc=%d)", argc);
        exit (0);
    }

    ZoneServer *zoneServer = NULL;
    BarrackServer *barrackServer = NULL;
    SocialServer *socialServer = NULL;

    // === Crash handler ===
    #ifdef WIN32
    SetUnhandledExceptionFilter (crashHandler);
    #else
    struct sigaction sa = {};
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = crashHandler;
    sigaction(SIGSEGV, &sa, NULL);
    sigaction(SIGABRT, &sa, NULL);
    #endif

    // === Read the command line arguments ===
    uint16_t routerId = atoi(*++argv);
    char *routerIp = *++argv;
    int port = atoi(*++argv);
    uint16_t workersCount = atoi(*++argv);
    char *globalServerIp = *++argv;
    int globalServerPort = atoi(*++argv);
    char *sqlHostname = *++argv;
    char *sqlUsername = *++argv;
    char *sqlPassword = *++argv;
    char *sqlDatabase = *++argv;
    char *redisHostname = *++argv;
    int redisPort = atoi(*++argv);
    ServerType serverType = atoi(*++argv);

    // Set a custom output for linux for each servers
    #ifndef WIN32
    dbgSetOutput(fopen(zsys_sprintf("ZoneServer%d_output.txt", routerId), "w+"));
    #else
    // For Windows, change the console title
    switch (serverType) {
        case SERVER_TYPE_BARRACK:
            SetConsoleTitle(zsys_sprintf("Barrack (%d)", routerId));
        break;

        case SERVER_TYPE_ZONE:
            SetConsoleTitle(zsys_sprintf("Zone (%d)", routerId));
        break;

        case SERVER_TYPE_SOCIAL:
            SetConsoleTitle(zsys_sprintf("Social (%d)", routerId));
        break;

        default :
            SetConsoleTitle(zsys_sprintf("UNKNOWN (%d)", routerId));
        break;
    }
    #endif

    // === Build the Event Server ===
    EventServer *eventServer;
    EventServerStartupInfo eventServerInfo;
    if (!(eventServerStartupInfoInit(&eventServerInfo, routerId, workersCount, redisHostname, redisPort))) {
        error("Cannot initialize the event server.");
        return -1;
    }
    if (!(eventServer = eventServerNew(&eventServerInfo, serverType))) {
        error("Cannot create the event server.");
        return -1;
    }
    if ((zthread_new((zthread_detached_fn *) eventServerStart, eventServer)) != 0) {
        error("Cannot start the event server.");
        return -1;
    }

    // Specific server stuff
    DisconnectEventHandler disconnectHandler;
    switch (serverType) {
        case SERVER_TYPE_BARRACK:
            disconnectHandler = barrackEventServerOnDisconnect;
        break;

        case SERVER_TYPE_ZONE:
            disconnectHandler = zoneEventServerOnDisconnect;
        break;

        case SERVER_TYPE_SOCIAL:
            disconnectHandler = socialEventServerOnDisconnect;
        break;

        default :
            error("Unknown server type.");
            return 0;
        break;
    }

    // === Build the Server ===
    Server *server;
    if (!(server = serverFactoryCreateServer(
        serverType,
        routerId, routerIp,
        port,
        workersCount,
        globalServerIp, globalServerPort,
        sqlHostname, sqlUsername, sqlPassword, sqlDatabase,
        redisHostname, redisPort, disconnectHandler
    ))) {
        error("Cannot create a Server.");
        return -1;
    }

    // Initialize the Server
    switch (serverType)
    {
        case SERVER_TYPE_BARRACK:
            // Initialize the Barrack Server
            if ((barrackServer = barrackServerNew(server))) {

                // Start the Barrack Server
                if (!barrackServerStart(barrackServer)) {
                    error("Cannot start the BarrackServer properly.");
                }

                // Unload the Barrack Server properly
                barrackServerDestroy(&barrackServer);
            }
            else {
                error("Cannot initialize the BarrackServer properly.");
            }
        break;

        case SERVER_TYPE_ZONE:
            // Initialize the Zone Server
            if ((zoneServer = zoneServerNew(server))) {

                // Start the Zone Server
                if (!zoneServerStart(zoneServer)) {
                    error("Cannot start the Zone Server properly.");
                }

                // Unload the Zone Server properly
                zoneServerDestroy(&zoneServer);
            }
            else {
                error("Cannot initialize the Zone Server properly.");
            }
        break;

        case SERVER_TYPE_SOCIAL:
            // Initialize the Social Server
            if ((socialServer = socialServerNew(server))) {

                // Start the Social Server
                if (!socialServerStart(socialServer)) {
                    error("Cannot start the Social Server properly.");
                }

                // Unload the Social Server properly
                socialServerDestroy(&socialServer);
            }
            else {
                error("Cannot initialize the Social Server properly.");
            }
        break;

        default :
            error("Cannot start an unknown serverType.");
        break;
    }

    // Shutdown the CZMQ layer properly
    zsys_shutdown();

    // Close the custom debug file if necessary
    dbgClose();

    pause();

    return 0;
}
