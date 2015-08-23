/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @license <license placeholder>
 */


// ---------- Includes ------------
#include "RedisSocketSession.h"
#include "RedisGameSession.h"


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
bool
Redis_getSocketSession (
    Redis *self,
    RedisSocketSessionKey *key,
    SocketSession *socketSession
) {
	redisReply *reply = NULL;

	reply = Redis_commandDbg (self,
        "HMGET zone%x:socket%s "
        REDIS_SOCKET_SESSION_accountId_str " "
        REDIS_SOCKET_SESSION_routerId_str " "
        REDIS_SOCKET_SESSION_mapId_str " "
        REDIS_SOCKET_SESSION_authenticated_str " ",
        key->routerId, key->sessionKey
    );

    if (!reply) {
        error ("Redis error encountered : The request is invalid.");
        return false;
    }

    switch (reply->type)
    {
        case REDIS_REPLY_ERROR:
            error ("Redis error encountered : %s", reply->str);
            Redis_replyDestroy (&reply);
            return false;
        break;

        case REDIS_REPLY_STATUS:
            // info ("Redis status : %s", reply->str);
            Redis_replyDestroy (&reply);
            return false;
        break;

        case REDIS_REPLY_ARRAY:
            if (reply->elements != REDIS_SOCKET_SESSION_COUNT) {
                error ("Wrong number of elements received.");
                Redis_replyDestroy (&reply);
                return false;
            }

            if (Redis_anyElementIsNull (reply->element, reply->elements) != -1) {
                // The socket session doesn't exist : create a new one and set it to its default value
                SocketSession_init (socketSession, SOCKET_SESSION_UNDEFINED_ACCOUNT, key->routerId, SOCKET_SESSION_UNDEFINED_MAP, key->sessionKey, false);

                // Update the newly created socketSession to the Redis Session
                RedisSocketSessionKey socketKey = {
                    .routerId = socketSession->routerId,
                    .sessionKey = socketSession->sessionKey
                };
                if (!Redis_updateSocketSession (self, &socketKey, socketSession)) {
                    dbg ("Cannot update the socket session");
                    Redis_replyDestroy (&reply);
                    return false;
                }
            }
            else {
                // Read the socket Session from the Redis server
                socketSession->accountId = strtoull (reply->element[REDIS_SOCKET_SESSION_accountId]->str, NULL, 16);
                socketSession->routerId = strtoul (reply->element[REDIS_SOCKET_SESSION_routerId]->str, NULL, 16);
                socketSession->mapId = strtoul (reply->element[REDIS_SOCKET_SESSION_mapId]->str, NULL, 16);
                socketSession->authenticated = strtoul (reply->element[REDIS_SOCKET_SESSION_authenticated]->str, NULL, 16);
                memcpy (socketSession->sessionKey, key->sessionKey, sizeof (socketSession->sessionKey));
            }
        break;

        default :
            error ("Unexpected Redis status : %d", reply->type);
            Redis_replyDestroy (&reply);
            return false;
        break;
    }

    Redis_replyDestroy (&reply);
    return true;
}


bool
Redis_updateSocketSession (
    Redis *self,
    RedisSocketSessionKey *key,
    SocketSession *socketSession
) {
    redisReply *reply = NULL;

    reply = Redis_commandDbg (self,
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
        error ("Redis error encountered : The request is invalid.");
        return false;
    }

    switch (reply->type)
    {
        case REDIS_REPLY_ERROR:
            error ("Redis error encountered : %s", reply->str);
            return false;
        break;

        case REDIS_REPLY_STATUS:
            // info ("Redis status : %s", reply->str);
        break;

        default :
            error ("Unexpected Redis status : %d", reply->type);
            Redis_replyDestroy (&reply);
            return false;
        break;
    }

    Redis_replyDestroy (&reply);

    return true;
}


bool
Redis_flushSocketSession (
    Redis *self,
    RedisSocketSessionKey *key
) {
    redisReply *reply = NULL;

    // Delete the key from the Redis
    reply = Redis_commandDbg (self,
        "DEL zone%x:socket%s",
        key->routerId, key->sessionKey
    );

    if (!reply) {
        error ("Redis error encountered : The request is invalid.");
        return false;
    }

    switch (reply->type)
    {
        case REDIS_REPLY_ERROR:
            error ("Redis error encountered : %s", reply->str);
            return false;
        break;

        case REDIS_REPLY_INTEGER:
            // Delete OK
        break;

        default :
            error ("Unexpected Redis status : %d", reply->type);
            Redis_replyDestroy (&reply);
            return false;
        break;
    }

    Redis_replyDestroy (&reply);

    return true;
}
