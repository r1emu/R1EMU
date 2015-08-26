/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file barrack_server.h
 * @brief BarrackServer controls and processes the client actions
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#pragma once

#include "R1EMU.h"
#include "common/server/server.h"

#define BARRACK_SERVER_EXECUTABLE_NAME             "BarrackServer"

// configuration default values
#define BARRACK_SERVER_PORTS_DEFAULT               (char []) {"2000"}
#define BARRACK_SERVER_WORKERS_COUNT_DEFAULT       1
#define BARRACK_SERVER_FRONTEND_IP_DEFAULT         (char []) {"127.0.0.1"}

// BarrackServer is opaque
typedef struct BarrackServer BarrackServer;

/**
 * @brief Allocate a new BarrackServer structure.
 * @param server An initialized server
 * @return A pointer to an allocated BarrackServer.
 */
BarrackServer *barrackServerNew(Server *server);

/**
 * @brief Initialize an allocated BarrackServer structure.
 * @param self An allocated BarrackServer to initialize.
 * @param server An initialized server
 * @return true on success, false otherwise.
 */
bool barrackServerInit(BarrackServer *self, Server *server);

/**
 * @brief Free an allocated BarrackServer structure and nullify the content of the pointer.
 * @param self A pointer to an allocated BarrackServer.
 */
void barrackServerDestroy(BarrackServer **self);

/**
 * @brief Start the barrack Server main loop.
 * @param self An allocated BarrackServer
 * @return always NULL
 */
bool barrackServerStart(BarrackServer *self);
