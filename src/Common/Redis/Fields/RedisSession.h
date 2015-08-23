/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file RedisSession.h
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
#include "Common/Session/Session.h"
#include "Common/Redis/Redis.h"
#include "Common/Redis/Fields/RedisSocketSession.h"

// ---------- Defines -------------


// ------ Structure declaration -------
typedef struct {
    RedisSocketSessionKey socketKey;
} RedisSessionKey;


// ----------- Functions ------------

/**
 * @brief Request the session from a SessionKey
 * @param self An allocated Redis
 * @param key The GameSession key
 * @param[out] session The resulting Session
 * @return
 */
bool
Redis_getSession (
    Redis *self,
    RedisSessionKey *key,
    Session *session
);


/**
 * @brief Save an entire Session to the Redis server.
 * @param self An allocated Redis instance
 * @param key The GameSession key
 * @param session The Session to save
 * @return true on success, false otherwise
 */
bool
Redis_updateSession (
    Redis *redis,
    Session *session
);

/**
 * @brief Flush an entire Session
 * @param self An allocated Redis instance
 * @param key The key of the Session
 * @return true on success, false otherwise
 */
bool
Redis_flushSession (
    Redis *self,
    RedisSessionKey *key
);

