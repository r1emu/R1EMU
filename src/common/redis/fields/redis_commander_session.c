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

#include "redis_commander_session.h"

const char *redisCommanderSessionStr [] = {
    // Commander session
    [REDIS_COMMANDER_SESSION_mapId] = REDIS_COMMANDER_SESSION_mapId_str,
    [REDIS_COMMANDER_SESSION_commanderName] = REDIS_COMMANDER_SESSION_commanderName_str,
    [REDIS_COMMANDER_SESSION_familyName] = REDIS_COMMANDER_SESSION_familyName_str,
    [REDIS_COMMANDER_SESSION_accountId] = REDIS_COMMANDER_SESSION_accountId_str,
    [REDIS_COMMANDER_SESSION_classId] = REDIS_COMMANDER_SESSION_classId_str,
    [REDIS_COMMANDER_SESSION_jobId] = REDIS_COMMANDER_SESSION_jobId_str,
    [REDIS_COMMANDER_SESSION_gender] = REDIS_COMMANDER_SESSION_gender_str,
    [REDIS_COMMANDER_SESSION_level] = REDIS_COMMANDER_SESSION_level_str,
    [REDIS_COMMANDER_SESSION_hairId] = REDIS_COMMANDER_SESSION_hairId_str,
    [REDIS_COMMANDER_SESSION_pose] = REDIS_COMMANDER_SESSION_pose_str,
    [REDIS_COMMANDER_SESSION_posX] = REDIS_COMMANDER_SESSION_posX_str,
    [REDIS_COMMANDER_SESSION_posY] = REDIS_COMMANDER_SESSION_posY_str,
    [REDIS_COMMANDER_SESSION_posZ] = REDIS_COMMANDER_SESSION_posZ_str,
    [REDIS_COMMANDER_SESSION_currentXP] = REDIS_COMMANDER_SESSION_currentXP_str,
    [REDIS_COMMANDER_SESSION_maxXP] = REDIS_COMMANDER_SESSION_maxXP_str,
    [REDIS_COMMANDER_SESSION_pcId] = REDIS_COMMANDER_SESSION_pcId_str,
    [REDIS_COMMANDER_SESSION_socialInfoId] = REDIS_COMMANDER_SESSION_socialInfoId_str,
    [REDIS_COMMANDER_SESSION_commanderId] = REDIS_COMMANDER_SESSION_commanderId_str,
    [REDIS_COMMANDER_SESSION_currentHP] = REDIS_COMMANDER_SESSION_currentHP_str,
    [REDIS_COMMANDER_SESSION_maxHP] = REDIS_COMMANDER_SESSION_maxHP_str,
    [REDIS_COMMANDER_SESSION_currentSP] = REDIS_COMMANDER_SESSION_currentSP_str,
    [REDIS_COMMANDER_SESSION_maxSP] = REDIS_COMMANDER_SESSION_maxSP_str,
    [REDIS_COMMANDER_SESSION_currentStamina] = REDIS_COMMANDER_SESSION_currentStamina_str,
    [REDIS_COMMANDER_SESSION_maxStamina] = REDIS_COMMANDER_SESSION_maxStamina_str,
    [REDIS_COMMANDER_SESSION_head_top] = REDIS_COMMANDER_SESSION_head_top_str,
    [REDIS_COMMANDER_SESSION_head_middle] = REDIS_COMMANDER_SESSION_head_middle_str,
    [REDIS_COMMANDER_SESSION_itemUnk1] = REDIS_COMMANDER_SESSION_itemUnk1_str,
    [REDIS_COMMANDER_SESSION_body_armor] = REDIS_COMMANDER_SESSION_body_armor_str,
    [REDIS_COMMANDER_SESSION_gloves] = REDIS_COMMANDER_SESSION_gloves_str,
    [REDIS_COMMANDER_SESSION_boots] = REDIS_COMMANDER_SESSION_boots_str,
    [REDIS_COMMANDER_SESSION_helmet] = REDIS_COMMANDER_SESSION_helmet_str,
    [REDIS_COMMANDER_SESSION_bracelet] = REDIS_COMMANDER_SESSION_bracelet_str,
    [REDIS_COMMANDER_SESSION_weapon] = REDIS_COMMANDER_SESSION_weapon_str,
    [REDIS_COMMANDER_SESSION_shield] = REDIS_COMMANDER_SESSION_shield_str,
    [REDIS_COMMANDER_SESSION_costume] = REDIS_COMMANDER_SESSION_costume_str,
    [REDIS_COMMANDER_SESSION_itemUnk3] = REDIS_COMMANDER_SESSION_itemUnk3_str,
    [REDIS_COMMANDER_SESSION_itemUnk4] = REDIS_COMMANDER_SESSION_itemUnk4_str,
    [REDIS_COMMANDER_SESSION_itemUnk5] = REDIS_COMMANDER_SESSION_itemUnk5_str,
    [REDIS_COMMANDER_SESSION_leg_armor] = REDIS_COMMANDER_SESSION_leg_armor_str,
    [REDIS_COMMANDER_SESSION_itemUnk6] = REDIS_COMMANDER_SESSION_itemUnk6_str,
    [REDIS_COMMANDER_SESSION_itemUnk7] = REDIS_COMMANDER_SESSION_itemUnk7_str,
    [REDIS_COMMANDER_SESSION_ring_left] = REDIS_COMMANDER_SESSION_ring_left_str,
    [REDIS_COMMANDER_SESSION_ring_right] = REDIS_COMMANDER_SESSION_ring_right_str,
    [REDIS_COMMANDER_SESSION_necklace] = REDIS_COMMANDER_SESSION_necklace_str,
};


bool redisUpdateCommanderSession(
    Redis *self,
    RedisCommanderSessionKey *key,
    uint8_t *sessionKey,
    CommanderSession *commanderSession)
{
    bool result = false;
    redisReply *reply = NULL;

    Commander *commander = commanderSession->currentCommander;
    CommanderAppearance *appearance = &commander->appearance;
    CommanderEquipment *equipment = &appearance->equipment;

    // Commander
    reply = redisCommandDbg(self,
        "HMSET zone%x:map%x:acc%llx"
        " " REDIS_COMMANDER_SESSION_mapId_str " %x"
        " " REDIS_COMMANDER_SESSION_commanderName_str " %s"
        " " REDIS_COMMANDER_SESSION_familyName_str " %s"
        " " REDIS_COMMANDER_SESSION_accountId_str " %llx"
        " " REDIS_COMMANDER_SESSION_classId_str " %x"
        " " REDIS_COMMANDER_SESSION_jobId_str " %x"
        " " REDIS_COMMANDER_SESSION_gender_str " %x"
        " " REDIS_COMMANDER_SESSION_level_str " %x"
        " " REDIS_COMMANDER_SESSION_hairId_str " %x"
        " " REDIS_COMMANDER_SESSION_pose_str " %x"
        " " REDIS_COMMANDER_SESSION_posX_str " %f"
        " " REDIS_COMMANDER_SESSION_posY_str " %f"
        " " REDIS_COMMANDER_SESSION_posZ_str " %f"
        " " REDIS_COMMANDER_SESSION_currentXP_str " %x"
        " " REDIS_COMMANDER_SESSION_maxXP_str " %x"
        " " REDIS_COMMANDER_SESSION_pcId_str " %x"
        " " REDIS_COMMANDER_SESSION_socialInfoId_str " %llx"
        " " REDIS_COMMANDER_SESSION_commanderId_str " %llx"
        " " REDIS_COMMANDER_SESSION_currentHP_str " %x"
        " " REDIS_COMMANDER_SESSION_maxHP_str " %x"
        " " REDIS_COMMANDER_SESSION_currentSP_str " %x"
        " " REDIS_COMMANDER_SESSION_maxSP_str " %x"
        " " REDIS_COMMANDER_SESSION_currentStamina_str " %x"
        " " REDIS_COMMANDER_SESSION_maxStamina_str " %x"
        " " REDIS_COMMANDER_SESSION_head_top_str " %x"
        " " REDIS_COMMANDER_SESSION_head_middle_str " %x"
        " " REDIS_COMMANDER_SESSION_itemUnk1_str " %x"
        " " REDIS_COMMANDER_SESSION_body_armor_str " %x"
        " " REDIS_COMMANDER_SESSION_gloves_str " %x"
        " " REDIS_COMMANDER_SESSION_boots_str " %x"
        " " REDIS_COMMANDER_SESSION_helmet_str " %x"
        " " REDIS_COMMANDER_SESSION_bracelet_str " %x"
        " " REDIS_COMMANDER_SESSION_weapon_str " %x"
        " " REDIS_COMMANDER_SESSION_shield_str " %x"
        " " REDIS_COMMANDER_SESSION_costume_str " %x"
        " " REDIS_COMMANDER_SESSION_itemUnk3_str " %x"
        " " REDIS_COMMANDER_SESSION_itemUnk4_str " %x"
        " " REDIS_COMMANDER_SESSION_itemUnk5_str " %x"
        " " REDIS_COMMANDER_SESSION_leg_armor_str " %x"
        " " REDIS_COMMANDER_SESSION_itemUnk6_str " %x"
        " " REDIS_COMMANDER_SESSION_itemUnk7_str " %x"
        " " REDIS_COMMANDER_SESSION_ring_left_str " %x"
        " " REDIS_COMMANDER_SESSION_ring_right_str " %x"
        " " REDIS_COMMANDER_SESSION_necklace_str " %x",

        commander->mapId,
        CHECK_REDIS_EMPTY_STRING(appearance->commanderName),
        CHECK_REDIS_EMPTY_STRING(appearance->familyName),
        key->accountId,
        appearance->classId,
        appearance->jobId,
        appearance->gender,
        appearance->level,
        appearance->hairId,
        appearance->pose,
        commander->pos.x,
        commander->pos.y,
        commander->pos.z,
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
        commander->maxStamina,
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
        equipment->necklace
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
            // Ok
            break;

        default :
            error("Unexpected Redis status. (%d)", reply->type);
            goto cleanup;
            break;
    }

    result = true;

cleanup:
    redisReplyDestroy(&reply);

    return result;
}

bool redisGetCommanderSession(Redis *self, RedisCommanderSessionKey *key, CommanderSession *commanderSession) {

    bool result = false;
    redisReply *reply = NULL;

    reply = redisCommandDbg(self,
        "HMGET zone%x:map%x:acc%llx:commanderSession"
        " " REDIS_COMMANDER_SESSION_mapId_str
        " " REDIS_COMMANDER_SESSION_commanderName_str
        " " REDIS_COMMANDER_SESSION_familyName_str
        " " REDIS_COMMANDER_SESSION_accountId_str
        " " REDIS_COMMANDER_SESSION_classId_str
        " " REDIS_COMMANDER_SESSION_jobId_str
        " " REDIS_COMMANDER_SESSION_gender_str
        " " REDIS_COMMANDER_SESSION_level_str
        " " REDIS_COMMANDER_SESSION_hairId_str
        " " REDIS_COMMANDER_SESSION_pose_str
        " " REDIS_COMMANDER_SESSION_posX_str
        " " REDIS_COMMANDER_SESSION_posY_str
        " " REDIS_COMMANDER_SESSION_posZ_str
        " " REDIS_COMMANDER_SESSION_currentXP_str
        " " REDIS_COMMANDER_SESSION_maxXP_str
        " " REDIS_COMMANDER_SESSION_pcId_str
        " " REDIS_COMMANDER_SESSION_socialInfoId_str
        " " REDIS_COMMANDER_SESSION_commanderId_str
        " " REDIS_COMMANDER_SESSION_currentHP_str
        " " REDIS_COMMANDER_SESSION_maxHP_str
        " " REDIS_COMMANDER_SESSION_currentSP_str
        " " REDIS_COMMANDER_SESSION_maxSP_str
        " " REDIS_COMMANDER_SESSION_currentStamina_str
        " " REDIS_COMMANDER_SESSION_maxStamina_str
        " " REDIS_COMMANDER_SESSION_head_top_str
        " " REDIS_COMMANDER_SESSION_head_middle_str
        " " REDIS_COMMANDER_SESSION_itemUnk1_str
        " " REDIS_COMMANDER_SESSION_body_armor_str
        " " REDIS_COMMANDER_SESSION_gloves_str
        " " REDIS_COMMANDER_SESSION_boots_str
        " " REDIS_COMMANDER_SESSION_helmet_str
        " " REDIS_COMMANDER_SESSION_bracelet_str
        " " REDIS_COMMANDER_SESSION_weapon_str
        " " REDIS_COMMANDER_SESSION_shield_str
        " " REDIS_COMMANDER_SESSION_costume_str
        " " REDIS_COMMANDER_SESSION_itemUnk3_str
        " " REDIS_COMMANDER_SESSION_itemUnk4_str
        " " REDIS_COMMANDER_SESSION_itemUnk5_str
        " " REDIS_COMMANDER_SESSION_leg_armor_str
        " " REDIS_COMMANDER_SESSION_itemUnk6_str
        " " REDIS_COMMANDER_SESSION_itemUnk7_str
        " " REDIS_COMMANDER_SESSION_ring_left_str
        " " REDIS_COMMANDER_SESSION_ring_right_str
        " " REDIS_COMMANDER_SESSION_necklace_str
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
                error("Element <%s> returned by Redis is NULL.", redisCommanderSessionStr[elementIndex]);
                goto cleanup;
            }

            /// Write the reply to the session
            Commander *commander = commanderSession->currentCommander;
            CommanderAppearance *appearance = &commander->appearance;
            CommanderEquipment *equipment = &appearance->equipment;

            COPY_REDIS_COMMANDER_STR(appearance->commanderName, commanderName);
            COPY_REDIS_COMMANDER_STR(appearance->familyName, familyName);
            appearance->accountId = GET_REDIS_COMMANDER_64(accountId);
            appearance->classId   = GET_REDIS_COMMANDER_32(classId);
            appearance->jobId     = GET_REDIS_COMMANDER_32(jobId);
            appearance->gender    = GET_REDIS_COMMANDER_32(gender);
            appearance->level     = GET_REDIS_COMMANDER_32(level);
            appearance->hairId    = GET_REDIS_COMMANDER_32(hairId);
            appearance->pose      = GET_REDIS_COMMANDER_32(pose);

            commander->mapId = GET_REDIS_COMMANDER_32(mapId);
            commander->pos.x = GET_REDIS_COMMANDER_FLOAT(posX);
            commander->pos.y = GET_REDIS_COMMANDER_FLOAT(posY);
            commander->pos.z = GET_REDIS_COMMANDER_FLOAT(posZ);
            commander->currentXP = GET_REDIS_COMMANDER_32(currentXP);
            commander->maxXP = GET_REDIS_COMMANDER_32(maxXP);
            commander->pcId = GET_REDIS_COMMANDER_32(pcId);
            commander->socialInfoId = GET_REDIS_COMMANDER_64(socialInfoId);
            commander->commanderId = GET_REDIS_COMMANDER_64(commanderId);
            commander->currentHP = GET_REDIS_COMMANDER_32(currentHP);
            commander->maxHP = GET_REDIS_COMMANDER_32(maxHP);
            commander->currentSP = GET_REDIS_COMMANDER_32(currentSP);
            commander->maxSP = GET_REDIS_COMMANDER_32(maxSP);
            commander->currentStamina = GET_REDIS_COMMANDER_32(currentStamina);
            commander->maxStamina = GET_REDIS_COMMANDER_32(maxStamina);

            // Equipment
            equipment->head_top    = GET_REDIS_COMMANDER_32(head_top);
            equipment->head_middle = GET_REDIS_COMMANDER_32(head_middle);
            equipment->itemUnk1    = GET_REDIS_COMMANDER_32(itemUnk1);
            equipment->body_armor  = GET_REDIS_COMMANDER_32(body_armor);
            equipment->gloves      = GET_REDIS_COMMANDER_32(gloves);
            equipment->boots       = GET_REDIS_COMMANDER_32(boots);
            equipment->helmet      = GET_REDIS_COMMANDER_32(helmet);
            equipment->bracelet    = GET_REDIS_COMMANDER_32(bracelet);
            equipment->weapon      = GET_REDIS_COMMANDER_32(weapon);
            equipment->shield      = GET_REDIS_COMMANDER_32(shield);
            equipment->costume     = GET_REDIS_COMMANDER_32(costume);
            equipment->itemUnk3    = GET_REDIS_COMMANDER_32(itemUnk3);
            equipment->itemUnk4    = GET_REDIS_COMMANDER_32(itemUnk4);
            equipment->itemUnk5    = GET_REDIS_COMMANDER_32(itemUnk5);
            equipment->leg_armor   = GET_REDIS_COMMANDER_32(leg_armor);
            equipment->itemUnk6    = GET_REDIS_COMMANDER_32(itemUnk6);
            equipment->itemUnk7    = GET_REDIS_COMMANDER_32(itemUnk7);
            equipment->ring_left   = GET_REDIS_COMMANDER_32(ring_left);
            equipment->ring_right  = GET_REDIS_COMMANDER_32(ring_right);
            equipment->necklace    = GET_REDIS_COMMANDER_32(necklace);
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


bool redisMoveCommanderSession(Redis *self, RedisCommanderSessionKey *from, RedisCommanderSessionKey *to) {

    bool result = true;
    redisReply *reply = NULL;

    reply = redisCommandDbg(self,
        "RENAME"
        " zone%x:map%x:acc%llx:commanderSession"
        " zone%x:map%x:acc%llx:commanderSession",
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

bool redisDeleteCommanderSession(Redis *self, RedisCommanderSessionKey *key) {

    bool result = true;
    redisReply *reply = NULL;

    // Delete the key from the Redis
    reply = redisCommandDbg(self,
        "DEL zone%x:map%x:acc%llx:commanderSession",
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
