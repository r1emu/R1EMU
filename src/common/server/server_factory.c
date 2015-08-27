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

#include "server_factory.h"
#include "zone_server/zone_handler/zone_handler.h"
#include "barrack_server/Barrack_handler/barrack_handler.h"
#include "social_server/social_handler/social_handler.h"

Server *serverFactoryCreateServer (
    ServerType serverType,
    uint16_t routerId,
    char *routerIp,
    int portsCount,
    int *ports,
    int workersCount,
    char *globalServerIp,
    int globalServerPort,
    char *sqlHostname,
    char *sqlUsername,
    char *sqlPassword,
    char *sqlDatabase,
    char *redisHostname,
    int redisPort) {
    Server *server;

    ServerStartupInfo serverInfo;
    if (!(serverFactoryInitServerInfo (&serverInfo,
        serverType,
        routerId, routerIp,
        portsCount, ports,
        workersCount,
        globalServerIp, globalServerPort,
        sqlHostname, sqlUsername, sqlPassword, sqlDatabase,
        redisHostname, redisPort
    ))) {
        error("Cannot build a ServerStartupInfo.");
        return NULL;
    }

    // Allocate a new server
    if (!(server = serverNew (&serverInfo))) {
        error("Cannot allocate a new Server.");
        return NULL;
    }

    return server;
}

bool
serverFactoryInitServerInfo (
    ServerStartupInfo *serverInfo,
    ServerType serverType,
    uint16_t routerId,
    char *routerIp,
    int portsCount,
    int *ports,
    int workersCount,
    char *globalServerIp,
    int globalServerPort,
    char *sqlHostname,
    char *sqlUsername,
    char *sqlPassword,
    char *sqlDatabase,
    char *redisHostname,
    int redisPort
) {
    // Initialize MySQL start up information
    MySQLStartupInfo sqlInfo;
    if (!(mySqlStartupInfoInit (&sqlInfo, sqlHostname, sqlUsername, sqlPassword, sqlDatabase))) {
        error("Cannot initialize correctly the MySQL start up information.");
        return false;
    }

    // Initialize Redis start up information
    RedisStartupInfo redisInfo;
    if (!(redisStartupInfoInit(&redisInfo, redisHostname, redisPort))) {
        error("Cannot initialize correctly the Redis start up information.");
        return false;
    }

    // Initialize Router start up information
    RouterStartupInfo routerInfo;
    if (!(routerStartupInfoInit (&routerInfo, routerId, routerIp, ports, portsCount, workersCount, &redisInfo, &sqlInfo))) {
        error("Cannot initialize correctly the Router start up information.");
        return false;
    }

    // Initialize Worker start up information
    WorkerStartupInfo *workersInfo;
    if (!(workersInfo = malloc (sizeof(WorkerStartupInfo) * workersCount))) {
        error("Cannot allocate an array of %d WorkerStartUpInfo.", workersCount);
        return false;
    }

    // Use the correct packetHandlers based on the ID
    const PacketHandler *packetHandlers = NULL;
    int packetHandlersCount;

    switch (serverType)
    {
        case SERVER_TYPE_BARRACK:
            packetHandlers = barrackHandlers;
            packetHandlersCount = PACKET_TYPE_COUNT;
        break;

        case SERVER_TYPE_ZONE:
            packetHandlers = zoneHandlers;
            packetHandlersCount = PACKET_TYPE_COUNT;
        break;

        case SERVER_TYPE_SOCIAL:
            packetHandlers = socialHandlers;
            packetHandlersCount = PACKET_TYPE_COUNT;
        break;

        default :
            error("Unknown serverType : %d !", serverType);
        break;
    }

    for (uint16_t workerId = 0; workerId < workersCount; workerId++) {
        if (!(workerStartupInfoInit (
            &workersInfo[workerId],
            workerId, routerId,
            serverType,
            globalServerIp, globalServerPort,
            &sqlInfo, &redisInfo,
            packetHandlers, packetHandlersCount)
        )) {
            error("Cannot initialize a Worker.");
            return false;
        }
    }

    // Initialize Server start up information
    if (!(serverStartupInfoInit (serverInfo, serverType, &routerInfo, workersInfo, workersCount))) {
        error("Cannot initialize correctly the Server start up information.");
        return false;
    }

    return true;
}
