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
#include "common/utils/math.h"
#include "common/actor/item/item_factory.h"


// ------ Structure declaration -------


// ------ Static declaration -------


// ------ Extern variables implementation -------
const char *redisAccountSessionsStr [] = {
    // Account session
    [REDIS_ACCOUNT_SESSION_accountName] = REDIS_SESSION_account_accountName_str,
    [REDIS_ACCOUNT_SESSION_sessionKey] = REDIS_SESSION_account_sessionKey_str,
    [REDIS_ACCOUNT_SESSION_privilege] = REDIS_SESSION_account_privilege_str,
    [REDIS_ACCOUNT_SESSION_commandersCountMax] = REDIS_SESSION_account_commandersCountMax_str,
};

const char *redisGameSessionsStr [] = {
    // Account session
    [REDIS_GAME_SESSION_account_accountName] = REDIS_SESSION_account_accountName_str,
    [REDIS_GAME_SESSION_account_sessionKey] = REDIS_SESSION_account_sessionKey_str,
    [REDIS_GAME_SESSION_account_privilege] = REDIS_SESSION_account_privilege_str,
    [REDIS_GAME_SESSION_account_commandersCountMax] = REDIS_SESSION_account_commandersCountMax_str,

    // Commander session
    [REDIS_GAME_SESSION_commander_mapId] = REDIS_SESSION_commander_mapId_str,
    [REDIS_GAME_SESSION_commander_commanderName] = REDIS_SESSION_commander_commanderName_str,
    [REDIS_GAME_SESSION_commander_familyName] = REDIS_SESSION_commander_familyName_str,
    [REDIS_GAME_SESSION_commander_accountId] = REDIS_SESSION_commander_accountId_str,
    [REDIS_GAME_SESSION_commander_classId] = REDIS_SESSION_commander_classId_str,
    [REDIS_GAME_SESSION_commander_jobId] = REDIS_SESSION_commander_jobId_str,
    [REDIS_GAME_SESSION_commander_gender] = REDIS_SESSION_commander_gender_str,
    [REDIS_GAME_SESSION_commander_level] = REDIS_SESSION_commander_level_str,
    [REDIS_GAME_SESSION_commander_hairId] = REDIS_SESSION_commander_hairId_str,
    [REDIS_GAME_SESSION_commander_pose] = REDIS_SESSION_commander_pose_str,
    [REDIS_GAME_SESSION_commander_posX] = REDIS_SESSION_commander_posX_str,
    [REDIS_GAME_SESSION_commander_posY] = REDIS_SESSION_commander_posY_str,
    [REDIS_GAME_SESSION_commander_posZ] = REDIS_SESSION_commander_posZ_str,
    [REDIS_GAME_SESSION_commander_barrackPosX] = REDIS_SESSION_commander_barrackPosX_str,
    [REDIS_GAME_SESSION_commander_barrackPosY] = REDIS_SESSION_commander_barrackPosY_str,
    [REDIS_GAME_SESSION_commander_barrackPosZ] = REDIS_SESSION_commander_barrackPosZ_str,
    [REDIS_GAME_SESSION_commander_currentXP] = REDIS_SESSION_commander_currentXP_str,
    [REDIS_GAME_SESSION_commander_maxXP] = REDIS_SESSION_commander_maxXP_str,
    [REDIS_GAME_SESSION_commander_pcId] = REDIS_SESSION_commander_pcId_str,
    [REDIS_GAME_SESSION_commander_socialInfoId] = REDIS_SESSION_commander_socialInfoId_str,
    [REDIS_GAME_SESSION_commander_commanderId] = REDIS_SESSION_commander_commanderId_str,
    [REDIS_GAME_SESSION_commander_currentHP] = REDIS_SESSION_commander_currentHP_str,
    [REDIS_GAME_SESSION_commander_maxHP] = REDIS_SESSION_commander_maxHP_str,
    [REDIS_GAME_SESSION_commander_currentSP] = REDIS_SESSION_commander_currentSP_str,
    [REDIS_GAME_SESSION_commander_maxSP] = REDIS_SESSION_commander_maxSP_str,
    [REDIS_GAME_SESSION_commander_currentStamina] = REDIS_SESSION_commander_currentStamina_str,
    [REDIS_GAME_SESSION_commander_maxStamina] = REDIS_SESSION_commander_maxStamina_str,

    // Equipment session
    [REDIS_GAME_SESSION_EQSLOT_HAT] = REDIS_SESSION_EQSLOT_HAT_str,
    [REDIS_GAME_SESSION_EQSLOT_HAT_L] = REDIS_SESSION_EQSLOT_HAT_L_str,
    [REDIS_GAME_SESSION_EQSLOT_UNKNOWN1] = REDIS_SESSION_EQSLOT_UNKNOWN1_str,
    [REDIS_GAME_SESSION_EQSLOT_BODY_ARMOR] = REDIS_SESSION_EQSLOT_BODY_ARMOR_str,
    [REDIS_GAME_SESSION_EQSLOT_GLOVES] = REDIS_SESSION_EQSLOT_GLOVES_str,
    [REDIS_GAME_SESSION_EQSLOT_BOOTS] = REDIS_SESSION_EQSLOT_BOOTS_str,
    [REDIS_GAME_SESSION_EQSLOT_HELMET] = REDIS_SESSION_EQSLOT_HELMET_str,
    [REDIS_GAME_SESSION_EQSLOT_BRACELET] = REDIS_SESSION_EQSLOT_BRACELET_str,
    [REDIS_GAME_SESSION_EQSLOT_WEAPON] = REDIS_SESSION_EQSLOT_WEAPON_str,
    [REDIS_GAME_SESSION_EQSLOT_SHIELD] = REDIS_SESSION_EQSLOT_SHIELD_str,
    [REDIS_GAME_SESSION_EQSLOT_COSTUME] = REDIS_SESSION_EQSLOT_COSTUME_str,
    [REDIS_GAME_SESSION_EQSLOT_UNKNOWN3] = REDIS_SESSION_EQSLOT_UNKNOWN3_str,
    [REDIS_GAME_SESSION_EQSLOT_UNKNOWN4] = REDIS_SESSION_EQSLOT_UNKNOWN4_str,
    [REDIS_GAME_SESSION_EQSLOT_UNKNOWN5] = REDIS_SESSION_EQSLOT_UNKNOWN5_str,
    [REDIS_GAME_SESSION_EQSLOT_LEG_ARMOR] = REDIS_SESSION_EQSLOT_LEG_ARMOR_str,
    [REDIS_GAME_SESSION_EQSLOT_UNKNOWN6] = REDIS_SESSION_EQSLOT_UNKNOWN6_str,
    [REDIS_GAME_SESSION_EQSLOT_UNKNOWN7] = REDIS_SESSION_EQSLOT_UNKNOWN7_str,
    [REDIS_GAME_SESSION_EQSLOT_RING_LEFT] = REDIS_SESSION_EQSLOT_RING_LEFT_str,
    [REDIS_GAME_SESSION_EQSLOT_RING_RIGHT] = REDIS_SESSION_EQSLOT_RING_RIGHT_str,
    [REDIS_GAME_SESSION_EQSLOT_NECKLACE] = REDIS_SESSION_EQSLOT_NECKLACE_str
};


// ------ Extern functions implementation -------

bool redisGetAccountSession(Redis *self, RedisAccountSessionKey *key, AccountSession *accountSession) {

    bool result = false;
    redisReply *reply = NULL;

    reply = redisCommandDbg(self,
        "HMGET zone%x:map%x:acc%llx"
        " " REDIS_SESSION_account_accountName_str
        " " REDIS_SESSION_account_sessionKey_str
        " " REDIS_SESSION_account_privilege_str
        " " REDIS_SESSION_account_commandersCountMax_str
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
                error("Element <%s> returned by Redis is NULL.", redisAccountSessionsStr[elementIndex]);
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

bool redisGetGameSession(Redis *self, RedisGameSessionKey *key, GameSession *gameSession) {

    memset(gameSession, 0, sizeof(*gameSession));

    bool result = false;
    redisReply *reply = NULL;

    reply = redisCommandDbg(self,
        "HMGET zone%x:map%x:acc%llx"
        /** Keep these fields in the same order than the RedisGameSessionFields fields one */
        // Account
        " " REDIS_SESSION_account_accountName_str
        " " REDIS_SESSION_account_sessionKey_str
        " " REDIS_SESSION_account_privilege_str
        " " REDIS_SESSION_account_commandersCountMax_str
        // Commander
        " " REDIS_SESSION_commander_mapId_str
        " " REDIS_SESSION_commander_commanderName_str
        " " REDIS_SESSION_commander_familyName_str
        " " REDIS_SESSION_commander_accountId_str
        " " REDIS_SESSION_commander_classId_str
        " " REDIS_SESSION_commander_jobId_str
        " " REDIS_SESSION_commander_gender_str
        " " REDIS_SESSION_commander_level_str
        " " REDIS_SESSION_commander_hairId_str
        " " REDIS_SESSION_commander_pose_str
        " " REDIS_SESSION_commander_posX_str
        " " REDIS_SESSION_commander_posY_str
        " " REDIS_SESSION_commander_posZ_str
        " " REDIS_SESSION_commander_barrackPosX_str
        " " REDIS_SESSION_commander_barrackPosY_str
        " " REDIS_SESSION_commander_barrackPosZ_str
        " " REDIS_SESSION_commander_currentXP_str
        " " REDIS_SESSION_commander_maxXP_str
        " " REDIS_SESSION_commander_pcId_str
        " " REDIS_SESSION_commander_socialInfoId_str
        " " REDIS_SESSION_commander_commanderId_str
        " " REDIS_SESSION_commander_currentHP_str
        " " REDIS_SESSION_commander_maxHP_str
        " " REDIS_SESSION_commander_currentSP_str
        " " REDIS_SESSION_commander_maxSP_str
        " " REDIS_SESSION_commander_currentStamina_str
        " " REDIS_SESSION_commander_maxStamina_str
        // Equipment
        " " REDIS_SESSION_EQSLOT_HAT_str
        " " REDIS_SESSION_EQSLOT_HAT_L_str
        " " REDIS_SESSION_EQSLOT_UNKNOWN1_str
        " " REDIS_SESSION_EQSLOT_BODY_ARMOR_str
        " " REDIS_SESSION_EQSLOT_GLOVES_str
        " " REDIS_SESSION_EQSLOT_BOOTS_str
        " " REDIS_SESSION_EQSLOT_HELMET_str
        " " REDIS_SESSION_EQSLOT_BRACELET_str
        " " REDIS_SESSION_EQSLOT_WEAPON_str
        " " REDIS_SESSION_EQSLOT_SHIELD_str
        " " REDIS_SESSION_EQSLOT_COSTUME_str
        " " REDIS_SESSION_EQSLOT_UNKNOWN3_str
        " " REDIS_SESSION_EQSLOT_UNKNOWN4_str
        " " REDIS_SESSION_EQSLOT_UNKNOWN5_str
        " " REDIS_SESSION_EQSLOT_LEG_ARMOR_str
        " " REDIS_SESSION_EQSLOT_UNKNOWN6_str
        " " REDIS_SESSION_EQSLOT_UNKNOWN7_str
        " " REDIS_SESSION_EQSLOT_RING_LEFT_str
        " " REDIS_SESSION_EQSLOT_RING_RIGHT_str
        " " REDIS_SESSION_EQSLOT_NECKLACE_str

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

        case REDIS_REPLY_STATUS:
            // info("Redis status : %s", reply->str);
            break;

        case REDIS_REPLY_ARRAY: {
            // Check the number of elements retrieved
            if (reply->elements != REDIS_SESSION_COUNT) {
                error("Wrong number of elements received.");
                goto cleanup;
            }

            // Check if any element of the reply is NULL
            size_t elementIndex;
            if ((elementIndex = redisAnyElementIsNull (reply->element, reply->elements)) != -1) {
                error("Element <%s> returned by Redis is NULL.", redisGameSessionsStr[elementIndex]);
                goto cleanup;
            }

            /// Write the reply to the session
            // Account
            COPY_REDIS_GAME_STR(gameSession->accountSession.accountName, account_accountName);
            COPY_REDIS_GAME_STR(gameSession->accountSession.sessionKey, account_sessionKey);
            gameSession->accountSession.privilege = GET_REDIS_GAME_32(account_privilege);
            gameSession->accountSession.commandersCountMax = GET_REDIS_GAME_32(account_commandersCountMax);

            CommanderSession *commanderSession = &gameSession->commanderSession;
            Commander *commander = commanderSession->currentCommander = commanderNew();

            // Commander
            COPY_REDIS_GAME_STR(commander->commanderName, commander_commanderName);
            COPY_REDIS_GAME_STR(commander->familyName, commander_familyName);
            commander->accountId = GET_REDIS_GAME_64(commander_accountId);
            commander->classId   = GET_REDIS_GAME_32(commander_classId);
            commander->jobId     = GET_REDIS_GAME_32(commander_jobId);
            commander->gender    = GET_REDIS_GAME_32(commander_gender);
            commander->level     = GET_REDIS_GAME_32(commander_level);
            commander->hairId    = GET_REDIS_GAME_32(commander_hairId);
            commander->pose      = GET_REDIS_GAME_32(commander_pose);
            commander->mapId = GET_REDIS_GAME_32(commander_mapId);
            commander->pos.x = GET_REDIS_GAME_FLOAT(commander_posX);
            commander->pos.y = GET_REDIS_GAME_FLOAT(commander_posY);
            commander->pos.z = GET_REDIS_GAME_FLOAT(commander_posZ);
            commander->barrackPos.x = GET_REDIS_GAME_FLOAT(commander_barrackPosX);
            commander->barrackPos.y = GET_REDIS_GAME_FLOAT(commander_barrackPosY);
            commander->barrackPos.z = GET_REDIS_GAME_FLOAT(commander_barrackPosZ);
            commander->currentXP = GET_REDIS_GAME_32(commander_currentXP);
            commander->maxXP = GET_REDIS_GAME_32(commander_maxXP);
            commander->pcId = GET_REDIS_GAME_32(commander_pcId);
            commander->socialInfoId = GET_REDIS_GAME_64(commander_socialInfoId);
            commander->commanderId = GET_REDIS_GAME_64(commander_commanderId);
            commander->currentHP = GET_REDIS_GAME_32(commander_currentHP);
            commander->maxHP = GET_REDIS_GAME_32(commander_maxHP);
            commander->currentSP = GET_REDIS_GAME_32(commander_currentSP);
            commander->maxSP = GET_REDIS_GAME_32(commander_maxSP);
            commander->currentStamina = GET_REDIS_GAME_32(commander_currentStamina);
            commander->maxStamina = GET_REDIS_GAME_32(commander_maxStamina);

            // Equipment
            #define GET_REDIS_EQUIPMENT(x)                                                                     \
              if (!(commander->inventory.equippedItems[x] = (ItemEquipable *) itemFactoryCreate(               \
                     GET_REDIS_GAME_32(x), 1)))                                                                \
              {                                                                                                \
                    error("Cannot get item '%s'", STRINGIFY(x));                                               \
                    goto cleanup;                                                                              \
              }                                                                                                \

            GET_REDIS_EQUIPMENT(EQSLOT_HAT);
            GET_REDIS_EQUIPMENT(EQSLOT_HAT_L);
            GET_REDIS_EQUIPMENT(EQSLOT_UNKNOWN1);
            GET_REDIS_EQUIPMENT(EQSLOT_BODY_ARMOR);
            GET_REDIS_EQUIPMENT(EQSLOT_GLOVES);
            GET_REDIS_EQUIPMENT(EQSLOT_BOOTS);
            GET_REDIS_EQUIPMENT(EQSLOT_HELMET);
            GET_REDIS_EQUIPMENT(EQSLOT_BRACELET);
            GET_REDIS_EQUIPMENT(EQSLOT_WEAPON);
            GET_REDIS_EQUIPMENT(EQSLOT_SHIELD);
            GET_REDIS_EQUIPMENT(EQSLOT_COSTUME);
            GET_REDIS_EQUIPMENT(EQSLOT_UNKNOWN3);
            GET_REDIS_EQUIPMENT(EQSLOT_UNKNOWN4);
            GET_REDIS_EQUIPMENT(EQSLOT_UNKNOWN5);
            GET_REDIS_EQUIPMENT(EQSLOT_LEG_ARMOR);
            GET_REDIS_EQUIPMENT(EQSLOT_UNKNOWN6);
            GET_REDIS_EQUIPMENT(EQSLOT_UNKNOWN7);
            GET_REDIS_EQUIPMENT(EQSLOT_RING_LEFT);
            GET_REDIS_EQUIPMENT(EQSLOT_RING_RIGHT);
            GET_REDIS_EQUIPMENT(EQSLOT_NECKLACE);
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

bool redisGetGameSessionBySocketId(Redis *self, RouterId_t routerId, uint8_t *sessionKey, GameSession *gameSession) {

    SocketSession socketSession;
    RedisSocketSessionKey socketKey = {
        .routerId = routerId,
        .sessionKey = sessionKey
    };
    if (!(redisGetSocketSession(self, &socketKey, &socketSession))) {
        error("Cannot get the socket session of the client.");
        return false;
    }

    RedisGameSessionKey gameKey = {
        .routerId = socketSession.routerId,
        .mapId = socketSession.mapId,
        .accountId = socketSession.accountId
    };

    if (!(redisGetGameSession(self, &gameKey, gameSession))) {
        error("Cannot get the game session of the client.");
        return false;
    }

    return true;
}

bool redisUpdateGameSession(Redis *self, RedisGameSessionKey *key, uint8_t *socketId, GameSession *gameSession) {

    bool result = true;
    size_t repliesCount = 3;
    redisReply *replies[repliesCount];

    Commander *commander = NULL;

    // Initialize replies
    void *noReply = (void *) -1;
    for (int i = 0; i < repliesCount; i++) {
        replies[i] = noReply;
    }

    commander = gameSession->commanderSession.currentCommander;

    // Account
    replies[0] = redisCommandDbg(self,
        "HMSET zone%x:map%x:acc%llx"
        " " REDIS_SESSION_account_sessionKey_str " %s"
        " " REDIS_SESSION_account_accountName_str " %s"
        " " REDIS_SESSION_account_privilege_str " %x"
        " " REDIS_SESSION_account_commandersCountMax_str " %x",
        key->routerId, key->mapId, key->accountId,

        socketId,
        gameSession->accountSession.accountName,
        gameSession->accountSession.privilege,
        gameSession->accountSession.commandersCountMax
    );

    // Commander
    if (commander) {
        replies[1] = redisCommandDbg(self,
            "HMSET zone%x:map%x:acc%llx"
            " " REDIS_SESSION_commander_mapId_str " %x"
            " " REDIS_SESSION_commander_commanderName_str " %s"
            " " REDIS_SESSION_commander_familyName_str " %s"
            " " REDIS_SESSION_commander_accountId_str " %llx"
            " " REDIS_SESSION_commander_classId_str " %x"
            " " REDIS_SESSION_commander_jobId_str " %x"
            " " REDIS_SESSION_commander_gender_str " %x"
            " " REDIS_SESSION_commander_level_str " %x"
            " " REDIS_SESSION_commander_hairId_str " %x"
            " " REDIS_SESSION_commander_pose_str " %x"
            " " REDIS_SESSION_commander_posX_str " %f"
            " " REDIS_SESSION_commander_posY_str " %f"
            " " REDIS_SESSION_commander_posZ_str " %f"
            " " REDIS_SESSION_commander_barrackPosX_str " %f"
            " " REDIS_SESSION_commander_barrackPosY_str " %f"
            " " REDIS_SESSION_commander_barrackPosZ_str " %f"
            " " REDIS_SESSION_commander_currentXP_str " %x"
            " " REDIS_SESSION_commander_maxXP_str " %x"
            " " REDIS_SESSION_commander_pcId_str " %x"
            " " REDIS_SESSION_commander_socialInfoId_str " %llx"
            " " REDIS_SESSION_commander_commanderId_str " %llx"
            " " REDIS_SESSION_commander_currentHP_str " %x"
            " " REDIS_SESSION_commander_maxHP_str " %x"
            " " REDIS_SESSION_commander_currentSP_str " %x"
            " " REDIS_SESSION_commander_maxSP_str " %x"
            " " REDIS_SESSION_commander_currentStamina_str " %x"
            " " REDIS_SESSION_commander_maxStamina_str " %x",
            key->routerId, key->mapId, key->accountId,

            commander->mapId,
            CHECK_REDIS_EMPTY_STRING(commander->commanderName),
            CHECK_REDIS_EMPTY_STRING(commander->familyName),
            key->accountId,
            commander->classId,
            commander->jobId,
            commander->gender,
            commander->level,
            commander->hairId,
            commander->pose,
            commander->pos.x,
            commander->pos.y,
            commander->pos.z,
            commander->barrackPos.x,
            commander->barrackPos.y,
            commander->barrackPos.z,
            commander->currentXP,
            commander->maxXP,
            commander->pcId,
            commander->socialInfoId,
            commander->commanderId,
            commander->currentHP,
            commander->maxHP,
            commander->currentSP,
            commander->maxSP,
            commander->currentStamina,
            commander->maxStamina
        );

        replies[2] = redisCommandDbg(self,
            "HMSET zone%x:map%x:acc%llx"
            " " REDIS_SESSION_EQSLOT_HAT_str " %x"
            " " REDIS_SESSION_EQSLOT_HAT_L_str " %x"
            " " REDIS_SESSION_EQSLOT_UNKNOWN1_str " %x"
            " " REDIS_SESSION_EQSLOT_BODY_ARMOR_str " %x"
            " " REDIS_SESSION_EQSLOT_GLOVES_str " %x"
            " " REDIS_SESSION_EQSLOT_BOOTS_str " %x"
            " " REDIS_SESSION_EQSLOT_HELMET_str " %x"
            " " REDIS_SESSION_EQSLOT_BRACELET_str " %x"
            " " REDIS_SESSION_EQSLOT_WEAPON_str " %x"
            " " REDIS_SESSION_EQSLOT_SHIELD_str " %x"
            " " REDIS_SESSION_EQSLOT_COSTUME_str " %x"
            " " REDIS_SESSION_EQSLOT_UNKNOWN3_str " %x"
            " " REDIS_SESSION_EQSLOT_UNKNOWN4_str " %x"
            " " REDIS_SESSION_EQSLOT_UNKNOWN5_str " %x"
            " " REDIS_SESSION_EQSLOT_LEG_ARMOR_str " %x"
            " " REDIS_SESSION_EQSLOT_UNKNOWN6_str " %x"
            " " REDIS_SESSION_EQSLOT_UNKNOWN7_str " %x"
            " " REDIS_SESSION_EQSLOT_RING_LEFT_str " %x"
            " " REDIS_SESSION_EQSLOT_RING_RIGHT_str " %x"
            " " REDIS_SESSION_EQSLOT_NECKLACE_str " %x"
            , key->routerId, key->mapId, key->accountId,

            // Equipment
            itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_HAT]),
            itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_HAT_L]),
            itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_UNKNOWN1]),
            itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_BODY_ARMOR]),
            itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_GLOVES]),
            itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_BOOTS]),
            itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_HELMET]),
            itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_BRACELET]),
            itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_WEAPON]),
            itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_SHIELD]),
            itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_COSTUME]),
            itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_UNKNOWN3]),
            itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_UNKNOWN4]),
            itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_UNKNOWN5]),
            itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_LEG_ARMOR]),
            itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_UNKNOWN6]),
            itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_UNKNOWN7]),
            itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_RING_LEFT]),
            itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_RING_RIGHT]),
            itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_NECKLACE])
        );
    }

    for (int i = 0; i < repliesCount; i++) {
        redisReply *reply = replies[i];

        if (reply != noReply) {
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
                    // Ok
                    break;

                default :
                    error("Unexpected Redis status. (%d)", reply->type);
                    result = false;
                    goto cleanup;
                    break;
            }
        }
    }

cleanup:
    for (int i = 0; i < repliesCount; i++) {
        redisReply *reply = replies[i];

        if (reply && reply != noReply) {
            redisReplyDestroy(&reply);
        }
    }
    return result;
}

bool redisMoveGameSession(Redis *self, RedisGameSessionKey *from, RedisGameSessionKey *to) {
    bool result = true;
    redisReply *reply = NULL;

    reply = redisCommandDbg(self,
        "RENAME "
        "zone%x:map%x:acc%llx "
        "zone%x:map%x:acc%llx",
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

zlist_t *redisGetClientsWithinDistance(
    Redis *self,
    RouterId_t routerId, MapId_t mapId,
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
            "SCAN %d MATCH zone%x:map%x:acc*",
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
                        "HMGET %s " REDIS_SESSION_commander_posX_str
                                " " REDIS_SESSION_commander_posZ_str // Get position
                                " " REDIS_SESSION_account_sessionKey_str, // SocketKey
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

                            // [0] = X, [1] = Z, [2] = socketId
                            PositionXZ curPpos = {
                                .x = strtof (posReply->element[0]->str, NULL),
                                .z = strtof (posReply->element[1]->str, NULL)
                            };
                            char *socketId = posReply->element[2]->str;

                            // Check range here
                            if (mathIsWithin2DManhattanDistance (&curPpos, center, range)) {
                                // The current client is within the area, add it to the list
                                // Don't include the ignored socketId
                                if (!(ignoredSessionKey && strcmp (socketId, ignoredSessionKey) == 0)) {
                                    zlist_append(clients, strdup(socketId));
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

bool redisFlushGameSession (Redis *self, RedisGameSessionKey *key) {

    bool result = true;
    redisReply *reply = NULL;

    // Delete the key from the Redis
    reply = redisCommandDbg(self,
        "DEL zone%x:map%x:acc%llx",
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
