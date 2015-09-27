/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file redis_commander_session.h
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
#include "redis_game_session.h"
#include "common/redis/redis.h"
#include "common/session/session.h"

// ---------- Defines -------------

// ------ Structure declaration -------
typedef RedisGameSessionKey RedisCommanderSessionKey;

// ----------- Functions ------------

/**
 * Get, update, move or delete an commander session inside Redis
 */
bool redisGetCommanderSession(Redis *self, RedisCommanderSessionKey *key, CommanderSession *commanderSession);
bool redisUpdateCommanderSession(Redis *self, RedisCommanderSessionKey *key, uint8_t *sessionKey, CommanderSession *commanderSession);
bool redisMoveCommanderSession(Redis *self, RedisCommanderSessionKey *from, RedisCommanderSessionKey *to);
bool redisDeleteCommanderSession(Redis *self, RedisCommanderSessionKey *key);
