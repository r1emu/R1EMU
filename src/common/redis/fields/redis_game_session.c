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
#include "redis_game_session.h"
#include "redis_socket_session.h"
#include "redis_account_session.h"
#include "redis_commander_session.h"
#include "common/utils/math.h"


// ------ Structure declaration -------


// ------ Static declaration -------


// ------ Extern variables implementation -------

// ------ Extern functions implementation -------

bool redisGetGameSession(Redis *self, RedisGameSessionKey *gameSessionKey, GameSession *gameSession) {

    bool status = false;

    RedisAccountSessionKey *accountSessionKey = gameSessionKey;
    RedisCommanderSessionKey *commanderSessionKey = gameSessionKey;

    if (!(redisGetAccountSession(self, accountSessionKey, &gameSession->accountSession))) {
        error("Cannot get the account session.");
        goto cleanup;
    }

    if (!(redisGetCommanderSession(self, commanderSessionKey, &gameSession->commanderSession))) {
        error("Cannot get the commander session.");
        goto cleanup;
    }

    status = true;

cleanup:
    return status;
}

bool redisUpdateGameSession(Redis *self, RedisGameSessionKey *gameSessionKey, uint8_t *sessionKey, GameSession *gameSession) {

    bool result = true;

    RedisAccountSessionKey *accountSessionKey = gameSessionKey;
    RedisCommanderSessionKey *commanderSessionKey = gameSessionKey;

    if (!(redisUpdateAccountSession(self, accountSessionKey, sessionKey, &gameSession->accountSession))) {
        error("Cannot update the account session.");
        goto cleanup;
    }

    if (!(redisUpdateCommanderSession(self, commanderSessionKey, sessionKey, &gameSession->commanderSession))) {
        error("Cannot update the account session.");
        goto cleanup;
    }

    result = true;

cleanup:
    return result;
}

bool redisMoveGameSession(Redis *self, RedisGameSessionKey *from, RedisGameSessionKey *to) {

    bool status = false;

    RedisAccountSessionKey *accountSessionKeyFrom = from;
    RedisAccountSessionKey *accountSessionKeyTo = to;
    if (!(redisMoveAccountSession(self, accountSessionKeyFrom, accountSessionKeyTo))) {
        error("Cannot move the account session.");
        goto cleanup;
    }

    RedisCommanderSessionKey *commanderSessionKeyFrom = from;
    RedisCommanderSessionKey *commanderSessionKeyTo = to;
    if (!(redisMoveCommanderSession(self, commanderSessionKeyFrom, commanderSessionKeyTo))) {
        error("Cannot move the commander session.");
        goto cleanup;
    }

    status = true;

cleanup:
    return status;
}

zlist_t *redisGetClientsWithinDistance(
    Redis *self,
    uint16_t routerId, uint16_t mapId,
    PositionXZ *center,
    float range,
    char *ignoredSessionKey
) {
    bool status = true;
    zlist_t *clients = NULL;
    redisReply *reply = NULL;
    redisReply *posReply = NULL;

    // TODO : Could be better. Don't allocate a new zlist everytime we call this function.
    // Who got this list then ? The Worker?
    if (!(clients = zlist_new())) {
        error("Cannot allocate a new zlist.");
        status = false;
        goto cleanup;
    }

    // Iterate through all the clients

    // TODO : Huge optimization to do here !
    // It iterates through all the clients of the zone server in the chosen map
    // It could use a better clustering method, but it should be enough for the moment

    // Start iterating
    int iterator = 0;

    do {
        // Get all the accounts within the same area
        reply = redisCommandDbg(self,
            "SCAN %d MATCH zone%x:map%x:acc*:commanderSession",
            iterator, routerId, mapId
        );

        if (!reply) {
            error("Redis error encountered : The request is invalid.");
            status = false;
            goto cleanup;
        }

        switch (reply->type) {
            case REDIS_REPLY_ERROR:
                error("Redis error encountered : %s", reply->str);
                status = false;
                goto cleanup;
                break;

            case REDIS_REPLY_ARRAY: {
                // [0] = new iterator
                iterator = strtoul(reply->element[0]->str, NULL, 10);
                // [1] = results
                for (int i = 0; i < reply->element[1]->elements; i++) {
                    // Get the position of all accounts in the map
                    posReply = redisCommandDbg(self,
                        "HMGET %s " REDIS_COMMANDER_SESSION_posX_str
                                " " REDIS_COMMANDER_SESSION_posZ_str // Get position
                                " " REDIS_ACCOUNT_SESSION_sessionKey_str, // SocketKey
                        reply->element[1]->element[i]->str // account key
                    );

                    if (!posReply) {
                        error("Redis error encountered : The request is invalid.");
                        status = false;
                        goto cleanup;
                    }

                    switch (posReply->type) {

                        case REDIS_REPLY_ERROR:
                            error("Redis error encountered : %s", reply->str);
                            status = false;
                            goto cleanup;
                            break;

                        case REDIS_REPLY_STATUS:
                            // info("Redis status : %s", reply->str);
                            break;

                        case REDIS_REPLY_ARRAY: {
                            if (posReply->elements != 3) {
                                error("Abnormal number of elements (%d, should be 3).", posReply->elements);
                                status = false;
                                goto cleanup;
                            }

                            // [0] = X, [1] = Z, [2] = sessionKey
                            PositionXZ curPpos = {
                                .x = strtof (posReply->element[0]->str, NULL),
                                .z = strtof (posReply->element[1]->str, NULL)
                            };
                            char *sessionKey = posReply->element[2]->str;

                            // Check range here
                            if (mathIsWithin2DManhattanDistance (&curPpos, center, range)) {
                                // The current client is within the area, add it to the list
                                // Don't include the ignored sessionKey
                                if (!(ignoredSessionKey && strcmp (sessionKey, ignoredSessionKey) == 0)) {
                                    zlist_append(clients, strdup(sessionKey));
                                }
                            }
                        } break;

                        default :
                            error("Unexpected Redis status. (%d)", reply->type);
                            status = false;
                            goto cleanup;
                            break;
                    }

                    redisReplyDestroy(&posReply);
                }
            } break;

            default :
                error("Unexpected Redis status. (%d)", reply->type);
                status = false;
                goto cleanup;
                break;
        }

        redisReplyDestroy(&reply);

    } while (iterator != 0);

cleanup:
    if (!status) {
        zlist_destroy (&clients);
    }
    redisReplyDestroy(&reply);
    redisReplyDestroy(&posReply);

    return clients;
}

bool redisDeleteGameSession(Redis *self, RedisGameSessionKey *gameSessionKey) {

    bool status = false;

    RedisAccountSessionKey *accountSessionKey = gameSessionKey;
    RedisCommanderSessionKey *commanderSessionKey = gameSessionKey;

    if (!(redisDeleteAccountSession(self, accountSessionKey))) {
        error("Cannot flush account session.");
        goto cleanup;
    }

    if (!(redisDeleteCommanderSession(self, commanderSessionKey))) {
        error("Cannot flush commander session.");
        goto cleanup;
    }

    status = true;

cleanup:
    return status;
}
