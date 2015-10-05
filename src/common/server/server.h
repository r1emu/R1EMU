/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file server.h
 * @brief
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#pragma once

#include "R1EMU.h"
#include "router.h"
#include "worker.h"

// Routers IDs
#define ZONES_SERVER_ROUTER_ID       0
#define BARRACKS_SERVER_ROUTER_ID    1000
#define SOCIALS_SERVER_ROUTER_ID     2000

typedef struct Server Server;

typedef struct {
    RouterInfo routerInfo;
    WorkerInfo *workersInfo;
    int workersInfoCount;
    char *output;
    ServerType serverType;
} ServerInfo;

/**
 * @brief Allocate a new Server structure.
 * @param info An allocated ServerInfo already initialized
 * @return A pointer to an allocated Server, or NULL if an error occurred.
 */
Server *serverNew(ServerInfo *info);

/**
 * @brief Initialize an allocated Server structure.
 * @param self An allocated Server to initialize.
 * @param info An allocated ServerInfo already initialized
 * @return true on success, false otherwise.
 */
bool serverInit(Server *self, ServerInfo *info);

/**
 * @brief Initialize an allocated ServerInfo structure.
 * @param self An allocated ServerInfo to initialize.
 * @param serverType The type of the server to initialize
 * @param routerInfo An allocated RouterInfo already initialized
 * @param workersInfo An allocated WorkerInfo array all already initialized
 * @param workersInfoCount The workersInfo elements count.
 * @return true on success, false otherwise.
 */
bool serverInfoInit(
    ServerInfo *self,
    ServerType serverType,
    RouterInfo *routerInfo,
    WorkerInfo *workersInfo,
    int workersInfoCount,
    char *output);

/**
 * @brief Start a new Server
 * @param self An allocated Server to start
 * @return true on success, false otherwise.
 */
bool serverStart(Server *self);

/**
 * @brief Launch a new Server process
 * @param info An allocated ServerInfo already initialized
 * @param executableName The executable name to launch
 * @return true on success, false otherwise.
 */
bool serverCreateProcess(ServerInfo *info, char *executableName);

/**
 * @brief Get the routerId of the Server
 * @param self A pointer to an allocated Server.
 * @return The server ID.
 */
uint16_t serverGetRouterId(Server *self);

/**
 * @brief Get the MySQL information of the Server
 * @param self A pointer to an allocated Server.
 * @return The server MySQL information.
 */
MySQLInfo *serverGetMySQLInfo(Server *self);

/**
 * @brief Free an allocated Server structure.
 * @param self A pointer to an allocated Server.
 */
void serverFree(Server *self);

/**
 * @brief Free an allocated Server structure and nullify the content of the pointer.
 * @param self A pointer to an allocated Server.
 */
void serverDestroy(Server **self);
