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

#include "R1EMU.h"
#include "event_server.h"

#define COMMANDER_RANGE_AROUND 250.0

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
    char sessionKey[SOCKET_SESSION_ID_SIZE];
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
    char sessionKey[SOCKET_SESSION_ID_SIZE];
    uint8_t familyName[COMMANDER_FAMILY_NAME_SIZE];
    uint8_t commanderName[COMMANDER_NAME_SIZE];
    uint8_t chatText[0]; // variable length array
} GameEventChat;

bool eventHandlerCommanderMove(EventServer *self, GameEventCommanderMove *eventData);

bool eventHandlerMoveStop(EventServer *self, GameEventMoveStop *eventData);

bool eventHandlerChat(EventServer *self, GameEventChat *event);

bool eventHandlerRestSit(EventServer *self, GameEventRestSit *eventData);

bool eventHandlerEnterPc(EventServer *self, GameEventPcEnter *event);

bool eventHandlerJump(EventServer *self, GameEventJump *event);
