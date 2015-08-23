/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file RedisSocketSession.h
 * @brief RedisSocketSession fields definition
 *
 *
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#pragma once

// ---------- Includes ------------
#include "R1EMU.h"
#include "Common/Redis/Redis.h"

// ---------- Defines -------------
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

// ------ Structure declaration -------


// ------ Global variables declaration -------
typedef struct {

    uint16_t routerId;
    uint8_t *sessionKey; // Size : SOCKET_SESSION_ID_SIZE

} RedisSocketSessionKey;

extern const char *redisSocketSessionsStr [];


// ----------- Functions ------------
/**
 * @brief Get the SocketSession associated with the Socket ID
 * @param self An allocated Redis
 * @param key The SocketSession key
 * @param[out] socketSession The socket Session
 * @return
 */
bool
Redis_getSocketSession (
    Redis *self,
    RedisSocketSessionKey *key,
    SocketSession *socketSession
);


/**
 * @brief Save an entire SocketSession to the Redis server.
 * @param self An allocated Redis instance
 * @param key The SocketSession key
 * @param socketSession An allocated socket session to refresh
 * @return true on success, false otherwise
 */
bool
Redis_updateSocketSession (
    Redis *self,
    RedisSocketSessionKey *key,
    SocketSession *socketSession
);


/**
 * @brief Flush a socket session
 * @param self An allocated Redis instance
 * @param key The SocketSession key
 * @return true on success, false otherwise
 */
bool
Redis_flushSocketSession (
    Redis *self,
    RedisSocketSessionKey *key
);
