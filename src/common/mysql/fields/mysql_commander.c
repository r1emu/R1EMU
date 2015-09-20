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

int mySqlGetCommandersByAccountId(MySQL *self, uint64_t accountId, CommanderInfo **commanders) {
    MYSQL_ROW row;

    dbg("accountId: %11x", accountId);

    int commandersCount = 0;

    // flush the commander
    if (mySqlQuery(self, "SELECT "
                   " " MYSQL_COMMANDER_FIELD_commander_id_str
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
                   "WHERE account_id = %11x "
                   "AND is_deleted = 'n' "
                   "LIMIT 20 ", // Add a limit, just in case
                   accountId)) {
        error("SQL Error : %s" , mysql_error(self->handle));
        return -1;
    }
    else {
        commandersCount = mysql_num_rows(self->result);
        if (commandersCount == 0) {
            dbg("No commanders for this accountId");
        } else {

            // Allocate memory for all commanders
            *commanders = malloc(sizeof(**commanders) * commandersCount);


            int i = 0;
            while ((row = mysql_fetch_row(self->result))) {

                CommanderInfo *thisCommander = &(*commanders)[i];

                thisCommander->pcId = strtoll(row[MYSQL_COMMANDER_FIELD_commander_id], NULL, 10);
                thisCommander->commanderId = strtoll(row[MYSQL_COMMANDER_FIELD_commander_id], NULL, 10);
                thisCommander->socialInfoId = strtoll(row[MYSQL_COMMANDER_FIELD_commander_id], NULL, 10);
                //thisCommander->mapId = strtol(row[MYSQL_COMMANDER_FIELD_map_id], NULL, 10);
                strncpy(thisCommander->appearance.commanderName, row[MYSQL_COMMANDER_FIELD_commanderName], sizeof(thisCommander->appearance.commanderName));
                thisCommander->appearance.jobId = strtol(row[MYSQL_COMMANDER_FIELD_job_id], NULL, 10);
                thisCommander->appearance.classId = strtol(row[MYSQL_COMMANDER_FIELD_class_id], NULL, 10);
                thisCommander->appearance.hairId = strtol(row[MYSQL_COMMANDER_FIELD_hair_id], NULL, 10);
                thisCommander->appearance.gender = strtol(row[MYSQL_COMMANDER_FIELD_gender], NULL, 10);
                thisCommander->appearance.level = strtol(row[MYSQL_COMMANDER_FIELD_level], NULL, 10);

                CommanderEquipment *equipment = &thisCommander->appearance.equipment;

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

                /// debug info
                //commanderPrint(&thisCommander->base);
                //commanderEquipmentPrint(equipment);
                i++;
            }
        }
    }
    return commandersCount;
}

bool mySqlCommanderCreate(MySQL *self, uint64_t accountId, CommanderCreateInfo *commanderCreate) {
    //MYSQL_ROW row;

    CommanderAppearance *commander = &commanderCreate->appearance;

    // flush the commander
    //if (mySqlQuery(self, "CALL bCreateCommander(%11x) "
    if (mySqlQuery(self, "INSERT INTO commanders "
                   "SET account_id = '%11x'"
                   ", " MYSQL_COMMANDER_FIELD_commanderName_str " = '%s'"
                   ", " MYSQL_COMMANDER_FIELD_level_str " = %i"
                   ", " MYSQL_COMMANDER_FIELD_exp_str " = %i"
                   ", " MYSQL_COMMANDER_FIELD_gender_str " = %i"
                   ", " MYSQL_COMMANDER_FIELD_job_id_str " = %i"
                   ", " MYSQL_COMMANDER_FIELD_class_id_str " = %i"
                   ", " MYSQL_COMMANDER_FIELD_hair_id_str " = %i"
                   ", " MYSQL_COMMANDER_FIELD_map_id_str " = %i"
                   ", " MYSQL_COMMANDER_FIELD_position_x_str " = %f"
                   ", " MYSQL_COMMANDER_FIELD_position_y_str " = %f"
                   ", " MYSQL_COMMANDER_FIELD_position_z_str " = %f"
                   ", " MYSQL_COMMANDER_FIELD_hp_str " = %i"
                   ", " MYSQL_COMMANDER_FIELD_mp_str " = %i"
                   ", " MYSQL_COMMANDER_FIELD_eqslot_head_top_str " = %i"
                   ", " MYSQL_COMMANDER_FIELD_eqslot_head_middle_str " = %i"
                   ", " MYSQL_COMMANDER_FIELD_eqslot_unkown_1_str " = %i"
                   ", " MYSQL_COMMANDER_FIELD_eqslot_body_armor_str " = %i"
                   ", " MYSQL_COMMANDER_FIELD_eqslot_gloves_str " = %i"
                   ", " MYSQL_COMMANDER_FIELD_eqslot_boots_str " = %i"
                   ", " MYSQL_COMMANDER_FIELD_eqslot_helmet_str " = %i"
                   ", " MYSQL_COMMANDER_FIELD_eqslot_bracelet_str " = %i"
                   ", " MYSQL_COMMANDER_FIELD_eqslot_weapon_str " = %i"
                   ", " MYSQL_COMMANDER_FIELD_eqslot_shield_str " = %i"
                   ", " MYSQL_COMMANDER_FIELD_eqslot_costume_str " = %i"
                   ", " MYSQL_COMMANDER_FIELD_eqslot_unkown_3_str " = %i"
                   ", " MYSQL_COMMANDER_FIELD_eqslot_unkown_4_str " = %i"
                   ", " MYSQL_COMMANDER_FIELD_eqslot_unkown_5_str " = %i"
                   ", " MYSQL_COMMANDER_FIELD_eqslot_leg_armor_str " = %i"
                   ", " MYSQL_COMMANDER_FIELD_eqslot_unkown_6_str " = %i"
                   ", " MYSQL_COMMANDER_FIELD_eqslot_unkown_7_str " = %i"
                   ", " MYSQL_COMMANDER_FIELD_eqslot_ring_left_str " = %i"
                   ", " MYSQL_COMMANDER_FIELD_eqslot_ring_right_str " = %i"
                   ", " MYSQL_COMMANDER_FIELD_eqslot_necklace_str " = %i",
                   accountId,
                   commander->commanderName,
                   commander->level,
                   commanderCreate->maxXP,
                   commander->gender,
                   commander->jobId,
                   commander->classId,
                   commander->hairId,
                   commanderCreate->mapId,
                   commanderCreate->pos.x, /// Using world pos, and should be barrack pos
                   commanderCreate->pos.y, /// Using world pos, and should be barrack pos
                   commanderCreate->pos.z, /// Using world pos, and should be barrack pos
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
                   commander->equipment.necklace
                   )) {
        error("SQL Error : %s" , mysql_error(self->handle));
        return false;
    }
    // check last insert id
    return true;


}
