/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file zone_event.h
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

// ---------- Defines -------------


// ------ Structure declaration -------
/** Event data needed for a commander to update position */
typedef struct {
    uint16_t mapId;
    CommanderInfo commanderInfo;
    PositionXYZ newPosition;
} GameEventUpdatePosition;

/** Event data needed for a commander to move */
typedef struct {
    GameEventUpdatePosition updatePosEvent;
    PositionXYZ position;
    PositionXZ direction;
    float timestamp;
} GameEventCommanderMove;

/** Event data needed for a commander to stop moving */
typedef struct {
    GameEventUpdatePosition updatePosEvent;
    PositionXYZ position;
    PositionXZ direction;
    float timestamp;
} GameEventMoveStop;

/** Event data needed for a commander to sit */
typedef struct {
    uint32_t pcId;
} GameEventRestSit;

/** Event data needed for a commander to jump */
typedef struct {
    GameEventUpdatePosition updatePosEvent;
    float height;
} GameEventJump;

/** Event data needed for make a commander enter in the zone */
typedef struct {
    GameEventUpdatePosition updatePosEvent;
} GameEventEnterPc;

/** Event data needed for a commander to chat */
typedef struct {
    CommanderInfo commander;
    uint8_t chatText[0]; // variable length array
} GameEventChat;

/** Event data needed for a commander to rotate head */
typedef struct {
    uint32_t pcId;
    PositionXZ direction;
} GameEventHeadRotate;

/** Event data needed for a commander to rotate the body */
typedef struct {
    uint32_t pcId;
    PositionXZ direction;
} GameEventRotate;

/** Union of all events */
typedef union {
    GameEventCommanderMove move;
    GameEventMoveStop moveStop;
    GameEventRestSit restSit;
    GameEventJump jump;
    GameEventEnterPc enterPc;
    GameEventChat chat;
    GameEventHeadRotate headRotate;
    GameEventRotate rotate;
} ZoneEvent;

// ----------- Functions ------------


