/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file barrack_builder.h
 * @brief barrack_builder contains the packet builders.
 *        In other words, it contains code that builds packet after a request from a client.
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#pragma once

#include "R1EMU.h"
#include "common/commander/commander.h"
#include "common/session/game_session.h"
#include "common/server/worker.h"

/**
 * BC_NORMAL packets subtypes
 */
typedef enum PacketTypeBarrackNormal {

    BC_NORMAL_COMMANDER_INFO = 0x0,
    BC_NORMAL_COMMANDER_MOVE_OK = 0x2,
    BC_NORMAL_UNKNOWN_1 = 0x4,
    BC_NORMAL_PET_INFORMATION = 0x8,
    BC_NORMAL_ZONE_TRAFFIC = 0xB,
    BC_NORMAL_POST_BOX_MESSAGE = 0xF,

} PacketTypeBarrackNormal;

/**
 * BC_BARRACKNAME_CHANGE result types
 */
typedef enum BarrackNameChangeStatus {

    BC_BARRACKNAME_CHANGE_ALREADYEXIST = -1,
    BC_BARRACKNAME_CHANGE_OK = 0,
    BC_BARRACKNAME_CHANGE_ERROR = 1,

} BarrackNameChangeStatus;

/**
 * @brief Send message to client
 */
void barrackBuilderMessage(uint8_t msgType, uint8_t *message, zmsg_t *replyMsg);

/**
 * @brief Send back the information of the account after a successful log in
 */
void barrackBuilderLoginOk(
    uint64_t accountId,
    uint8_t *accountName,
    uint8_t *sessionKey,
    AccountSessionPrivileges accountPrivileges,
    zmsg_t *replyMsg
);

/**
 * @brief Packet sent when the request for the player to enter in game (AKA connect to a given zone server) is accepted
 */
void barrackBuilderStartGameOk(
    uint32_t zoneServerId,
    uint32_t zoneServerIp,
    uint32_t zoneServerPort,
    MapId_t mapId,
    uint8_t commanderListId,
    SocialInfoId_t socialInfoId,
    uint8_t isSingleMap,
    zmsg_t *replyMsg
);

/**
 * @brief The request for the player to move in the barrack is accepted
 */
void barrackBuilderCommanderMoveOk(
    uint64_t accountId,
    uint16_t commanderListId,
    PositionXYZ *position,
    zmsg_t *replyMsg
);

/**
 * @brief Register new servers
 */
void barrackBuilderServerEntry(
    uint32_t ipClientNet,
    uint32_t ipVirtualClientNet,
    uint16_t channelPort1,
    uint16_t channelPort2,
    zmsg_t *replyMsg
);

/**
 * @brief Build the list of commanders in the barrack
 */
void barrackBuilderCommanderList(
    uint64_t accountId,
    GameSession *gameSession,
    Commander **commanders,
    int commandersCount,
    zmsg_t *replyMsg);

/**
 * @brief Build the list of zone servers
 */
void barrackBuilderZoneTraffics(MapId_t mapId, zmsg_t *replyMsg);

/**
 * @brief Change the name of the barrack
 */
void
barrackBuilderBarrackNameChange(BarrackNameChangeStatus resultType, uint8_t *barrackName, zmsg_t *replyMsg);

/**
 * @brief Destroy commanders
 */
void
barrackBuilderCommanderDestroy(uint8_t commanderDestroyMask, zmsg_t *replyMsg);

/**
 * @brief Create a new commander
 */
void barrackBuilderCommanderCreate(Commander *commander, uint8_t commandersCount, zmsg_t *replyMsg);

/**
 * @brief Send information about the pets of the account
 */
void barrackBuilderPetInformation(zmsg_t *reply);

/**
 * @brief @unknown
 */
void barrackBuilderIesModifyList(zmsg_t *replyMsg);

/**
 * @brief @unknown ; Makes the character selection appear directly without asking for the family name
 */
void barrackBuilderNormalUnk1(uint64_t accountId, zmsg_t *replyMsg);

/**
 * @brief Logout
 */
void barrackBuilderLogoutOk(zmsg_t *replyMsg);
