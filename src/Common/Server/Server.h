/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file Server.h
 * @brief
 *
 *
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#pragma once

// ---------- Includes ------------
#include "R1EMU.h"
#include "Router.h"
#include "Worker.h"

// ---------- Defines -------------
/** Router unique IDs */
#define BARRACK_SERVER_ROUTER_ID ((uint16_t) -1)
#define SOCIAL_SERVER_ROUTER_ID  ((uint16_t) -2)

// ------ Structure declaration -------
typedef struct Server Server;

typedef struct {
    RouterStartupInfo routerInfo;
    WorkerStartupInfo *workersInfo;
    int workersInfoCount;
    ServerType serverType;
}   ServerStartupInfo;


// ----------- Functions ------------

/**
 * @brief Allocate a new Server structure.
 * @param info An allocated ServerStartupInfo already initialized
 * @return A pointer to an allocated Server, or NULL if an error occured.
 */
Server *
Server_new (
    ServerStartupInfo *info
);


/**
 * @brief Initialize an allocated Server structure.
 * @param self An allocated Server to initialize.
 * @param info An allocated ServerStartupInfo already initialized
 * @return true on success, false otherwise.
 */
bool
Server_init (
    Server *self,
    ServerStartupInfo *info
);


/**
 * @brief Initialize an allocated ServerStartupInfo structure.
 * @param self An allocated ServerStartupInfo to initialize.
 * @param serverType The type of the server to initialize
 * @param routerInfo An allocated RouterStartupInfo already initialized
 * @param workersInfo An allocated WorkerStartupInfo array all already initialized
 * @param workersInfoCount The workersInfo elements count.
 * @return true on success, false otherwise.
 */
bool
ServerStartupInfo_init (
    ServerStartupInfo *self,
    ServerType serverType,
    RouterStartupInfo *routerInfo,
    WorkerStartupInfo *workersInfo,
    int workersInfoCount
);


/**
 * @brief Start a new Server
 * @param self An allocated Server to start
 * @return true on success, false otherwise.
 */
bool
Server_start (
    Server *self
);


/**
 * @brief Launch a new Server process
 * @param info An allocated ServerStartupInfo already initialized
 * @param executableName The executable name to launch
 * @return true on success, false otherwise.
 */
bool
Server_createProcess (
    ServerStartupInfo *info,
    char *executableName
);

/**
 * @brief Get the routerId of the Server
 * @param self A pointer to an allocated Server.
 * @return The Router ID.
 */
uint16_t
Server_getRouterId (
    Server *self
);

/**
 * @brief Free an allocated Server structure.
 * @param self A pointer to an allocated Server.
 */
void
Server_free (
    Server *self
);

/**
 * @brief Free an allocated Server structure and nullify the content of the pointer.
 * @param self A pointer to an allocated Server.
 */
void
Server_destroy (
    Server **self
);

