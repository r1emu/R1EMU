/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file redis_socket_session.h
 * @brief RedisSocketSession fields definition
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#pragma once

#include "R1EMU.h"
#include "common/redis/redis.h"

#define REDIS_SOCKET_SESSION_accountId_str "accountId"
#define REDIS_SOCKET_SESSION_routerId_str "routerId"
#define REDIS_SOCKET_SESSION_mapId_str "mapId"
#define REDIS_SOCKET_SESSION_authenticated_str "authenticated"

enum RedisSocketSessionFields {
	REDIS_SOCKET_SESSION_accountId,
	REDIS_SOCKET_SESSION_routerId,
	REDIS_SOCKET_SESSION_mapId,
	REDIS_SOCKET_SESSION_authenticated,
	REDIS_SOCKET_SESSION_COUNT
};

typedef struct {
    RouterId_t routerId;
    uint8_t *sessionKey; // size : SOCKET_SESSION_ID_SIZE
} RedisSocketSessionKey;

extern const char *redisSocketSessionsStr[];

/**
 * @brief Get the SocketSession associated with the Socket ID
 * @param self An allocated Redis
 * @param key The SocketSession key
 * @param[out] socketSession The socket Session
 * @return
 */
bool redisGetSocketSession(Redis *self, RedisSocketSessionKey *key, SocketSession *socketSession);

/**
 * @brief Save an entire SocketSession to the Redis server.
 * @param self An allocated Redis instance
 * @param key The SocketSession key
 * @param socketSession An allocated socket session to refresh
 * @return true on success, false otherwise
 */
bool redisUpdateSocketSession(Redis *self, RedisSocketSessionKey *key, SocketSession *socketSession);

/**
 * @brief Flush a socket session
 * @param self An allocated Redis instance
 * @param key The SocketSession key
 * @return true on success, false otherwise
 */
bool redisFlushSocketSession(Redis *self, RedisSocketSessionKey *key);
