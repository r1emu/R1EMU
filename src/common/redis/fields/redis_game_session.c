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


// ------ Structure declaration -------


// ------ Static declaration -------


// ------ Extern variables implementation -------
const char *redisGameSessionsStr [] = {
    // Account session
    [REDIS_GAME_SESSION_account_login] = REDIS_GAME_SESSION_account_login_str,
    [REDIS_GAME_SESSION_account_sessionKey] = REDIS_GAME_SESSION_account_sessionKey_str,
    [REDIS_GAME_SESSION_account_privilege] = REDIS_GAME_SESSION_account_privilege_str,
    // Barrack session
    [REDIS_GAME_SESSION_barrack_charactersCreatedCount] = REDIS_GAME_SESSION_barrack_charactersCreatedCount_str,
    // CommanderInfo session
    [REDIS_GAME_SESSION_commander_mapId] = REDIS_GAME_SESSION_commander_mapId_str,
    [REDIS_GAME_SESSION_commander_commanderName] = REDIS_GAME_SESSION_commander_commanderName_str,
    [REDIS_GAME_SESSION_commander_familyName] = REDIS_GAME_SESSION_commander_familyName_str,
    [REDIS_GAME_SESSION_commander_accountId] = REDIS_GAME_SESSION_commander_accountId_str,
    [REDIS_GAME_SESSION_commander_classId] = REDIS_GAME_SESSION_commander_classId_str,
    [REDIS_GAME_SESSION_commander_unk4] = REDIS_GAME_SESSION_commander_unk4_str,
    [REDIS_GAME_SESSION_commander_jobId] = REDIS_GAME_SESSION_commander_jobId_str,
    [REDIS_GAME_SESSION_commander_gender] = REDIS_GAME_SESSION_commander_gender_str,
    [REDIS_GAME_SESSION_commander_unk5] = REDIS_GAME_SESSION_commander_unk5_str,
    [REDIS_GAME_SESSION_commander_level] = REDIS_GAME_SESSION_commander_level_str,
    [REDIS_GAME_SESSION_commander_hairId] = REDIS_GAME_SESSION_commander_hairId_str,
    [REDIS_GAME_SESSION_commander_pose] = REDIS_GAME_SESSION_commander_pose_str,
    // Equipment session
    [REDIS_GAME_SESSION_equipment_head_top] = REDIS_GAME_SESSION_equipment_head_top_str,
    [REDIS_GAME_SESSION_equipment_head_middle] = REDIS_GAME_SESSION_equipment_head_middle_str,
    [REDIS_GAME_SESSION_equipment_itemUnk1] = REDIS_GAME_SESSION_equipment_itemUnk1_str,
    [REDIS_GAME_SESSION_equipment_body_armor] = REDIS_GAME_SESSION_equipment_body_armor_str,
    [REDIS_GAME_SESSION_equipment_gloves] = REDIS_GAME_SESSION_equipment_gloves_str,
    [REDIS_GAME_SESSION_equipment_boots] = REDIS_GAME_SESSION_equipment_boots_str,
    [REDIS_GAME_SESSION_equipment_helmet] = REDIS_GAME_SESSION_equipment_helmet_str,
    [REDIS_GAME_SESSION_equipment_bracelet] = REDIS_GAME_SESSION_equipment_bracelet_str,
    [REDIS_GAME_SESSION_equipment_weapon] = REDIS_GAME_SESSION_equipment_weapon_str,
    [REDIS_GAME_SESSION_equipment_shield] = REDIS_GAME_SESSION_equipment_shield_str,
    [REDIS_GAME_SESSION_equipment_costume] = REDIS_GAME_SESSION_equipment_costume_str,
    [REDIS_GAME_SESSION_equipment_itemUnk3] = REDIS_GAME_SESSION_equipment_itemUnk3_str,
    [REDIS_GAME_SESSION_equipment_itemUnk4] = REDIS_GAME_SESSION_equipment_itemUnk4_str,
    [REDIS_GAME_SESSION_equipment_itemUnk5] = REDIS_GAME_SESSION_equipment_itemUnk5_str,
    [REDIS_GAME_SESSION_equipment_leg_armor] = REDIS_GAME_SESSION_equipment_leg_armor_str,
    [REDIS_GAME_SESSION_equipment_itemUnk6] = REDIS_GAME_SESSION_equipment_itemUnk6_str,
    [REDIS_GAME_SESSION_equipment_itemUnk7] = REDIS_GAME_SESSION_equipment_itemUnk7_str,
    [REDIS_GAME_SESSION_equipment_ring_left] = REDIS_GAME_SESSION_equipment_ring_left_str,
    [REDIS_GAME_SESSION_equipment_ring_right] = REDIS_GAME_SESSION_equipment_ring_right_str,
    [REDIS_GAME_SESSION_equipment_necklace] = REDIS_GAME_SESSION_equipment_necklace_str,
    // CommanderInfo Info session
    [REDIS_GAME_SESSION_info_posX] = REDIS_GAME_SESSION_info_posX_str,
    [REDIS_GAME_SESSION_info_posY] = REDIS_GAME_SESSION_info_posY_str,
    [REDIS_GAME_SESSION_info_posZ] = REDIS_GAME_SESSION_info_posZ_str,
    [REDIS_GAME_SESSION_info_currentXP] = REDIS_GAME_SESSION_info_currentXP_str,
    [REDIS_GAME_SESSION_info_maxXP] = REDIS_GAME_SESSION_info_maxXP_str,
    [REDIS_GAME_SESSION_info_pcId] = REDIS_GAME_SESSION_info_pcId_str,
    [REDIS_GAME_SESSION_info_socialInfoId] = REDIS_GAME_SESSION_info_socialInfoId_str,
    [REDIS_GAME_SESSION_info_commanderId] = REDIS_GAME_SESSION_info_commanderId_str,
    [REDIS_GAME_SESSION_info_currentHP] = REDIS_GAME_SESSION_info_currentHP_str,
    [REDIS_GAME_SESSION_info_maxHP] = REDIS_GAME_SESSION_info_maxHP_str,
    [REDIS_GAME_SESSION_info_currentSP] = REDIS_GAME_SESSION_info_currentSP_str,
    [REDIS_GAME_SESSION_info_maxSP] = REDIS_GAME_SESSION_info_maxSP_str,
    [REDIS_GAME_SESSION_info_currentStamina] = REDIS_GAME_SESSION_info_currentStamina_str,
    [REDIS_GAME_SESSION_info_maxStamina] = REDIS_GAME_SESSION_info_maxStamina_str,
    [REDIS_GAME_SESSION_info_unk6] = REDIS_GAME_SESSION_info_unk6_str,
    [REDIS_GAME_SESSION_info_unk7] = REDIS_GAME_SESSION_info_unk7_str,
};


// ------ Extern functions implementation -------

bool redisGetGameSession(Redis *self, RedisGameSessionKey *key, GameSession *gameSession) {
    bool result = true;
    redisReply *reply = NULL;

    reply = redisCommandDbg(self,
        "HMGET zone%x:map%x:acc%llx"
        /** Keep these fields in the same order than the RedisGameSessionFields fields one */
        // Account
        " " REDIS_GAME_SESSION_account_login_str
        " " REDIS_GAME_SESSION_account_sessionKey_str
        " " REDIS_GAME_SESSION_account_privilege_str
        // Barrack
        " " REDIS_GAME_SESSION_barrack_charactersCreatedCount_str
        // Commander
        " " REDIS_GAME_SESSION_commander_mapId_str
        " " REDIS_GAME_SESSION_commander_commanderName_str
        " " REDIS_GAME_SESSION_commander_familyName_str
        " " REDIS_GAME_SESSION_commander_accountId_str
        " " REDIS_GAME_SESSION_commander_classId_str
        " " REDIS_GAME_SESSION_commander_unk4_str
        " " REDIS_GAME_SESSION_commander_jobId_str
        " " REDIS_GAME_SESSION_commander_gender_str
        " " REDIS_GAME_SESSION_commander_unk5_str
        " " REDIS_GAME_SESSION_commander_level_str
        " " REDIS_GAME_SESSION_commander_hairId_str
        " " REDIS_GAME_SESSION_commander_pose_str
        // Equipment
        " " REDIS_GAME_SESSION_equipment_head_top_str
        " " REDIS_GAME_SESSION_equipment_head_middle_str
        " " REDIS_GAME_SESSION_equipment_itemUnk1_str
        " " REDIS_GAME_SESSION_equipment_body_armor_str
        " " REDIS_GAME_SESSION_equipment_gloves_str
        " " REDIS_GAME_SESSION_equipment_boots_str
        " " REDIS_GAME_SESSION_equipment_helmet_str
        " " REDIS_GAME_SESSION_equipment_bracelet_str
        " " REDIS_GAME_SESSION_equipment_weapon_str
        " " REDIS_GAME_SESSION_equipment_shield_str
        " " REDIS_GAME_SESSION_equipment_costume_str
        " " REDIS_GAME_SESSION_equipment_itemUnk3_str
        " " REDIS_GAME_SESSION_equipment_itemUnk4_str
        " " REDIS_GAME_SESSION_equipment_itemUnk5_str
        " " REDIS_GAME_SESSION_equipment_leg_armor_str
        " " REDIS_GAME_SESSION_equipment_itemUnk6_str
        " " REDIS_GAME_SESSION_equipment_itemUnk7_str
        " " REDIS_GAME_SESSION_equipment_ring_left_str
        " " REDIS_GAME_SESSION_equipment_ring_right_str
        " " REDIS_GAME_SESSION_equipment_necklace_str
        // CommanderInfo Info
        " " REDIS_GAME_SESSION_info_posX_str
        " " REDIS_GAME_SESSION_info_posY_str
        " " REDIS_GAME_SESSION_info_posZ_str
        " " REDIS_GAME_SESSION_info_currentXP_str
        " " REDIS_GAME_SESSION_info_maxXP_str
        " " REDIS_GAME_SESSION_info_pcId_str
        " " REDIS_GAME_SESSION_info_socialInfoId_str
        " " REDIS_GAME_SESSION_info_commanderId_str
        " " REDIS_GAME_SESSION_info_currentHP_str
        " " REDIS_GAME_SESSION_info_maxHP_str
        " " REDIS_GAME_SESSION_info_currentSP_str
        " " REDIS_GAME_SESSION_info_maxSP_str
        " " REDIS_GAME_SESSION_info_currentStamina_str
        " " REDIS_GAME_SESSION_info_maxStamina_str
        " " REDIS_GAME_SESSION_info_unk6_str
        " " REDIS_GAME_SESSION_info_unk7_str

        , key->routerId, key->mapId, key->accountId
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

        case REDIS_REPLY_ARRAY: {
            // Check the number of elements retrieved
            if (reply->elements != REDIS_GAME_SESSION_COUNT) {
                error("Wrong number of elements received.");
                result = false;
                goto cleanup;
            }

            // Check if any element of the reply is NULL
            size_t elementIndex;
            if ((elementIndex = redisAnyElementIsNull (reply->element, reply->elements)) != -1) {
                error("Element <%s> returned by Redis is NULL.", redisGameSessionsStr[elementIndex]);
                result = false;
                goto cleanup;
            }

            /// Write the reply to the session
            CommanderSession *commanderSession = &gameSession->commanderSession;
            CommanderInfo *cInfo = &commanderSession->currentCommander.info;
            CommanderAppearance *appearance = &cInfo->appearance;
            CommanderEquipment *equipment = &appearance->equipment;

            // Account
            COPY_REDIS_STR(gameSession->accountSession.login, account_login);
            COPY_REDIS_STR(gameSession->accountSession.sessionKey, account_sessionKey);
            gameSession->accountSession.privilege = GET_REDIS_32(account_privilege);

            // Barrack
            gameSession->accountSession.charactersCreatedCount = GET_REDIS_32(barrack_charactersCreatedCount);

            // Commander
            commanderSession->mapId = GET_REDIS_32(commander_mapId);
            COPY_REDIS_STR(appearance->commanderName, commander_commanderName);
            COPY_REDIS_STR(appearance->familyName, commander_familyName);
            appearance->accountId = GET_REDIS_64 (commander_accountId);
            appearance->classId   = GET_REDIS_32(commander_classId);
            appearance->unk4      = GET_REDIS_32(commander_unk4);
            appearance->jobId     = GET_REDIS_32(commander_jobId);
            appearance->gender    = GET_REDIS_32(commander_gender);
            appearance->unk5      = GET_REDIS_32(commander_unk5);
            appearance->level     = GET_REDIS_32(commander_level);
            appearance->hairId    = GET_REDIS_32(commander_hairId);
            appearance->pose      = GET_REDIS_32(commander_pose);

            // Equipment
            equipment->head_top    = GET_REDIS_32(equipment_head_top);
            equipment->head_middle = GET_REDIS_32(equipment_head_middle);
            equipment->itemUnk1    = GET_REDIS_32(equipment_itemUnk1);
            equipment->body_armor  = GET_REDIS_32(equipment_body_armor);
            equipment->gloves      = GET_REDIS_32(equipment_gloves);
            equipment->boots       = GET_REDIS_32(equipment_boots);
            equipment->helmet      = GET_REDIS_32(equipment_helmet);
            equipment->bracelet    = GET_REDIS_32(equipment_bracelet);
            equipment->weapon      = GET_REDIS_32(equipment_weapon);
            equipment->shield      = GET_REDIS_32(equipment_shield);
            equipment->costume     = GET_REDIS_32(equipment_costume);
            equipment->itemUnk3    = GET_REDIS_32(equipment_itemUnk3);
            equipment->itemUnk4    = GET_REDIS_32(equipment_itemUnk4);
            equipment->itemUnk5    = GET_REDIS_32(equipment_itemUnk5);
            equipment->leg_armor   = GET_REDIS_32(equipment_leg_armor);
            equipment->itemUnk6    = GET_REDIS_32(equipment_itemUnk6);
            equipment->itemUnk7    = GET_REDIS_32(equipment_itemUnk7);
            equipment->ring_left   = GET_REDIS_32(equipment_ring_left);
            equipment->ring_right  = GET_REDIS_32(equipment_ring_right);
            equipment->necklace    = GET_REDIS_32(equipment_necklace);

            // CommanderInfo info
            cInfo->pos.x = GET_REDIS_FLOAT (info_posX);
            cInfo->pos.y = GET_REDIS_FLOAT (info_posY);
            cInfo->pos.z = GET_REDIS_FLOAT (info_posZ);
            cInfo->currentXP = GET_REDIS_32(info_currentXP);
            cInfo->maxXP = GET_REDIS_32(info_maxXP);
            cInfo->pcId = GET_REDIS_32(info_pcId);
            cInfo->socialInfoId = GET_REDIS_64 (info_socialInfoId);
            cInfo->commanderId = GET_REDIS_64 (info_commanderId);
            cInfo->currentHP = GET_REDIS_32(info_currentHP);
            cInfo->maxHP = GET_REDIS_32(info_maxHP);
            cInfo->currentSP = GET_REDIS_32(info_currentSP);
            cInfo->maxSP = GET_REDIS_32(info_maxSP);
            cInfo->currentStamina = GET_REDIS_32(info_currentStamina);
            cInfo->maxStamina = GET_REDIS_32(info_maxStamina);
            cInfo->unk6 = GET_REDIS_32(info_unk6);
            cInfo->unk7 = GET_REDIS_32(info_unk7);

            #undef COPY_REDIS_STR
            #undef GET_REDIS_32
            #undef GET_REDIS_64
            #undef GET_REDIS_FLOAT
        }
        break;

        default :
            error("Unexpected Redis status (%d).", reply->type);
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

bool redisGetGameSessionBySocketId(Redis *self, uint16_t routerId, uint8_t *sessionKey, GameSession *gameSession) {

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
    redisReply *reply = NULL;

    CommanderInfo *cInfo = &gameSession->commanderSession.currentCommander.info;
    CommanderAppearance *appearance = &cInfo->appearance;
    CommanderEquipment *equipment = &appearance->equipment;

    reply = redisCommandDbg(self,
        "HMSET zone%x:map%x:acc%llx"
        // Account
        " " REDIS_GAME_SESSION_account_sessionKey_str " %s"
        " " REDIS_GAME_SESSION_account_login_str " %s"
        " " REDIS_GAME_SESSION_account_privilege_str " %x"
        // Barrack
        " " REDIS_GAME_SESSION_barrack_charactersCreatedCount_str " %x"
        // Commander
        " " REDIS_GAME_SESSION_commander_mapId_str " %x"
        " " REDIS_GAME_SESSION_commander_commanderName_str " %s"
        " " REDIS_GAME_SESSION_commander_familyName_str " %s"
        " " REDIS_GAME_SESSION_commander_accountId_str " %llx"
        " " REDIS_GAME_SESSION_commander_classId_str " %x"
        " " REDIS_GAME_SESSION_commander_unk4_str " %x"
        " " REDIS_GAME_SESSION_commander_jobId_str " %x"
        " " REDIS_GAME_SESSION_commander_gender_str " %x"
        " " REDIS_GAME_SESSION_commander_unk5_str " %x"
        " " REDIS_GAME_SESSION_commander_level_str " %x"
        " " REDIS_GAME_SESSION_commander_hairId_str " %x"
        " " REDIS_GAME_SESSION_commander_pose_str " %x"
        // Equipment
        " " REDIS_GAME_SESSION_equipment_head_top_str " %x"
        " " REDIS_GAME_SESSION_equipment_head_middle_str " %x"
        " " REDIS_GAME_SESSION_equipment_itemUnk1_str " %x"
        " " REDIS_GAME_SESSION_equipment_body_armor_str " %x"
        " " REDIS_GAME_SESSION_equipment_gloves_str " %x"
        " " REDIS_GAME_SESSION_equipment_boots_str " %x"
        " " REDIS_GAME_SESSION_equipment_helmet_str " %x"
        " " REDIS_GAME_SESSION_equipment_bracelet_str " %x"
        " " REDIS_GAME_SESSION_equipment_weapon_str " %x"
        " " REDIS_GAME_SESSION_equipment_shield_str " %x"
        " " REDIS_GAME_SESSION_equipment_costume_str " %x"
        " " REDIS_GAME_SESSION_equipment_itemUnk3_str " %x"
        " " REDIS_GAME_SESSION_equipment_itemUnk4_str " %x"
        " " REDIS_GAME_SESSION_equipment_itemUnk5_str " %x"
        " " REDIS_GAME_SESSION_equipment_leg_armor_str " %x"
        " " REDIS_GAME_SESSION_equipment_itemUnk6_str " %x"
        " " REDIS_GAME_SESSION_equipment_itemUnk7_str " %x"
        " " REDIS_GAME_SESSION_equipment_ring_left_str " %x"
        " " REDIS_GAME_SESSION_equipment_ring_right_str " %x"
        " " REDIS_GAME_SESSION_equipment_necklace_str " %x"
        // CommanderInfo Info
        " " REDIS_GAME_SESSION_info_posX_str " %f"
        " " REDIS_GAME_SESSION_info_posY_str " %f"
        " " REDIS_GAME_SESSION_info_posZ_str " %f"
        " " REDIS_GAME_SESSION_info_currentXP_str " %x"
        " " REDIS_GAME_SESSION_info_maxXP_str " %x"
        " " REDIS_GAME_SESSION_info_pcId_str " %x"
        " " REDIS_GAME_SESSION_info_socialInfoId_str " %llx"
        " " REDIS_GAME_SESSION_info_commanderId_str " %llx"
        " " REDIS_GAME_SESSION_info_currentHP_str " %x"
        " " REDIS_GAME_SESSION_info_maxHP_str " %x"
        " " REDIS_GAME_SESSION_info_currentSP_str " %x"
        " " REDIS_GAME_SESSION_info_maxSP_str " %x"
        " " REDIS_GAME_SESSION_info_currentStamina_str " %x"
        " " REDIS_GAME_SESSION_info_maxStamina_str " %x"
        " " REDIS_GAME_SESSION_info_unk6_str " %x"
        " " REDIS_GAME_SESSION_info_unk7_str " %x"
        , key->routerId, key->mapId, key->accountId,

        // Account
        socketId,
        gameSession->accountSession.login,
        gameSession->accountSession.privilege,
        // Barrack
        gameSession->accountSession.charactersCreatedCount,
        // Commander
        gameSession->commanderSession.mapId,
        CHECK_REDIS_EMPTY_STRING(appearance->commanderName),
        CHECK_REDIS_EMPTY_STRING(appearance->familyName),
        key->accountId,
        appearance->classId,
        appearance->unk4,
        appearance->jobId,
        appearance->gender,
        appearance->unk5,
        appearance->level,
        appearance->hairId,
        appearance->pose,

        // Equipment
        equipment->head_top,
        equipment->head_middle,
        equipment->itemUnk1,
        equipment->body_armor,
        equipment->gloves,
        equipment->boots,
        equipment->helmet,
        equipment->bracelet,
        equipment->weapon,
        equipment->shield,
        equipment->costume,
        equipment->itemUnk3,
        equipment->itemUnk4,
        equipment->itemUnk5,
        equipment->leg_armor,
        equipment->itemUnk6,
        equipment->itemUnk7,
        equipment->ring_left,
        equipment->ring_right,
        equipment->necklace,

        // CommanderInfo info
        cInfo->pos.x,
        cInfo->pos.y,
        cInfo->pos.z,
        cInfo->currentXP,
        cInfo->maxXP,
        cInfo->pcId,
        cInfo->socialInfoId,
        cInfo->commanderId,
        cInfo->currentHP,
        cInfo->maxHP,
        cInfo->currentSP,
        cInfo->maxSP,
        cInfo->currentStamina,
        cInfo->maxStamina,
        cInfo->unk6,
        cInfo->unk7
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
                        "HMGET %s " REDIS_GAME_SESSION_info_posX_str
                                " " REDIS_GAME_SESSION_info_posZ_str // Get position
                                " " REDIS_GAME_SESSION_account_sessionKey_str, // SocketKey
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
