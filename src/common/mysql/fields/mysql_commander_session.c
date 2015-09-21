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

bool mySqlCommanderInsertCommanderInfo(MySQL *self, CommanderInfo *cInfo) {

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
        cInfo->commanderId,
        cInfo->appearance.accountId,
        cInfo->appearance.commanderName,
        cInfo->appearance.familyName,
        cInfo->appearance.classId,
        cInfo->appearance.gender,
        cInfo->appearance.level,
        cInfo->appearance.equipment.head_top,
        cInfo->appearance.equipment.head_middle,
        cInfo->appearance.equipment.necklace,
        cInfo->appearance.equipment.body_armor,
        cInfo->appearance.equipment.leg_armor,
        cInfo->appearance.equipment.gloves,
        cInfo->appearance.equipment.boots,
        cInfo->appearance.equipment.weapon,
        cInfo->appearance.equipment.shield,
        cInfo->appearance.equipment.costume,
        cInfo->appearance.equipment.bracelet,
        cInfo->appearance.equipment.ring_left,
        cInfo->appearance.equipment.ring_right,
        cInfo->appearance.hairId,
        cInfo->pos.x,
        cInfo->pos.y,
        cInfo->pos.z,
        cInfo->currentXP,
        cInfo->currentHP,
        cInfo->currentSP))
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

    Commander *commander = &commanderSession->currentCommander;
    CommanderInfo *cInfo = &commander->info;

    // flush the commander
    if (mySqlQuery(self, "SELECT count(*) FROM commander WHERE commander_id = %u", cInfo->commanderId)) {
        error("SQL Error : %s" , mysql_error(self->handle));
        goto cleanup;
    }

    count = mysql_fetch_row(self->result);

    if (atoi(count[0]) == 0) {
        // insert the commander
        if (!(mySqlCommanderInsertCommanderInfo(self, cInfo))) {
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
