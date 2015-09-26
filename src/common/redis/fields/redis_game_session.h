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

// Game session
#define REDIS_SESSION_account_accountName_str        "account_accountName"
#define REDIS_SESSION_account_sessionKey_str         "account_sessionKey"
#define REDIS_SESSION_account_privilege_str          "account_privilege"
#define REDIS_SESSION_account_commandersCountMax_str "commandersCountMax"

// CommanderInfo session
#define REDIS_SESSION_commander_mapId_str           "commander_mapId"
#define REDIS_SESSION_commander_commanderName_str   "commander_commanderName"
#define REDIS_SESSION_commander_familyName_str      "commander_familyName"
#define REDIS_SESSION_commander_accountId_str       "commander_accountId"
#define REDIS_SESSION_commander_classId_str         "commander_classId"
#define REDIS_SESSION_commander_jobId_str           "commander_jobId"
#define REDIS_SESSION_commander_gender_str          "commander_gender"
#define REDIS_SESSION_commander_level_str           "commander_level"
#define REDIS_SESSION_commander_hairId_str          "commander_hairId"
#define REDIS_SESSION_commander_pose_str            "commander_pose"

// CommanderEquipment
#define REDIS_SESSION_equipment_head_top_str        "equipment_head_top"
#define REDIS_SESSION_equipment_head_middle_str     "equipment_head_middle"
#define REDIS_SESSION_equipment_itemUnk1_str        "equipment_itemUnk1"
#define REDIS_SESSION_equipment_body_armor_str      "equipment_body_armor"
#define REDIS_SESSION_equipment_gloves_str          "equipment_gloves"
#define REDIS_SESSION_equipment_boots_str           "equipment_boots"
#define REDIS_SESSION_equipment_helmet_str          "equipment_helmet"
#define REDIS_SESSION_equipment_bracelet_str        "equipment_bracelet"
#define REDIS_SESSION_equipment_weapon_str          "equipment_weapon"
#define REDIS_SESSION_equipment_shield_str          "equipment_shield"
#define REDIS_SESSION_equipment_costume_str         "equipment_costume"
#define REDIS_SESSION_equipment_itemUnk3_str        "equipment_itemUnk3"
#define REDIS_SESSION_equipment_itemUnk4_str        "equipment_itemUnk4"
#define REDIS_SESSION_equipment_itemUnk5_str        "equipment_itemUnk5"
#define REDIS_SESSION_equipment_leg_armor_str       "equipment_leg_armor"
#define REDIS_SESSION_equipment_itemUnk6_str        "equipment_itemUnk6"
#define REDIS_SESSION_equipment_itemUnk7_str        "equipment_itemUnk7"
#define REDIS_SESSION_equipment_ring_left_str       "equipment_ring_left"
#define REDIS_SESSION_equipment_ring_right_str      "equipment_ring_right"
#define REDIS_SESSION_equipment_necklace_str        "equipment_necklace"

// Commander
#define REDIS_SESSION_commander_posX_str            "info_posX"
#define REDIS_SESSION_commander_posY_str            "info_posY"
#define REDIS_SESSION_commander_posZ_str            "info_posZ"
#define REDIS_SESSION_commander_currentXP_str       "info_currentXP"
#define REDIS_SESSION_commander_maxXP_str           "info_maxXP"
#define REDIS_SESSION_commander_pcId_str            "info_pcId"
#define REDIS_SESSION_commander_socialInfoId_str    "info_socialInfoId"
#define REDIS_SESSION_commander_commanderId_str     "info_commanderId"
#define REDIS_SESSION_commander_currentHP_str       "info_currentHP"
#define REDIS_SESSION_commander_maxHP_str           "info_maxHP"
#define REDIS_SESSION_commander_currentSP_str       "info_currentSP"
#define REDIS_SESSION_commander_maxSP_str           "info_maxSP"
#define REDIS_SESSION_commander_currentStamina_str  "info_currentStamina"
#define REDIS_SESSION_commander_maxStamina_str      "info_maxStamina"

enum RedisAccountSessionFields {
    // Account Session
    REDIS_ACCOUNT_SESSION_accountName,
    REDIS_ACCOUNT_SESSION_sessionKey,
    REDIS_ACCOUNT_SESSION_privilege,
    REDIS_ACCOUNT_SESSION_commandersCountMax,
};

enum RedisGameSessionFields {
    // Account Session
    REDIS_GAME_SESSION_account_accountName,
    REDIS_GAME_SESSION_account_sessionKey,
    REDIS_GAME_SESSION_account_privilege,
    REDIS_GAME_SESSION_account_commandersCountMax,

    // CommanderInfo Session
    REDIS_GAME_SESSION_commander_mapId,
    REDIS_GAME_SESSION_commander_commanderName,
    REDIS_GAME_SESSION_commander_familyName,
    REDIS_GAME_SESSION_commander_accountId,
    REDIS_GAME_SESSION_commander_classId,
    REDIS_GAME_SESSION_commander_jobId,
    REDIS_GAME_SESSION_commander_gender,
    REDIS_GAME_SESSION_commander_level,
    REDIS_GAME_SESSION_commander_hairId,
    REDIS_GAME_SESSION_commander_pose,

    // Equipment Session
    REDIS_GAME_SESSION_equipment_head_top,
    REDIS_GAME_SESSION_equipment_head_middle,
    REDIS_GAME_SESSION_equipment_itemUnk1,
    REDIS_GAME_SESSION_equipment_body_armor,
    REDIS_GAME_SESSION_equipment_gloves,
    REDIS_GAME_SESSION_equipment_boots,
    REDIS_GAME_SESSION_equipment_helmet,
    REDIS_GAME_SESSION_equipment_bracelet,
    REDIS_GAME_SESSION_equipment_weapon,
    REDIS_GAME_SESSION_equipment_shield,
    REDIS_GAME_SESSION_equipment_costume,
    REDIS_GAME_SESSION_equipment_itemUnk3,
    REDIS_GAME_SESSION_equipment_itemUnk4,
    REDIS_GAME_SESSION_equipment_itemUnk5,
    REDIS_GAME_SESSION_equipment_leg_armor,
    REDIS_GAME_SESSION_equipment_itemUnk6,
    REDIS_GAME_SESSION_equipment_itemUnk7,
    REDIS_GAME_SESSION_equipment_ring_left,
    REDIS_GAME_SESSION_equipment_ring_right,
    REDIS_GAME_SESSION_equipment_necklace,

    // CommanderInfo Info Session
    REDIS_GAME_SESSION_commander_posX,
    REDIS_GAME_SESSION_commander_posY,
    REDIS_GAME_SESSION_commander_posZ,
    REDIS_GAME_SESSION_commander_currentXP,
    REDIS_GAME_SESSION_commander_maxXP,
    REDIS_GAME_SESSION_commander_pcId,
    REDIS_GAME_SESSION_commander_socialInfoId,
    REDIS_GAME_SESSION_commander_commanderId,
    REDIS_GAME_SESSION_commander_currentHP,
    REDIS_GAME_SESSION_commander_maxHP,
    REDIS_GAME_SESSION_commander_currentSP,
    REDIS_GAME_SESSION_commander_maxSP,
    REDIS_GAME_SESSION_commander_currentStamina,
    REDIS_GAME_SESSION_commander_maxStamina,

	REDIS_SESSION_COUNT
};

typedef struct {
    uint16_t routerId;
    uint16_t mapId;
    uint64_t accountId;
} RedisGameSessionKey;

typedef RedisGameSessionKey RedisAccountSessionKey;

extern const char *redisGameSessionsStr [];


/**
 * @brief Get the AccountSession associated with the SocketSession
 * @param self An allocated Redis
 * @param key The AccountSession key
 * @param[out] accountSession The output AccountSession
 * @return true on success, false otherwise
 */
bool redisGetAccountSession(Redis *self, RedisAccountSessionKey *key, AccountSession *accountSession);

/**
 * @brief Get the GameSession associated with the SocketSession
 * @param self An allocated Redis
 * @param key The GameSession key
 * @param[out] gameSession The output gameSession
 * @return true on success, false otherwise
 */
bool redisGetGameSession(Redis *self, RedisGameSessionKey *key, GameSession *gameSession);

/**
 * @brief Get the GameSession by socket Id
 * @param self An allocated Redis
 * @param routerId The router ID containing the target session
 * @param socketId The socket ID of the target session
 * @param[out] gameSession The output gameSession
 * @return true on success, false otherwise
 */
bool redisGetGameSessionBySocketId(Redis *self, uint16_t routerId, uint8_t *socketId, GameSession *gameSession);

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
bool redisFlushGameSession(Redis *self, RedisGameSessionKey *key);

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
