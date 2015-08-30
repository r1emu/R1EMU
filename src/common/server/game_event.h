/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file game_event.h
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
#include "common/session/socket_session.h"
#include "common/server/event_server.h"

#include "zone_server/zone_handler/zone_event.h"
#include "barrack_server/barrack_handler/barrack_event.h"

// ---------- Defines -------------


// ------ Structure declaration -------
typedef union {
    ZoneEvent zone;
    BarrackEvent barrack;
    // SocialEvent social;
} EventDataCategories;

/**
 * @brief GameEvent contains a generic structure concerning all the events
 */
typedef struct {
    // Session key of the client
    uint8_t emitterSk[SOCKET_SESSION_ID_SIZE];
    // Type of the event
    EventType type;
    // Event data
    EventDataCategories data;
} GameEvent;

// ----------- Functions ------------

GameEvent *gameEventNew(uint8_t *emitterSk, EventType type, void *event, size_t eventSize);

bool gameEventInit(GameEvent *self, uint8_t *emitterSk, EventType type, void *event, size_t eventSize);

void gameEventFree(GameEvent *self);

void gameEventDestroy(GameEvent **self);

