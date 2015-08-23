/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file SocialServer.h
 * @brief SocialServer controls and processes the client social events
 *
 * @license <license placeholder>
 */

#pragma once

// ---------- Includes ------------
#include "R1EMU.h"
#include "Common/Server/Server.h"

// ---------- Defines -------------
// Configuration default values
#define SOCIAL_SERVER_PORTS_DEFAULT               (char []) {"1337 1338"}
#define SOCIAL_SERVER_WORKERS_COUNT_DEFAULT       1


// ------ Structure declaration -------
// SocialServer is opaque
typedef struct SocialServer SocialServer;

// ----------- Functions ------------

/**
 * @brief Allocate a new SocialServer structure.
 * @param server An initialized server
 * @return A pointer to an allocated SocialServer.
 */
SocialServer *
SocialServer_new (
    Server *server
);


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
bool
SocialServer_init (
    SocialServer *self,
    Server *server
);


/**
 * @brief Free an allocated SocialServer structure and nullify the content of the pointer.
 * @param self A pointer to an allocated SocialServer.
 */
void
SocialServer_destroy (
    SocialServer **self
);

/**
 * @brief Start the Social Server main loop.
 * @param self An allocated SocialServer
 * @return always NULL
 */
bool
SocialServer_start (
    SocialServer *self
);
