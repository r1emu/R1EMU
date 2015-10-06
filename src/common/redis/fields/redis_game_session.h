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
#define REDIS_SESSION_account_accountName_str        "accountName"
#define REDIS_SESSION_account_sessionKey_str         "sessionKey"
#define REDIS_SESSION_account_privilege_str          "privilege"
#define REDIS_SESSION_account_commandersCountMax_str "commandersCountMax"

// Commander session
#define REDIS_SESSION_commander_mapId_str           "mapId"
#define REDIS_SESSION_commander_commanderName_str   "commanderName"
#define REDIS_SESSION_commander_familyName_str      "familyName"
#define REDIS_SESSION_commander_accountId_str       "accountId"
#define REDIS_SESSION_commander_classId_str         "classId"
#define REDIS_SESSION_commander_jobId_str           "jobId"
#define REDIS_SESSION_commander_gender_str          "gender"
#define REDIS_SESSION_commander_level_str           "level"
#define REDIS_SESSION_commander_hairId_str          "hairId"
#define REDIS_SESSION_commander_pose_str            "pose"
#define REDIS_SESSION_commander_posX_str            "posX"
#define REDIS_SESSION_commander_posY_str            "posY"
#define REDIS_SESSION_commander_posZ_str            "posZ"
#define REDIS_SESSION_commander_currentXP_str       "currentXP"
#define REDIS_SESSION_commander_maxXP_str           "maxXP"
#define REDIS_SESSION_commander_pcId_str            "pcId"
#define REDIS_SESSION_commander_socialInfoId_str    "socialInfoId"
#define REDIS_SESSION_commander_commanderId_str     "commanderId"
#define REDIS_SESSION_commander_currentHP_str       "currentHP"
#define REDIS_SESSION_commander_maxHP_str           "maxHP"
#define REDIS_SESSION_commander_currentSP_str       "currentSP"
#define REDIS_SESSION_commander_maxSP_str           "maxSP"
#define REDIS_SESSION_commander_currentStamina_str  "currentStamina"
#define REDIS_SESSION_commander_maxStamina_str      "maxStamina"

// Commander equipment
#define REDIS_SESSION_EQSLOT_HAT_str        "head_top"
#define REDIS_SESSION_EQSLOT_HAT_L_str     "head_middle"
#define REDIS_SESSION_EQSLOT_UNKOWN1_str         "itemUnk1"
#define REDIS_SESSION_EQSLOT_BODY_ARMOR_str      "body_armor"
#define REDIS_SESSION_EQSLOT_GLOVES_str          "gloves"
#define REDIS_SESSION_EQSLOT_BOOTS_str           "boots"
#define REDIS_SESSION_EQSLOT_HELMET_str          "helmet"
#define REDIS_SESSION_EQSLOT_BRACELET_str        "bracelet"
#define REDIS_SESSION_EQSLOT_WEAPON_str          "weapon"
#define REDIS_SESSION_EQSLOT_SHIELD_str          "shield"
#define REDIS_SESSION_EQSLOT_COSTUME_str         "costume"
#define REDIS_SESSION_EQSLOT_UNKOWN3_str         "itemUnk3"
#define REDIS_SESSION_EQSLOT_UNKOWN4_str         "itemUnk4"
#define REDIS_SESSION_EQSLOT_UNKOWN5_str         "itemUnk5"
#define REDIS_SESSION_EQSLOT_LEG_ARMOR_str       "leg_armor"
#define REDIS_SESSION_EQSLOT_UNKOWN6_str         "itemUnk6"
#define REDIS_SESSION_EQSLOT_UNKOWN7_str         "itemUnk7"
#define REDIS_SESSION_EQSLOT_RING_LEFT_str       "ring_left"
#define REDIS_SESSION_EQSLOT_RING_RIGHT_str      "ring_right"
#define REDIS_SESSION_EQSLOT_NECKLACE_str        "necklace"

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

    // Commander Session
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
    REDIS_GAME_SESSION_EQSLOT_HAT,
    REDIS_GAME_SESSION_EQSLOT_HAT_L,
    REDIS_GAME_SESSION_EQSLOT_UNKOWN1,
    REDIS_GAME_SESSION_EQSLOT_BODY_ARMOR,
    REDIS_GAME_SESSION_EQSLOT_GLOVES,
    REDIS_GAME_SESSION_EQSLOT_BOOTS,
    REDIS_GAME_SESSION_EQSLOT_HELMET,
    REDIS_GAME_SESSION_EQSLOT_BRACELET,
    REDIS_GAME_SESSION_EQSLOT_WEAPON,
    REDIS_GAME_SESSION_EQSLOT_SHIELD,
    REDIS_GAME_SESSION_EQSLOT_COSTUME,
    REDIS_GAME_SESSION_EQSLOT_UNKOWN3,
    REDIS_GAME_SESSION_EQSLOT_UNKOWN4,
    REDIS_GAME_SESSION_EQSLOT_UNKOWN5,
    REDIS_GAME_SESSION_EQSLOT_LEG_ARMOR,
    REDIS_GAME_SESSION_EQSLOT_UNKOWN6,
    REDIS_GAME_SESSION_EQSLOT_UNKOWN7,
    REDIS_GAME_SESSION_EQSLOT_RING_LEFT,
    REDIS_GAME_SESSION_EQSLOT_RING_RIGHT,
    REDIS_GAME_SESSION_EQSLOT_NECKLACE,

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
