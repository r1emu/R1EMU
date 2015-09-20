/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file barrack_handler.h
 * @brief BarrackHandler is a module containing all the handlers for the Barrack system
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#pragma once

#include "R1EMU.h"
#include "common/packet/packet_type.h"
#include "common/server/worker.h"

/**
 * Global table of handlers for the Barrack. It contains only the packets allowed.
 */
extern const PacketHandler barrackHandlers[PACKET_TYPE_COUNT];

/**
 * BC_MESSAGE messages
 */
typedef enum PacketMessageTypes {

    BC_MESSAGE_CUSTOM_MSG = 0,
    BC_MESSAGE_USER_PASS_INCORRECT_1 = 1,
    BC_MESSAGE_USER_PASS_INCORRECT_2 = 2,
    BC_MESSAGE_ALREADY_LOGGEDIN = 3,
    BC_MESSAGE_WRONG_CODE = 4,
    BC_MESSAGE_VERSION_MISSMATCH = 5,
    BC_MESSAGE_NAME_ALREADY_EXIST = 6,
    BC_MESSAGE_CANT_CREATE_COMMANDER = 7,
    BC_MESSAGE_COMMANDER_NAME_TOO_SHORT = 8,
    BC_MESSAGE_CANNOT_DEL_COMMANDER_1 = 9,
    BC_MESSAGE_CREATE_COMMANDER_FAIL = 10,
    BC_MESSAGE_COMMANDER_NAME_TOO_LONG = 11,
    BC_MESSAGE_CANNOT_DEL_COMMANDER_2 = 12,
    BC_MESSAGE_TEAM_NOT_EXIST = 13,
    BC_MESSAGE_COMMANDER_NOT_EXIST = 14,
    BC_MESSAGE_CANT_START_GAME = 15,
    BC_MESSAGE_NEW_JOB_ADDED = 16,
    BC_MESSAGE_NOTUSED_1 = 17,
    BC_MESSAGE_TEAM_CAP_EXCEEDED = 18,
    BC_MESSAGE_NEED_MORE_COMM_POITS = 19,
    BC_MESSAGE_ACCOUNT_BLOCKED_1 = 20,
    BC_MESSAGE_ACCOUNT_BLOCKED_2 = 21,
    BC_MESSAGE_INVENTORY_FULL = 22,
    BC_MESSAGE_STARTING_MAP_NOT_AVAIL = 23,
    BC_MESSAGE_CANT_DELETE_PARTY_LEADER = 24,
    BC_MESSAGE_NOT_CBT_USER = 25,
    BC_MESSAGE_RATED_ESRB_TEEN = 26,

} PacketMessageTypes;
