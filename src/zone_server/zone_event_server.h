/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file zone_event_server.h
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
#include "zone_handler/zone_event_handler.h"

// ---------- Defines -------------


// ------ Structure declaration -------

// ----------- Functions ------------
/**
 * @brief Process the zone events received from the workers
 */
bool zoneEventServerProcess(EventServer *self, EventType type, void *eventData);

/**
 * @brief Update the position of a client within the event server structures
 * @param self A pointer to an allocated EventServer.
 * @param event A game event
 * @param updatePosEvent Contains needed information for updating a commander position
 * @param newPosition The new position of the client
 * @param[out] redisClientsAround The list of other clients around the client at its new position
 * @return true on success, false otherwise
 */
bool zoneEventServerUpdateClientPosition(
    EventServer *self,
    GameEvent *event,
    GameEventUpdatePosition *updatePosEvent,
    zlist_t **_redisClientsAround);
