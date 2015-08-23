/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file ServerFactory.h
 * @brief
 *
 *
 *
 * @license <license placeholder>
 */

#pragma once

// ---------- Includes ------------
#include "R1EMU.h"
#include "Common/Server/Server.h"

// ---------- Defines -------------

// ------ Structure declaration -------

// ----------- Functions ------------

/**
 * @brief : Factory for creating a generic Server
 */
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
);


/**
 * @brief : Factory for initializing a generic Server
 */
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
);
