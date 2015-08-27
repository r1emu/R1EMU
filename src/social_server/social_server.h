/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file social_server.h
 * @brief SocialServer controls and processes the client social events
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#pragma once

#include "R1EMU.h"
#include "common/server/server.h"

// configuration default values
#define SOCIAL_SERVER_PORTS_DEFAULT             (char []) {"1337 1338"}
#define SOCIAL_SERVER_WORKERS_COUNT_DEFAULT     1

typedef struct SocialServer SocialServer;

/**
 * @brief Allocate a new SocialServer structure.
 * @param server An initialized server
 * @return A pointer to an allocated SocialServer.
 */
SocialServer *socialServerNew(Server *server);

/**
 * @brief Initialize an allocated SocialServer structure.
 * @param self An allocated SocialServer to initialize.
 * @param socialZoneServerId The social server ID
 * @param serverIp The IP of the social server
 * @param frontendPort The social server port opened to the internet
 * @param workersCount Count of worker per social server
 * @param confFilePath The configuration file path
 * @return true on success, false otherwise.
 */
bool socialServerInit(SocialServer *self, Server *server);

/**
 * @brief Free an allocated SocialServer structure and nullify the content of the pointer.
 * @param self A pointer to an allocated SocialServer.
 */
void socialServerDestroy(SocialServer **self);

/**
 * @brief Start the Social Server main loop.
 * @param self An allocated SocialServer
 * @return always NULL
 */
bool socialServerStart(SocialServer *self);
