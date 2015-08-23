/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file EventHandler.h
 * @brief
 *
 *
 *
 * @license <license placeholder>
 */

#pragma once

// ---------- Includes ------------
#include "R1EMU.h"
#include "EventServer.h"

// ---------- Defines -------------
#define COMMANDER_RANGE_AROUND 250.0


// ------ Structure declaration -------
typedef struct {
    GameEventUpdatePosition updatePosEvent;
    PositionXYZ position;
    PositionXZ direction;
    float timestamp;
} GameEventCommanderMove;

typedef struct {
    GameEventUpdatePosition updatePosEvent;
    PositionXYZ position;
    PositionXZ direction;
    float timestamp;
} GameEventMoveStop;

typedef struct {
    uint32_t pcId;
    char sessionKey [SOCKET_SESSION_ID_SIZE];
} GameEventRestSit;

typedef struct {
    GameEventUpdatePosition updatePosEvent;
    float height;
} GameEventJump;

typedef struct {
    GameEventUpdatePosition updatePosEvent;
} GameEventPcEnter;

typedef struct {
    uint32_t pcId;
    char sessionKey [SOCKET_SESSION_ID_SIZE];
    uint8_t familyName [COMMANDER_FAMILY_NAME_SIZE];
    uint8_t commanderName [COMMANDER_NAME_SIZE];
    uint8_t chatText [0]; // Variable length array
} GameEventChat;


// ----------- Functions ------------

bool
EventHandler_commanderMove (
    EventServer *self,
    GameEventCommanderMove *eventData
);

bool
EventHandler_moveStop (
    EventServer *self,
    GameEventMoveStop *eventData
);

bool
EventHandler_chat (
    EventServer *self,
    GameEventChat *event
);

bool
EventHandler_restSit (
    EventServer *self,
    GameEventRestSit *eventData
);

bool
EventHandler_enterPc (
    EventServer *self,
    GameEventPcEnter *event
);

bool
EventHandler_jump (
    EventServer *self,
    GameEventJump *event
);
