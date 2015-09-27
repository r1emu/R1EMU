/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file redis_game_session.h
 * @brief GameSession fields definition
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#pragma once

#include "R1EMU.h"
#include "common/redis/redis.h"
#include "common/session/session.h"

// accessors helpers
// Account
#define COPY_REDIS_ACCOUNT_STR(_str, _x) strncpy(_str, reply->element[REDIS_ACCOUNT_SESSION_##_x]->str, sizeof(_str));
#define GET_REDIS_ACCOUNT_32(_x) strtoul(reply->element[REDIS_ACCOUNT_SESSION_##_x]->str, NULL, 16)
#define GET_REDIS_ACCOUNT_64(_x) strtoull(reply->element[REDIS_ACCOUNT_SESSION_##_x]->str, NULL, 16)
#define GET_REDIS_ACCOUNT_FLOAT(_x) strtof (reply->element[REDIS_ACCOUNT_SESSION_##_x]->str, NULL)

// Commander
#define COPY_REDIS_COMMANDER_STR(_str, _x) strncpy(_str, reply->element[REDIS_COMMANDER_SESSION_##_x]->str, sizeof(_str));
#define GET_REDIS_COMMANDER_32(_x) strtoul(reply->element[REDIS_COMMANDER_SESSION_##_x]->str, NULL, 16)
#define GET_REDIS_COMMANDER_64(_x) strtoull(reply->element[REDIS_COMMANDER_SESSION_##_x]->str, NULL, 16)
#define GET_REDIS_COMMANDER_FLOAT(_x) strtof (reply->element[REDIS_COMMANDER_SESSION_##_x]->str, NULL)

// Equipment
#define COPY_REDIS_EQUIPMENT_STR(_str, _x) strncpy(_str, reply->element[REDIS_EQUIPMENT_SESSION_##_x]->str, sizeof(_str));
#define GET_REDIS_EQUIPMENT_32(_x) strtoul(reply->element[REDIS_EQUIPMENT_SESSION_##_x]->str, NULL, 16)
#define GET_REDIS_EQUIPMENT_64(_x) strtoull(reply->element[REDIS_EQUIPMENT_SESSION_##_x]->str, NULL, 16)
#define GET_REDIS_EQUIPMENT_FLOAT(_x) strtof (reply->element[REDIS_EQUIPMENT_SESSION_##_x]->str, NULL)

/// String fields

// Account Session
#define REDIS_ACCOUNT_SESSION_accountName_str        "account_accountName"
#define REDIS_ACCOUNT_SESSION_sessionKey_str         "account_sessionKey"
#define REDIS_ACCOUNT_SESSION_privilege_str          "account_privilege"
#define REDIS_ACCOUNT_SESSION_commandersCountMax_str "account_commandersCountMax"

// Commander Session
#define REDIS_COMMANDER_SESSION_mapId_str           "commander_mapId"
#define REDIS_COMMANDER_SESSION_commanderName_str   "commander_commanderName"
#define REDIS_COMMANDER_SESSION_familyName_str      "commander_familyName"
#define REDIS_COMMANDER_SESSION_accountId_str       "commander_accountId"
#define REDIS_COMMANDER_SESSION_classId_str         "commander_classId"
#define REDIS_COMMANDER_SESSION_jobId_str           "commander_jobId"
#define REDIS_COMMANDER_SESSION_gender_str          "commander_gender"
#define REDIS_COMMANDER_SESSION_level_str           "commander_level"
#define REDIS_COMMANDER_SESSION_hairId_str          "commander_hairId"
#define REDIS_COMMANDER_SESSION_pose_str            "commander_pose"
#define REDIS_COMMANDER_SESSION_posX_str            "commander_posX"
#define REDIS_COMMANDER_SESSION_posY_str            "commander_posY"
#define REDIS_COMMANDER_SESSION_posZ_str            "commander_posZ"
#define REDIS_COMMANDER_SESSION_currentXP_str       "commander_currentXP"
#define REDIS_COMMANDER_SESSION_maxXP_str           "commander_maxXP"
#define REDIS_COMMANDER_SESSION_pcId_str            "commander_pcId"
#define REDIS_COMMANDER_SESSION_socialInfoId_str    "commander_socialInfoId"
#define REDIS_COMMANDER_SESSION_commanderId_str     "commander_commanderId"
#define REDIS_COMMANDER_SESSION_currentHP_str       "commander_currentHP"
#define REDIS_COMMANDER_SESSION_maxHP_str           "commander_maxHP"
#define REDIS_COMMANDER_SESSION_currentSP_str       "commander_currentSP"
#define REDIS_COMMANDER_SESSION_maxSP_str           "commander_maxSP"
#define REDIS_COMMANDER_SESSION_currentStamina_str  "commander_currentStamina"
#define REDIS_COMMANDER_SESSION_maxStamina_str      "commander_maxStamina"
#define REDIS_COMMANDER_SESSION_head_top_str        "commander_head_top"
#define REDIS_COMMANDER_SESSION_head_middle_str     "commander_head_middle"
#define REDIS_COMMANDER_SESSION_itemUnk1_str        "commander_itemUnk1"
#define REDIS_COMMANDER_SESSION_body_armor_str      "commander_body_armor"
#define REDIS_COMMANDER_SESSION_gloves_str          "commander_gloves"
#define REDIS_COMMANDER_SESSION_boots_str           "commander_boots"
#define REDIS_COMMANDER_SESSION_helmet_str          "commander_helmet"
#define REDIS_COMMANDER_SESSION_bracelet_str        "commander_bracelet"
#define REDIS_COMMANDER_SESSION_weapon_str          "commander_weapon"
#define REDIS_COMMANDER_SESSION_shield_str          "commander_shield"
#define REDIS_COMMANDER_SESSION_costume_str         "commander_costume"
#define REDIS_COMMANDER_SESSION_itemUnk3_str        "commander_itemUnk3"
#define REDIS_COMMANDER_SESSION_itemUnk4_str        "commander_itemUnk4"
#define REDIS_COMMANDER_SESSION_itemUnk5_str        "commander_itemUnk5"
#define REDIS_COMMANDER_SESSION_leg_armor_str       "commander_leg_armor"
#define REDIS_COMMANDER_SESSION_itemUnk6_str        "commander_itemUnk6"
#define REDIS_COMMANDER_SESSION_itemUnk7_str        "commander_itemUnk7"
#define REDIS_COMMANDER_SESSION_ring_left_str       "commander_ring_left"
#define REDIS_COMMANDER_SESSION_ring_right_str      "commander_ring_right"
#define REDIS_COMMANDER_SESSION_necklace_str        "commander_necklace"


enum RedisAccountSessionFields {
    // Account session
    REDIS_ACCOUNT_SESSION_accountName,
    REDIS_ACCOUNT_SESSION_sessionKey,
    REDIS_ACCOUNT_SESSION_privilege,
    REDIS_ACCOUNT_SESSION_commandersCountMax,
};

enum RedisCommanderSessionFields {
    // Commander session
    REDIS_COMMANDER_SESSION_mapId,
    REDIS_COMMANDER_SESSION_commanderName,
    REDIS_COMMANDER_SESSION_familyName,
    REDIS_COMMANDER_SESSION_accountId,
    REDIS_COMMANDER_SESSION_classId,
    REDIS_COMMANDER_SESSION_jobId,
    REDIS_COMMANDER_SESSION_gender,
    REDIS_COMMANDER_SESSION_level,
    REDIS_COMMANDER_SESSION_hairId,
    REDIS_COMMANDER_SESSION_pose,
    REDIS_COMMANDER_SESSION_posX,
    REDIS_COMMANDER_SESSION_posY,
    REDIS_COMMANDER_SESSION_posZ,
    REDIS_COMMANDER_SESSION_currentXP,
    REDIS_COMMANDER_SESSION_maxXP,
    REDIS_COMMANDER_SESSION_pcId,
    REDIS_COMMANDER_SESSION_socialInfoId,
    REDIS_COMMANDER_SESSION_commanderId,
    REDIS_COMMANDER_SESSION_currentHP,
    REDIS_COMMANDER_SESSION_maxHP,
    REDIS_COMMANDER_SESSION_currentSP,
    REDIS_COMMANDER_SESSION_maxSP,
    REDIS_COMMANDER_SESSION_currentStamina,
    REDIS_COMMANDER_SESSION_maxStamina,
    REDIS_COMMANDER_SESSION_head_top,
    REDIS_COMMANDER_SESSION_head_middle,
    REDIS_COMMANDER_SESSION_itemUnk1,
    REDIS_COMMANDER_SESSION_body_armor,
    REDIS_COMMANDER_SESSION_gloves,
    REDIS_COMMANDER_SESSION_boots,
    REDIS_COMMANDER_SESSION_helmet,
    REDIS_COMMANDER_SESSION_bracelet,
    REDIS_COMMANDER_SESSION_weapon,
    REDIS_COMMANDER_SESSION_shield,
    REDIS_COMMANDER_SESSION_costume,
    REDIS_COMMANDER_SESSION_itemUnk3,
    REDIS_COMMANDER_SESSION_itemUnk4,
    REDIS_COMMANDER_SESSION_itemUnk5,
    REDIS_COMMANDER_SESSION_leg_armor,
    REDIS_COMMANDER_SESSION_itemUnk6,
    REDIS_COMMANDER_SESSION_itemUnk7,
    REDIS_COMMANDER_SESSION_ring_left,
    REDIS_COMMANDER_SESSION_ring_right,
    REDIS_COMMANDER_SESSION_necklace,
};

typedef struct {
    uint16_t routerId;
    uint16_t mapId;
    uint64_t accountId;
} RedisGameSessionKey;

extern const char *redisGameSessionsStr [];


/**
 * @brief Get the GameSession associated with the SocketSession
 * @param self An allocated Redis
 * @param key The GameSession key
 * @param[out] gameSession The output gameSession
 * @return true on success, false otherwise
 */
bool redisGetGameSession(Redis *self, RedisGameSessionKey *key, GameSession *gameSession);

/**
 * @brief Save an entire GameSession to the Redis server.
 * @param self An allocated Redis instance
 * @param key The GameSession key
 * @param socketId The socketId linked with the Game Session
 * @param gameSession The Game Session to save
 * @return true on success, false otherwise
 */
bool redisUpdateGameSession(Redis *self, RedisGameSessionKey *key, uint8_t *socketId, GameSession *gameSession);

/**
 * @brief Flush a GameSession
 * @param self An allocated Redis instance
 * @param key The GameSession key
 * @return true on success, false otherwise
 */
bool redisDeleteGameSession(Redis *self, RedisGameSessionKey *key);

/**
 * @brief Move a game session from a server to another
 * @param self An initialized Redis instance
 * @param from The source server
 * @param to The destination server
 */
bool redisMoveGameSession(Redis *self, RedisGameSessionKey *from, RedisGameSessionKey *to);

/**
 * @brief Return a list of clients into the area of a given position
 * @param self An allocated Worker
 * @param serverId The serverId concerned
 * @param mapId The mapId of the position
 * @param The 2D position of the center of the circle
 * @param range Radius of the circle
 * @param socketIdNoInclude if not NULL, don't include this socketId to the result
 * @return a zlist_t of identity keys
 */
zlist_t *redisGetClientsWithinDistance(
    Redis *self,
    uint16_t serverId,
    uint16_t mapId,
    PositionXZ *position,
    float range,
    char *socketIdNoInclude);

/**
 * @brief Get the GameSession by socket Id
 * @param self An allocated Redis
 * @param routerId The router ID containing the target session
 * @param socketId The socket ID of the target session
 * @param[out] gameSession The output gameSession
 * @return true on success, false otherwise
 */
// bool redisGetGameSessionBySocketId(Redis *self, uint16_t routerId, uint8_t *socketId, GameSession *gameSession);
