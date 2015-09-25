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

#include "mysql_commander.h"
#include "common/commander/commander.h"

bool mySqlGetCommanders(MySQL *self, Commander *commanders) {

    MYSQL_ROW row;

    dbg("mySqlGetCommanders count: %d", mysql_num_rows(self->result));

    for (int i = 0; (row = mysql_fetch_row(self->result)); i++) {

        Commander *curCommander = &commanders[i];
        commanderInit(curCommander);

        curCommander->mapId = strtol(row[MYSQL_COMMANDER_FIELD_map_id], NULL, 10);

        CommanderInfo *cInfo = &curCommander->info;
        cInfo->pcId = strtoll(row[MYSQL_COMMANDER_FIELD_commander_id], NULL, 10);
        cInfo->commanderId = strtoll(row[MYSQL_COMMANDER_FIELD_commander_id], NULL, 10);
        cInfo->socialInfoId = strtoll(row[MYSQL_COMMANDER_FIELD_commander_id], NULL, 10);

        CommanderAppearance *appearance = &cInfo->appearance;
        strncpy(appearance->commanderName, row[MYSQL_COMMANDER_FIELD_commanderName], sizeof(appearance->commanderName));
        appearance->jobId = strtol(row[MYSQL_COMMANDER_FIELD_job_id], NULL, 10);
        appearance->classId = strtol(row[MYSQL_COMMANDER_FIELD_class_id], NULL, 10);
        appearance->hairId = strtol(row[MYSQL_COMMANDER_FIELD_hair_id], NULL, 10);
        appearance->gender = strtol(row[MYSQL_COMMANDER_FIELD_gender], NULL, 10);
        appearance->level = strtol(row[MYSQL_COMMANDER_FIELD_level], NULL, 10);

        CommanderEquipment *equipment = &appearance->equipment;
        equipment->head_top = strtol(row[MYSQL_COMMANDER_FIELD_eqslot_head_top], NULL, 10);
        equipment->head_middle = strtol(row[MYSQL_COMMANDER_FIELD_eqslot_head_middle], NULL, 10);
        equipment->itemUnk1 = strtol(row[MYSQL_COMMANDER_FIELD_eqslot_unkown_1], NULL, 10);
        equipment->body_armor = strtol(row[MYSQL_COMMANDER_FIELD_eqslot_body_armor], NULL, 10);
        equipment->gloves = strtol(row[MYSQL_COMMANDER_FIELD_eqslot_gloves], NULL, 10);
        equipment->boots = strtol(row[MYSQL_COMMANDER_FIELD_eqslot_boots], NULL, 10);
        equipment->helmet = strtol(row[MYSQL_COMMANDER_FIELD_eqslot_helmet], NULL, 10);
        equipment->bracelet = strtol(row[MYSQL_COMMANDER_FIELD_eqslot_bracelet], NULL, 10);
        equipment->weapon = strtol(row[MYSQL_COMMANDER_FIELD_eqslot_weapon], NULL, 10);
        equipment->shield = strtol(row[MYSQL_COMMANDER_FIELD_eqslot_shield], NULL, 10);
        equipment->costume = strtol(row[MYSQL_COMMANDER_FIELD_eqslot_costume], NULL, 10);
        equipment->itemUnk3 = strtol(row[MYSQL_COMMANDER_FIELD_eqslot_unkown_3], NULL, 10);
        equipment->itemUnk4 = strtol(row[MYSQL_COMMANDER_FIELD_eqslot_unkown_4], NULL, 10);
        equipment->itemUnk5 = strtol(row[MYSQL_COMMANDER_FIELD_eqslot_unkown_5], NULL, 10);
        equipment->leg_armor = strtol(row[MYSQL_COMMANDER_FIELD_eqslot_leg_armor], NULL, 10);
        equipment->itemUnk6 = strtol(row[MYSQL_COMMANDER_FIELD_eqslot_unkown_6], NULL, 10);
        equipment->itemUnk7 = strtol(row[MYSQL_COMMANDER_FIELD_eqslot_unkown_7], NULL, 10);
        equipment->ring_left = strtol(row[MYSQL_COMMANDER_FIELD_eqslot_ring_left], NULL, 10);
        equipment->ring_right = strtol(row[MYSQL_COMMANDER_FIELD_eqslot_ring_right], NULL, 10);
        equipment->necklace = strtol(row[MYSQL_COMMANDER_FIELD_eqslot_necklace], NULL, 10);

    }

    return true;
}

bool mySqlRequestCommandersByAccountId(MySQL *self, uint64_t accountId, size_t *_commandersCount) {

    bool status = false;
    size_t commandersCount = 0;

    // check if current commander exists
    if (mySqlQuery(self, "SELECT "
        " "  MYSQL_COMMANDER_FIELD_commander_id_str
        ", " MYSQL_COMMANDER_FIELD_commanderName_str
        ", " MYSQL_COMMANDER_FIELD_time_deleted_str
        ", " MYSQL_COMMANDER_FIELD_level_str
        ", " MYSQL_COMMANDER_FIELD_exp_str
        ", " MYSQL_COMMANDER_FIELD_gender_str
        ", " MYSQL_COMMANDER_FIELD_job_id_str
        ", " MYSQL_COMMANDER_FIELD_class_id_str
        ", " MYSQL_COMMANDER_FIELD_hair_id_str
        ", " MYSQL_COMMANDER_FIELD_map_id_str
        ", " MYSQL_COMMANDER_FIELD_position_x_str
        ", " MYSQL_COMMANDER_FIELD_position_y_str
        ", " MYSQL_COMMANDER_FIELD_position_z_str
        ", " MYSQL_COMMANDER_FIELD_hp_str
        ", " MYSQL_COMMANDER_FIELD_mp_str
        ", " MYSQL_COMMANDER_FIELD_eqslot_head_top_str
        ", " MYSQL_COMMANDER_FIELD_eqslot_head_middle_str
        ", " MYSQL_COMMANDER_FIELD_eqslot_unkown_1_str
        ", " MYSQL_COMMANDER_FIELD_eqslot_body_armor_str
        ", " MYSQL_COMMANDER_FIELD_eqslot_gloves_str
        ", " MYSQL_COMMANDER_FIELD_eqslot_boots_str
        ", " MYSQL_COMMANDER_FIELD_eqslot_helmet_str
        ", " MYSQL_COMMANDER_FIELD_eqslot_bracelet_str
        ", " MYSQL_COMMANDER_FIELD_eqslot_weapon_str
        ", " MYSQL_COMMANDER_FIELD_eqslot_shield_str
        ", " MYSQL_COMMANDER_FIELD_eqslot_costume_str
        ", " MYSQL_COMMANDER_FIELD_eqslot_unkown_3_str
        ", " MYSQL_COMMANDER_FIELD_eqslot_unkown_4_str
        ", " MYSQL_COMMANDER_FIELD_eqslot_unkown_5_str
        ", " MYSQL_COMMANDER_FIELD_eqslot_leg_armor_str
        ", " MYSQL_COMMANDER_FIELD_eqslot_unkown_6_str
        ", " MYSQL_COMMANDER_FIELD_eqslot_unkown_7_str
        ", " MYSQL_COMMANDER_FIELD_eqslot_ring_left_str
        ", " MYSQL_COMMANDER_FIELD_eqslot_ring_right_str
        ", " MYSQL_COMMANDER_FIELD_eqslot_necklace_str
        " FROM commanders "
        "WHERE account_id = %llx "
        "AND is_deleted = 'n' ",
        accountId))
    {
        error("SQL Error : %s" , mysql_error(self->handle));
        goto cleanup;
    }

    commandersCount = mysql_num_rows(self->result);

    dbg("commanders found in account %d", commandersCount);

    *_commandersCount = commandersCount;
    status = true;

cleanup:
    return status;
}

bool mySqlCommanderInsert(MySQL *self, uint64_t accountId, Commander *commanderToCreate) {

    bool status = false;
    CommanderAppearance *commander = &commanderToCreate->info.appearance;

    // Insert a new commander
    if (mySqlQuery(self, "INSERT INTO commanders "
       "SET account_id = '%llx'"
       ", " MYSQL_COMMANDER_FIELD_commanderName_str " = '%s'"
       ", " MYSQL_COMMANDER_FIELD_level_str " = %d"
       ", " MYSQL_COMMANDER_FIELD_exp_str " = %d"
       ", " MYSQL_COMMANDER_FIELD_gender_str " = %d"
       ", " MYSQL_COMMANDER_FIELD_job_id_str " = %d"
       ", " MYSQL_COMMANDER_FIELD_class_id_str " = %d"
       ", " MYSQL_COMMANDER_FIELD_hair_id_str " = %d"
       ", " MYSQL_COMMANDER_FIELD_map_id_str " = %d"
       ", " MYSQL_COMMANDER_FIELD_position_x_str " = %f"
       ", " MYSQL_COMMANDER_FIELD_position_y_str " = %f"
       ", " MYSQL_COMMANDER_FIELD_position_z_str " = %f"
       ", " MYSQL_COMMANDER_FIELD_hp_str " = %d"
       ", " MYSQL_COMMANDER_FIELD_mp_str " = %d"
       ", " MYSQL_COMMANDER_FIELD_eqslot_head_top_str " = %d"
       ", " MYSQL_COMMANDER_FIELD_eqslot_head_middle_str " = %d"
       ", " MYSQL_COMMANDER_FIELD_eqslot_unkown_1_str " = %d"
       ", " MYSQL_COMMANDER_FIELD_eqslot_body_armor_str " = %d"
       ", " MYSQL_COMMANDER_FIELD_eqslot_gloves_str " = %d"
       ", " MYSQL_COMMANDER_FIELD_eqslot_boots_str " = %d"
       ", " MYSQL_COMMANDER_FIELD_eqslot_helmet_str " = %d"
       ", " MYSQL_COMMANDER_FIELD_eqslot_bracelet_str " = %d"
       ", " MYSQL_COMMANDER_FIELD_eqslot_weapon_str " = %d"
       ", " MYSQL_COMMANDER_FIELD_eqslot_shield_str " = %d"
       ", " MYSQL_COMMANDER_FIELD_eqslot_costume_str " = %d"
       ", " MYSQL_COMMANDER_FIELD_eqslot_unkown_3_str " = %d"
       ", " MYSQL_COMMANDER_FIELD_eqslot_unkown_4_str " = %d"
       ", " MYSQL_COMMANDER_FIELD_eqslot_unkown_5_str " = %d"
       ", " MYSQL_COMMANDER_FIELD_eqslot_leg_armor_str " = %d"
       ", " MYSQL_COMMANDER_FIELD_eqslot_unkown_6_str " = %d"
       ", " MYSQL_COMMANDER_FIELD_eqslot_unkown_7_str " = %d"
       ", " MYSQL_COMMANDER_FIELD_eqslot_ring_left_str " = %d"
       ", " MYSQL_COMMANDER_FIELD_eqslot_ring_right_str " = %d"
       ", " MYSQL_COMMANDER_FIELD_eqslot_necklace_str " = %d",

       accountId,
       commander->commanderName,
       commander->level,
       commanderToCreate->info.maxXP,
       commander->gender,
       commander->jobId,
       commander->classId,
       commander->hairId,
       commanderToCreate->mapId,
       commanderToCreate->info.pos.x, /// FIXME : Using world pos, and should be barrack pos
       commanderToCreate->info.pos.y, /// FIXME : Using world pos, and should be barrack pos
       commanderToCreate->info.pos.z, /// FIXME : Using world pos, and should be barrack pos
       10,
       10,
       commander->equipment.head_top,
       commander->equipment.head_middle,
       commander->equipment.itemUnk1,
       commander->equipment.body_armor,
       commander->equipment.gloves,
       commander->equipment.boots,
       commander->equipment.helmet,
       commander->equipment.bracelet,
       commander->equipment.weapon,
       commander->equipment.shield,
       commander->equipment.costume,
       commander->equipment.itemUnk3,
       commander->equipment.itemUnk4,
       commander->equipment.itemUnk5,
       commander->equipment.leg_armor,
       commander->equipment.itemUnk6,
       commander->equipment.itemUnk7,
       commander->equipment.ring_left,
       commander->equipment.ring_right,
       commander->equipment.necklace))
    {
        error("SQL Error : %s" , mysql_error(self->handle));
        goto cleanup;
    }

    uint64_t commanderId = mysql_insert_id(self->handle);

    commanderToCreate->info.commanderId = commanderId;

    // TODO : check last insert id


    status = true;

cleanup:
    return status;
}

bool MySqlCommanderDelete(MySQL *self, uint64_t commanderId) {

    bool status = false;

// Insert a new commander
    if (mySqlQuery(self, "DELETE FROM commanders "
       "WHERE commander_id = '%llu'",
       commanderId))
    {
        error("SQL Error : %s" , mysql_error(self->handle));
        goto cleanup;
    }

    status = true;

cleanup:
    return status;
}
