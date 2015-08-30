/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file server_factory.h
 * @brief
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#pragma once

#include "R1EMU.h"
#include "common/server/server.h"

/**
 * @brief : Factory for creating a generic Server
 */
Server *serverFactoryCreateServer(
    ServerType serverType,
    uint16_t routerId,
    char *routerIp,
    int port,
    int workersCount,
    char *globalServerIp,
    int globalServerPort,
    char *sqlHostname,
    char *sqlUsername,
    char *sqlPassword,
    char *sqlDatabase,
    char *redisHostname,
    int redisPort);

/**
 * @brief : Factory for initializing a generic Server
 */
bool serverFactoryInitServerInfo(
    ServerStartupInfo *serverInfo,
    ServerType serverType,
    uint16_t routerId,
    char *routerIp,
    int port,
    int workersCount,
    char *globalServerIp,
    int globalServerPort,
    char *sqlHostname,
    char *sqlUsername,
    char *sqlPassword,
    char *sqlDatabase,
    char *redisHostname,
    int redisPort);
