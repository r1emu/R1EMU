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

// router unique IDs
#define BARRACK_SERVER_ROUTER_ID((uint16_t) -1)
#define SOCIAL_SERVER_ROUTER_ID ((uint16_t) -2)

typedef struct Server Server;

typedef struct {
    RouterStartupInfo routerInfo;
    WorkerStartupInfo *workersInfo;
    int workersInfoCount;
    ServerType serverType;
} ServerStartupInfo;

/**
 * @brief Allocate a new Server structure.
 * @param info An allocated ServerStartupInfo already initialized
 * @return A pointer to an allocated Server, or NULL if an error occured.
 */
Server *serverNew(ServerStartupInfo *info);

/**
 * @brief Initialize an allocated Server structure.
 * @param self An allocated Server to initialize.
 * @param info An allocated ServerStartupInfo already initialized
 * @return true on success, false otherwise.
 */
bool serverInit(Server *self, ServerStartupInfo *info);

/**
 * @brief Initialize an allocated ServerStartupInfo structure.
 * @param self An allocated ServerStartupInfo to initialize.
 * @param serverType The type of the server to initialize
 * @param routerInfo An allocated RouterStartupInfo already initialized
 * @param workersInfo An allocated WorkerStartupInfo array all already initialized
 * @param workersInfoCount The workersInfo elements count.
 * @return true on success, false otherwise.
 */
bool serverStartupInfoInit(
    ServerStartupInfo *self,
    ServerType serverType,
    RouterStartupInfo *routerInfo,
    WorkerStartupInfo *workersInfo,
    int workersInfoCount);

/**
 * @brief Start a new Server
 * @param self An allocated Server to start
 * @return true on success, false otherwise.
 */
bool serverStart(Server *self);

/**
 * @brief Launch a new Server process
 * @param info An allocated ServerStartupInfo already initialized
 * @param executableName The executable name to launch
 * @return true on success, false otherwise.
 */
bool serverCreateProcess(ServerStartupInfo *info, char *executableName);

/**
 * @brief Get the routerId of the Server
 * @param self A pointer to an allocated Server.
 * @return The Router ID.
 */
uint16_t serverGetRouterId(Server *self);

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
