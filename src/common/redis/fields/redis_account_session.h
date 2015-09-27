/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file redis_account_session.h
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
typedef RedisGameSessionKey RedisAccountSessionKey;

// ----------- Functions ------------

/**
 * Get, update, move or delete an account session inside Redis
 */
bool redisGetAccountSession(Redis *self, RedisAccountSessionKey *key, AccountSession *accountSession);
bool redisUpdateAccountSession(Redis *self, RedisAccountSessionKey *key, uint8_t *sessionKey, AccountSession *accountSession);
bool redisMoveAccountSession(Redis *self, RedisAccountSessionKey *from, RedisAccountSessionKey *to);
bool redisDeleteAccountSession(Redis *self, RedisAccountSessionKey *key);
