/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */


// ---------- Includes ------------
#include "redis_socket_session.h"
#include "redis_game_session.h"


// ------ Structure declaration -------


// ------ Static declaration -------


// ------ Extern variables implementation -------
const char *redisSocketSessionsStr [] = {
	[REDIS_SOCKET_SESSION_accountId] = REDIS_SOCKET_SESSION_accountId_str,
	[REDIS_SOCKET_SESSION_routerId] = REDIS_SOCKET_SESSION_routerId_str,
	[REDIS_SOCKET_SESSION_mapId] = REDIS_SOCKET_SESSION_mapId_str,
	[REDIS_SOCKET_SESSION_authenticated] = REDIS_SOCKET_SESSION_authenticated_str
};

// ------ Extern functions implementation -------
bool redisGetSocketSession(Redis *self, RedisSocketSessionKey *key, SocketSession *socketSession) {

	redisReply *reply = NULL;

	reply = redisCommandDbg(self,
        "HMGET zone%x:socket%s "
        REDIS_SOCKET_SESSION_accountId_str " "
        REDIS_SOCKET_SESSION_routerId_str " "
        REDIS_SOCKET_SESSION_mapId_str " "
        REDIS_SOCKET_SESSION_authenticated_str " ",
        key->routerId, key->sessionKey
    );

    if (!reply) {
        error("Redis error encountered : The request is invalid.");
        return false;
    }

    switch (reply->type)
    {
        case REDIS_REPLY_ERROR:
            error("Redis error encountered : %s", reply->str);
            redisReplyDestroy(&reply);
            return false;
            break;

        case REDIS_REPLY_STATUS:
            // info("Redis status : %s", reply->str);
            redisReplyDestroy(&reply);
            return false;
            break;

        case REDIS_REPLY_ARRAY:
            if (reply->elements != REDIS_SOCKET_SESSION_COUNT) {
                error("Wrong number of elements received.");
                redisReplyDestroy(&reply);
                return false;
            }

            if (redisAnyElementIsNull (reply->element, reply->elements) != -1) {
                // The socket session doesn't exist : create a new one and set it to its default value
                socketSessionInit(socketSession, SOCKET_SESSION_UNDEFINED_ACCOUNT, key->routerId, SOCKET_SESSION_UNDEFINED_MAP, key->sessionKey, false);

                // Update the newly created socketSession to the Redis Session
                RedisSocketSessionKey socketKey = {
                    .routerId = socketSession->routerId,
                    .sessionKey = socketSession->sessionKey
                };
                if (!redisUpdateSocketSession (self, &socketKey, socketSession)) {
                    dbg("Cannot update the socket session");
                    redisReplyDestroy(&reply);
                    return false;
                }
            }
            else {
                // Read the socket Session from the Redis server
                socketSession->accountId = strtoull(reply->element[REDIS_SOCKET_SESSION_accountId]->str, NULL, 16);
                socketSession->routerId = strtoul(reply->element[REDIS_SOCKET_SESSION_routerId]->str, NULL, 16);
                socketSession->mapId = strtoul(reply->element[REDIS_SOCKET_SESSION_mapId]->str, NULL, 16);
                socketSession->authenticated = strtoul(reply->element[REDIS_SOCKET_SESSION_authenticated]->str, NULL, 16);
                memcpy(socketSession->sessionKey, key->sessionKey, sizeof(socketSession->sessionKey));
            }
        break;

        default :
            error("Unexpected Redis status : %d", reply->type);
            redisReplyDestroy(&reply);
            return false;
            break;
    }

    redisReplyDestroy(&reply);

    return true;
}

bool redisUpdateSocketSession (Redis *self, RedisSocketSessionKey *key, SocketSession *socketSession) {

    redisReply *reply = NULL;

    reply = redisCommandDbg(self,
        "HMSET zone%x:socket%s"
        " accountId %llx"
        " routerId %x"
        " mapId %x"
        " authenticated %x"
        , key->routerId, key->sessionKey,
        socketSession->accountId,
        key->routerId,
        socketSession->mapId,
        socketSession->authenticated
    );

    if (!reply) {
        error("Redis error encountered : The request is invalid.");
        return false;
    }

    switch (reply->type)
    {
        case REDIS_REPLY_ERROR:
            error("Redis error encountered : %s", reply->str);
            return false;
            break;

        case REDIS_REPLY_STATUS:
            // info("Redis status : %s", reply->str);
            break;

        default :
            error("Unexpected Redis status : %d", reply->type);
            redisReplyDestroy(&reply);
            return false;
            break;
    }

    redisReplyDestroy(&reply);

    return true;
}

bool redisFlushSocketSession (Redis *self, RedisSocketSessionKey *key) {

    redisReply *reply = NULL;

    // Delete the key from the Redis
    reply = redisCommandDbg(self,
        "DEL zone%x:socket%s",
        key->routerId, key->sessionKey
    );

    if (!reply) {
        error("Redis error encountered : The request is invalid.");
        return false;
    }

    switch (reply->type)
    {
        case REDIS_REPLY_ERROR:
            error("Redis error encountered : %s", reply->str);
            return false;
            break;

        case REDIS_REPLY_INTEGER:
            // Delete OK
            break;

        default :
            error("Unexpected Redis status : %d", reply->type);
            redisReplyDestroy(&reply);
            return false;
            break;
    }

    redisReplyDestroy(&reply);

    return true;
}
