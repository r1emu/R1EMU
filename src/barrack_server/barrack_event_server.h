/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file barrack_event_server.h
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
#include "barrack_handler/barrack_event_handler.h"
#include "common/server/event_server.h"
#include "common/server/game_event.h"
#include "common/db/db_client.h"

// ---------- Defines -------------

// ------ Structure declaration -------

// ----------- Functions ------------
/**
 * @brief Process the barrack events received from the workers
 */
bool barrackEventServerProcess(EventServer *self, EventType type, void *eventData);

/**
 * @brief Event handler when a client disconnects
 */
bool barrackEventServerOnDisconnect (
    zsock_t *eventServer,
    DbClient *dbSession,
    Redis *redis,
    MySQL *mysql,
    uint16_t routerId,
    uint8_t *sessionKeyStr
);
