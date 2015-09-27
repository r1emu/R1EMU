/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file social_event_server.h
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
#include "common/server/event_server.h"
#include "common/server/game_event.h"
#include "social_handler/social_event_handler.h"

// ---------- Defines -------------

// ------ Structure declaration -------

// ----------- Functions ------------
/**
 * @brief Process the social events received from the workers
 */
bool socialEventServerProcess(EventServer *self, EventType type, void *eventData);

/**
 * @brief Event handler when a client disconnects
 */
bool socialEventServerOnDisconnect (
    zsock_t *eventServer,
    DbClient *dbSession,
    Redis *redis,
    MySQL *mysql,
    uint16_t routerId,
    uint8_t *sessionKeyStr
);
