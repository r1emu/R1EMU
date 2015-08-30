/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file event_handler.h
 * @brief
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#pragma once

// ---------- Includes ------------
#include "R1EMU.h"
#include "event_server.h"
#include "game_event.h"

// ---------- Defines -------------
#define COMMANDER_RANGE_AROUND 250.0

// ------ Structure declaration -------

/**
 * @brief EventHandlerFunction takes an event and handle it
 */
typedef bool (*EventHandlerFunction)(
    // a pointer to the current EventServer
    EventServer *self,
    // event received
    GameEvent *event
);

/**
 * @brief EventHandler contains a function handler and its name
 */
typedef struct {
    // Function handler
    EventHandlerFunction handler;

    // The packet name
    char *eventName;
}   EventHandler;

