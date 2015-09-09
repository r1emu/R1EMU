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

bool mySqlCommanderSessionFlush(MySQL *self, CommanderSession *commanderSession) {
    bool status = true;
    MYSQL_ROW count;

    Commander *commander = &commanderSession->currentCommander;
    CommanderInfo *cInfo = &commander->info;

    // flush the commander
    if (mySqlQuery(self, "SELECT count(*) FROM commander WHERE commander_id = %u", cInfo->commanderId)) {
        error("SQL Error : %s" , mysql_error(self->handle));
        status = false;
        goto cleanup;
    }
    else {
        count = mysql_fetch_row(self->result);
        if (atoi(count[0]) == 0) {
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
                cInfo->appareance.accountId,
                cInfo->appareance.commanderName,
                cInfo->appareance.familyName,
                cInfo->appareance.classId,
                cInfo->appareance.gender,
                cInfo->appareance.level,
                cInfo->appareance.equipment.head_top,
                cInfo->appareance.equipment.head_middle,
                cInfo->appareance.equipment.necklace,
                cInfo->appareance.equipment.body_armor,
                cInfo->appareance.equipment.leg_armor,
                cInfo->appareance.equipment.gloves,
                cInfo->appareance.equipment.boots,
                cInfo->appareance.equipment.weapon,
                cInfo->appareance.equipment.shield,
                cInfo->appareance.equipment.costume,
                cInfo->appareance.equipment.bracelet,
                cInfo->appareance.equipment.ring_left,
                cInfo->appareance.equipment.ring_right,
                cInfo->appareance.hairId,
                cInfo->pos.x,
                cInfo->pos.y,
                cInfo->pos.z,
                cInfo->currentXP,
                cInfo->currentHP,
                cInfo->currentSP))
            {
                error("SQL Error : %s" , mysql_error(self->handle));
                status = false;
                goto cleanup;
            }
            dbg("Inserting a new commander");
        }
        else {
            // update the commander
            dbg("Updating the commander");
            // TODO
        }
    }

cleanup:

    return status;
}
