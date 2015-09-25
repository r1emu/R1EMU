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
#include "mysql_commander_session.h"

bool mySqlCommanderInsertCommander(MySQL *self, Commander *commander) {

    bool status = false;

    // insert a new Commander
    if (mySqlQuery (self,
        "INSERT INTO commander ("
        "`commander_id`, `account_id`, `name`, `family_name`, `class`, "
        "`gender`, `level`, `head_top`, `head_middle`, `necklace`, `body_armor`, `leg_armor`, "
        "`gloves`, `shoes`, `weapon`, `shield`, `costume`, `ring`, `bracelet_left`, "
        "`bracelet_right`, `hair_type`, `last_x`, `last_y`, `last_z`, `current_xp`, "
        "`current_hp`, `current_sp`) VALUES ("
        "%u, %llu, '%s', '%s', %d, "
        "%d, %d, %d, %d, %d, %d, %d, "
        "%d, %d, %d, %d, %d, %d, %d, "
        "%d, %d, %f, %f, %f, "
        "%d, %d, %d)",
        commander->commanderId,
        commander->appearance.accountId,
        commander->appearance.commanderName,
        commander->appearance.familyName,
        commander->appearance.classId,
        commander->appearance.gender,
        commander->appearance.level,
        commander->appearance.equipment.head_top,
        commander->appearance.equipment.head_middle,
        commander->appearance.equipment.necklace,
        commander->appearance.equipment.body_armor,
        commander->appearance.equipment.leg_armor,
        commander->appearance.equipment.gloves,
        commander->appearance.equipment.boots,
        commander->appearance.equipment.weapon,
        commander->appearance.equipment.shield,
        commander->appearance.equipment.costume,
        commander->appearance.equipment.bracelet,
        commander->appearance.equipment.ring_left,
        commander->appearance.equipment.ring_right,
        commander->appearance.hairId,
        commander->pos.x,
        commander->pos.y,
        commander->pos.z,
        commander->currentXP,
        commander->currentHP,
        commander->currentSP))
    {
        error("SQL Error : %s" , mysql_error(self->handle));
        goto cleanup;
    }

    status = true;

cleanup:
    return status;
}

bool mySqlCommanderSessionFlush(MySQL *self, CommanderSession *commanderSession) {

    bool status = false;
    MYSQL_ROW count;

    Commander *commander = commanderSession->currentCommander;

    // flush the commander
    if (mySqlQuery(self, "SELECT count(*) FROM commander WHERE commander_id = %u", commander->commanderId)) {
        error("SQL Error : %s" , mysql_error(self->handle));
        goto cleanup;
    }

    count = mysql_fetch_row(self->result);

    if (atoi(count[0]) == 0) {
        // insert the commander
        if (!(mySqlCommanderInsertCommander(self, commander))) {
            error("Cannot insert a new commander.");
            goto cleanup;
        }
    }
    else {
        // update the commander
        // TODO
    }

    status = true;
cleanup:

    return status;
}
