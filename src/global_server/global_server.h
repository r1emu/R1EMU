/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file global_server.h
 * @brief GlobalServer keeps a table of all the zone servers and communicate with them.
 *
 * GlobalServer detains the authority on all the zone servers. It can start or shut them down.
 * It communicates with the nodes that need to communicates with all the zone servers.
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#pragma once

#include "R1EMU.h"
#include "common/redis/redis.h"
#include "common/mysql/mysql.h"
#include "common/server/server.h"

#define GLOBAL_SERVER_CLI_ENDPOINT          "tcp://%s:%d"
#define GLOBAL_SERVER_ZONES_ENDPOINT        "tcp://%s:*"
#define GLOBAL_SERVER_CLI_IP_DEFAULT        "127.0.0.1"

// configuration default values
#define GLOBAL_SERVER_CLI_PORT_DEFAULT      2003
#define GLOBAL_SERVER_ZONES_PORT_DEFAULT    2007

/**
 * Enumeration of all the packets headers that the global server handles
 */
// we want to differentiate the recv header from the send header, but we want to keep a list
// with uniques header id. So let's declare all the ids here, and distribute them afterward
typedef enum GlobalServerHeader {
    _GLOBAL_SERVER_PING, // ping
    _GLOBAL_SERVER_PONG, // pong
} GlobalServerHeader;

// macro helper for the distribution
#define DECL_GLOBAL_SERVER_HEADER(x) \
    x = _##x

/**
 * Enumeration of all the packets header that the global server accepts
 */
typedef enum GlobalServerRecvHeader {
    DECL_GLOBAL_SERVER_HEADER(GLOBAL_SERVER_PING),
}   GlobalServerRecvHeader;

/**
 * Enumeration of all the packets header that the global server sends
 */
typedef enum GlobalServerSendHeader {
    DECL_GLOBAL_SERVER_HEADER(GLOBAL_SERVER_PONG),
}   GlobalServerSendHeader;

#undef DECL_GLOBAL_SERVER_HEADER

// GlobalServer is opaque
typedef struct GlobalServer GlobalServer;

typedef struct {
    // server IP for the global server
    char *ip;

    // frontend port of the global server. It shouldn't be opened on internet
    int cliPort;

    // zone servers ports. They should be opened to the internet, as clients will connect to them
    int *socialServersPorts;

    // zone servers IP
    char **socialServersIp;

    // count of zone servers
    int socialServersCount;

    // count of worker for each zone servers
    int socialWorkersCount;

    // port for communicating with the zones
    int socialPort;

    // zone servers ports. They should be opened to the internet, as clients will connect to them
    int *zoneServersPorts;

    // zone servers IP
    char **zoneServersIp;

    // count of zone servers
    int zoneServersCount;

    // count of worker for each zone servers
    int zoneWorkersCount;

    // port for communicating with the zones
    int zonesPort;

    // barrack servers ports. They should be opened to the internet, as clients will connect to them
    int *barrackServerPort;

    // number of ports
    int barrackServerPortCount;

    // barrack servers IP
    char *barrackServerIp;

    // count of workers
    int barrackWorkersCount;

    // configuration file path
    char *confFilePath;

    // database
    MySQLStartupInfo sqlInfo;
    RedisStartupInfo redisInfo;

} GlobalServerStartupInfo;

/**
 * @brief Allocate a new GlobalServer structure.
 * @param info An allocated GlobalServerStartupInfo.
 * @return A pointer to an allocated GlobalServer.
 */
GlobalServer *globalServerNew(GlobalServerStartupInfo *info);

/**
 * @brief Initialize an allocated GlobalServer structure.
 * @param self An allocated GlobalServer to initialize.
 * @param info An allocated GlobalServerStartupInfo.
 * @return true on success, false otherwise.
 */
bool globalServerInit(GlobalServer *self, GlobalServerStartupInfo *info);

/**
 * @brief Initialize an allocated GlobalServerStartupInfo structure.
 * @param self An allocated GlobalServerStartupInfo to initialize.
 * @param confFilePath The configuration file of the Global Server.
 * @return true on success, false otherwise.
 */
bool globalServerStartupInfoInit(GlobalServerStartupInfo *self,char *confFilePath);

/**
 * @brief Request to the GlobalServer to flush the Redis Server
 * @param self An allocated GlobalServer
 * @return true on success, false otherwise.
 */
bool globalServerFlushRedis(GlobalServer *self);

/**
 * @brief Free an allocated GlobalServer structure and nullify the content of the pointer.
 * @param self A pointer to an allocated GlobalServer.
 */
void globalServerDestroy(GlobalServer **self);

/**
 * @brief Start the Global Server main loop.
 * @param self An allocated GlobalServer
 * @return true on success, false otherwise
 */
bool globalServerStart(GlobalServer *self);
