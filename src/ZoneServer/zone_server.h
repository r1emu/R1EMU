/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file zone_server.h
 * @brief ZoneServer controls and processes the client actions
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#pragma once

#include "R1EMU.h"
#include "common/server/server.h"

#define ZONE_SERVER_EXECUTABLE_NAME         "ZoneServer"

// configuration default values
#define ZONE_SERVER_PORTS_DEFAULT           (char []) {"2004"}
#define ZONE_SERVER_WORKERS_COUNT_DEFAULT   1

// ZoneServer is opaque
typedef struct ZoneServer ZoneServer;

/**
 * @brief Allocate a new ZoneServer structure.
 * @param server An initialized server
 * @return A pointer to an allocated ZoneServer.
 */
ZoneServer *zoneServerNew(Server *server);

/**
 * @brief Initialize an allocated ZoneServer structure.
 * @param self An allocated ZoneServer to initialize.
 * @param server An initialized server
 * @return true on success, false otherwise.
 */
bool zoneServerInit(ZoneServer *self, Server *server);

/**
 * @brief Free an allocated ZoneServer structure and nullify the content of the pointer.
 * @param self A pointer to an allocated ZoneServer.
 */
void zoneServerDestroy(ZoneServer **self);

/**
 * @brief Start the Zone Server main loop.
 * @param self An allocated ZoneServer
 * @return true on success, false otherwise
 */
bool zoneServerStart(ZoneServer *self);
