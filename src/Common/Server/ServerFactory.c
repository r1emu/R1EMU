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


// ---------- Includes ------------
#include "ServerFactory.h"
#include "ZoneServer/ZoneHandler/ZoneHandler.h"
#include "BarrackServer/BarrackHandler/BarrackHandler.h"
#include "SocialServer/SocialHandler/SocialHandler.h"


// ------ Structure declaration -------


// ------ Static declaration -------


// ------ Extern function implementation -------

Server *
ServerFactory_createServer (
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
    Server *server;

    ServerStartupInfo serverInfo;
    if (!(ServerFactory_initServerInfo (&serverInfo,
        serverType,
        routerId, routerIp,
        portsCount, ports,
        workersCount,
        globalServerIp, globalServerPort,
        sqlHostname, sqlUsername, sqlPassword, sqlDatabase,
        redisHostname, redisPort
    ))) {
        error ("Cannot build a ServerStartupInfo.");
        return NULL;
    }

    // Allocate a new server
    if (!(server = Server_new (&serverInfo))) {
        error ("Cannot allocate a new Server.");
        return NULL;
    }

    return server;
}

bool
ServerFactory_initServerInfo (
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
    if (!(MySQLStartupInfo_init (&sqlInfo, sqlHostname, sqlUsername, sqlPassword, sqlDatabase))) {
        error ("Cannot initialize correctly the MySQL start up information.");
        return false;
    }

    // Initialize Redis start up information
    RedisStartupInfo redisInfo;
    if (!(RedisStartupInfo_init (&redisInfo, redisHostname, redisPort))) {
        error ("Cannot initialize correctly the Redis start up information.");
        return false;
    }

    // Initialize Router start up information
    RouterStartupInfo routerInfo;
    if (!(RouterStartupInfo_init (&routerInfo, routerId, routerIp, ports, portsCount, workersCount, &redisInfo, &sqlInfo))) {
        error ("Cannot initialize correctly the Router start up information.");
        return false;
    }

    // Initialize Worker start up information
    WorkerStartupInfo *workersInfo;
    if (!(workersInfo = malloc (sizeof (WorkerStartupInfo) * workersCount))) {
        error ("Cannot allocate an array of %d WorkerStartUpInfo.", workersCount);
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
            error ("Unknown serverType : %d !", serverType);
        break;
    }

    for (uint16_t workerId = 0; workerId < workersCount; workerId++) {
        if (!(WorkerStartupInfo_init (
            &workersInfo[workerId],
            workerId, routerId,
            serverType,
            globalServerIp, globalServerPort,
            &sqlInfo, &redisInfo,
            packetHandlers, packetHandlersCount)
        )) {
            error ("Cannot initialize a Worker.");
            return false;
        }
    }

    // Initialize Server start up information
    if (!(ServerStartupInfo_init (serverInfo, serverType, &routerInfo, workersInfo, workersCount))) {
        error ("Cannot initialize correctly the Server start up information.");
        return false;
    }

    return true;
}
