/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file GlobalServer.h
 * @brief GlobalServer keeps a table of all the zone servers and communicate with them.
 *
 * GlobalServer detains the authority on all the zone servers. It can start or shut them down.
 * It communicates with the nodes that need to communicates with all the zone servers.
 *
 * @license <license placeholder>
 */

#pragma once

// ---------- Includes ------------
#include "R1EMU.h"
#include "Common/Redis/Redis.h"
#include "Common/MySQL/MySQL.h"
#include "Common/Server/Server.h"

// ---------- Defines -------------
#define GLOBAL_SERVER_CLI_ENDPOINT        "tcp://%s:%d"
#define GLOBAL_SERVER_ZONES_ENDPOINT      "tcp://%s:*"
#define GLOBAL_SERVER_CLI_IP_DEFAULT      "127.0.0.1"

// Configuration default values
#define GLOBAL_SERVER_CLI_PORT_DEFAULT         2003
#define GLOBAL_SERVER_ZONES_PORT_DEFAULT       2007

/** Enumeration of all the packets headers that the global server handles */
// We want to differentiate the recv header from the send header, but we want to keep a list
// with uniques header id. So let's declare all the ids here, and distribute them afterward
typedef enum GlobalServerHeader {
    _GLOBAL_SERVER_PING,                    // Ping
    _GLOBAL_SERVER_PONG,                    // Pong
}   GlobalServerHeader;

// Macro helper for the distribution
#define DECL_GLOBAL_SERVER_HEADER(x) \
    x = _##x

/** Enumeration of all the packets header that the global server accepts */
typedef enum GlobalServerRecvHeader {
    DECL_GLOBAL_SERVER_HEADER (GLOBAL_SERVER_PING),
}   GlobalServerRecvHeader;

/** Enumeration of all the packets header that the global server sends */
typedef enum GlobalServerSendHeader {
    DECL_GLOBAL_SERVER_HEADER (GLOBAL_SERVER_PONG),
}   GlobalServerSendHeader;

#undef DECL_GLOBAL_SERVER_HEADER


// ------ Structure declaration -------
// GlobalServer is opaque
typedef struct GlobalServer GlobalServer;

typedef struct {

    // === Global Server ===
    /** Server IP for the global server. */
    char *ip;

    /** Frontend port of the global server. It shouldn't be opened on internet. */
    int cliPort;

    // === Zone Server ===
    /** Zone servers ports. They should be opened to the internet, as clients will connect to them */
    int *socialServersPorts;

    /** Zone servers IP. */
    char **socialServersIp;

    /** Count of zone servers */
    int socialServersCount;

    /** Count of worker for each zone servers */
    int socialWorkersCount;

    /** Port for communicating with the zones */
    int socialPort;

    // === Zone Server ===
    /** Zone servers ports. They should be opened to the internet, as clients will connect to them */
    int *zoneServersPorts;

    /** Zone servers IP. */
    char **zoneServersIp;

    /** Count of zone servers */
    int zoneServersCount;

    /** Count of worker for each zone servers */
    int zoneWorkersCount;

    /** Port for communicating with the zones */
    int zonesPort;

    // === Barrack Server ===
    /** Barrack servers ports. They should be opened to the internet, as clients will connect to them */
    int *barrackServerPort;

    /** Number of ports */
    int barrackServerPortCount;

    /** Barrack servers IP. */
    char *barrackServerIp;

    /** Count of workers */
    int barrackWorkersCount;

    /** Configuration file path */
    char *confFilePath;

    // === Database ===
    MySQLStartupInfo sqlInfo;
    RedisStartupInfo redisInfo;

} GlobalServerStartupInfo;

// ----------- Functions ------------

/**
 * @brief Allocate a new GlobalServer structure.
 * @param info An allocated GlobalServerStartupInfo.
 * @return A pointer to an allocated GlobalServer.
 */
GlobalServer *
GlobalServer_new (
    GlobalServerStartupInfo *info
);


/**
 * @brief Initialize an allocated GlobalServer structure.
 * @param self An allocated GlobalServer to initialize.
 * @param info An allocated GlobalServerStartupInfo.
 * @return true on success, false otherwise.
 */
bool
GlobalServer_init (
    GlobalServer *self,
    GlobalServerStartupInfo *info
);


/**
 * @brief Initialize an allocated GlobalServerStartupInfo structure.
 * @param self An allocated GlobalServerStartupInfo to initialize.
 * @param confFilePath The configuration file of the Global Server.
 * @return true on success, false otherwise.
 */
bool
GlobalServerStartupInfo_init (
    GlobalServerStartupInfo *self,
    char *confFilePath
);


/**
 * @brief Request to the GlobalServer to flush the Redis Server
 * @param self An allocated GlobalServer
 * @return true on success, false otherwise.
 */
bool
GlobalServer_flushRedis (
    GlobalServer *self
);


/**
 * @brief Free an allocated GlobalServer structure and nullify the content of the pointer.
 * @param self A pointer to an allocated GlobalServer.
 */
void
GlobalServer_destroy (
    GlobalServer **self
);


/**
 * @brief Start the Global Server main loop.
 * @param self An allocated GlobalServer
 * @return true on success, false otherwise
 */
bool
GlobalServer_start (
    GlobalServer *self
);
