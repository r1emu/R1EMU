/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file redis_session.h
 * @brief
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#pragma once

#include "R1EMU.h"
#include "common/session/session.h"
#include "common/redis/redis.h"
#include "common/redis/fields/redis_socket_session.h"

typedef struct {
    RedisSocketSessionKey socketKey;
} RedisSessionKey;

/**
 * @brief Request the session from a SessionKey
 * @param self An allocated Redis
 * @param key The GameSession key
 * @param[out] session The resulting Session
 * @return
 */
bool redisGetSession(Redis *self, RedisSessionKey *key, Session *session);

/**
 * @brief Save an entire Session to the Redis server.
 * @param self An allocated Redis instance
 * @param key The GameSession key
 * @param session The Session to save
 * @return true on success, false otherwise
 */
bool redisUpdateSession(Redis *redis,Session *session);

/**
 * @brief Flush an entire Session
 * @param self An allocated Redis instance
 * @param key The key of the Session
 * @return true on success, false otherwise
 */
bool redisFlushSession(Redis *self,RedisSessionKey *key);
