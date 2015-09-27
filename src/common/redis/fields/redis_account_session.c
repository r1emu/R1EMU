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

#include "redis_account_session.h"


const char *redisAccountSessionStr [] = {
    // Account session
    [REDIS_ACCOUNT_SESSION_accountName] = REDIS_ACCOUNT_SESSION_accountName_str,
    [REDIS_ACCOUNT_SESSION_sessionKey] = REDIS_ACCOUNT_SESSION_sessionKey_str,
    [REDIS_ACCOUNT_SESSION_privilege] = REDIS_ACCOUNT_SESSION_privilege_str,
    [REDIS_ACCOUNT_SESSION_commandersCountMax] = REDIS_ACCOUNT_SESSION_commandersCountMax_str,
};


bool redisGetAccountSession(Redis *self, RedisAccountSessionKey *key, AccountSession *accountSession) {

    bool result = false;
    redisReply *reply = NULL;

    reply = redisCommandDbg(self,
        "HMGET zone%x:map%x:acc%llx:accountSession"
        " " REDIS_ACCOUNT_SESSION_accountName_str
        " " REDIS_ACCOUNT_SESSION_sessionKey_str
        " " REDIS_ACCOUNT_SESSION_privilege_str
        " " REDIS_ACCOUNT_SESSION_commandersCountMax_str
        , key->routerId, key->mapId, key->accountId
    );

    if (!reply) {
        error("Redis error encountered : The request is invalid.");
        goto cleanup;
    }

    switch (reply->type)
    {
        case REDIS_REPLY_ERROR:
            error("Redis error encountered : %s", reply->str);
            goto cleanup;
            break;

        case REDIS_REPLY_ARRAY: {
            // Check if any element of the reply is NULL
            size_t elementIndex;
            if ((elementIndex = redisAnyElementIsNull (reply->element, reply->elements)) != -1) {
                error("Element <%s> returned by Redis is NULL.", redisAccountSessionStr[elementIndex]);
                goto cleanup;
            }

            /// Write the reply to the session
            COPY_REDIS_ACCOUNT_STR(accountSession->accountName, accountName);
            COPY_REDIS_ACCOUNT_STR(accountSession->sessionKey, sessionKey);
            accountSession->privilege = GET_REDIS_ACCOUNT_32(privilege);
            accountSession->commandersCountMax = GET_REDIS_ACCOUNT_32(commandersCountMax);
        }
        break;

        default :
            error("Unexpected Redis status (%d).", reply->type);
            goto cleanup;
            break;
    }

    result = true;

cleanup:
    if (reply) {
        redisReplyDestroy(&reply);
    }

    return result;
}

bool redisUpdateAccountSession(Redis *self, RedisAccountSessionKey *key, uint8_t *sessionKey, AccountSession *accountSession) {

    bool result = false;
    redisReply *reply = NULL;

    // Account
    reply = redisCommandDbg(self,
        "HMSET zone%x:map%x:acc%llx:accountSession"
        " " REDIS_ACCOUNT_SESSION_sessionKey_str " %s"
        " " REDIS_ACCOUNT_SESSION_accountName_str " %s"
        " " REDIS_ACCOUNT_SESSION_privilege_str " %x"
        " " REDIS_ACCOUNT_SESSION_commandersCountMax_str " %x",
        key->routerId, key->mapId, key->accountId,
        sessionKey,
        accountSession->accountName,
        accountSession->privilege,
        accountSession->commandersCountMax
    );

    if (!reply) {
        error("Redis error encountered : The request is invalid.");
        goto cleanup;
    }

    switch (reply->type)
    {
        case REDIS_REPLY_ERROR:
            error("Redis error encountered : %s", reply->str);
            result = false;
            goto cleanup;
            break;

        case REDIS_REPLY_STATUS:
            // Ok
            break;

        default :
            error("Unexpected Redis status. (%d)", reply->type);
            result = false;
            goto cleanup;
            break;
    }

    result = true;

cleanup:
    redisReplyDestroy(&reply);

    return result;
}


bool redisMoveAccountSession(Redis *self, RedisAccountSessionKey *from, RedisAccountSessionKey *to) {

    bool result = true;
    redisReply *reply = NULL;

    reply = redisCommandDbg(self,
        "RENAME"
        " zone%x:map%x:acc%llx:accountSession"
        " zone%x:map%x:acc%llx:accountSession",
        from->routerId, from->mapId, from->accountId,
          to->routerId,   to->mapId,   to->accountId
    );

    if (!reply) {
        error("Redis error encountered : The request is invalid.");
        result = false;
        goto cleanup;
    }

    switch (reply->type)
    {
        case REDIS_REPLY_ERROR:
            error("Redis error encountered : %s", reply->str);
            result = false;
            goto cleanup;
            break;

        case REDIS_REPLY_STATUS:
            // info("Redis status : %s", reply->str);
            break;

        default :
            error("Unexpected Redis status. (%d)", reply->type);
            result = false;
            goto cleanup;
            break;
    }

cleanup:
    if (reply) {
        redisReplyDestroy(&reply);
    }

    return result;
}


bool redisDeleteAccountSession(Redis *self, RedisAccountSessionKey *key) {

    bool result = true;
    redisReply *reply = NULL;

    // Delete the key from Redis
    reply = redisCommandDbg(self,
        "DEL zone%x:map%x:acc%llx:accountSession",
        key->routerId, key->mapId, key->accountId
    );

    if (!reply) {
        error("Redis error encountered : The request is invalid.");
        result = false;
        goto cleanup;
    }

    switch (reply->type)
    {
        case REDIS_REPLY_ERROR:
            error("Redis error encountered : %s", reply->str);
            result = false;
            goto cleanup;
            break;

        case REDIS_REPLY_INTEGER:
            // Delete OK
            break;

        default :
            error("Unexpected Redis status : %d", reply->type);
            result = false;
            goto cleanup;
            break;
    }

cleanup:
    if (reply) {
        redisReplyDestroy(&reply);
    }

    return result;
}

