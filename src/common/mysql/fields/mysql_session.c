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

#include "mysql_session.h"

void mySqlSessionFlush(MySQL *self, Session *session) {
    MYSQL_ROW count;

    // flush the commander
    if (mySqlQuery(self, "SELECT count(*) FROM commander WHERE commander_id = %u",
                   session->game.commanderSession.currentCommander.commanderId)) {
        error("SQL Error : %s" , mysql_error(self->handle));
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
                "`bracelet_right`, `hair_type`, `last_x`, `last_y`, `current_xp`, "
                "`current_hp`, `current_sp`) VALUES ("
                "%u, %lu, '%s', '%s', %d, "
                "%d, %d, %d, %d, %d, %d, %d, "
                "%d, %d, %d, %d, %d, %d, %d, "
                "%d, %d, %d, %d, %d, "
                "%d, %d)",
                session->game.commanderSession.currentCommander.commanderId,
                session->game.commanderSession.currentCommander.appareance.accountId,
                session->game.commanderSession.currentCommander.appareance.commanderName,
                session->game.commanderSession.currentCommander.appareance.familyName,
                session->game.commanderSession.currentCommander.appareance.classId,
                session->game.commanderSession.currentCommander.appareance.gender,
                session->game.commanderSession.currentCommander.appareance.level,
                session->game.commanderSession.currentCommander.appareance.equipment.head_top,
                session->game.commanderSession.currentCommander.appareance.equipment.head_middle,
                session->game.commanderSession.currentCommander.appareance.equipment.necklace,
                session->game.commanderSession.currentCommander.appareance.equipment.body_armor,
                session->game.commanderSession.currentCommander.appareance.equipment.leg_armor,
                session->game.commanderSession.currentCommander.appareance.equipment.gloves,
                session->game.commanderSession.currentCommander.appareance.equipment.boots,
                session->game.commanderSession.currentCommander.appareance.equipment.weapon,
                session->game.commanderSession.currentCommander.appareance.equipment.shield,
                session->game.commanderSession.currentCommander.appareance.equipment.costume,
                session->game.commanderSession.currentCommander.appareance.equipment.bracelet,
                session->game.commanderSession.currentCommander.appareance.equipment.ring_left,
                session->game.commanderSession.currentCommander.appareance.equipment.ring_right,
                session->game.commanderSession.currentCommander.appareance.hairId,
                session->game.commanderSession.currentCommander.pos.x,
                session->game.commanderSession.currentCommander.pos.z,
                session->game.commanderSession.currentCommander.currentXP,
                session->game.commanderSession.currentCommander.currentHP,
                session->game.commanderSession.currentCommander.currentSP))
            {
                error("SQL Error : %s" , mysql_error(self->handle));
            }
            dbg("Inserting a new commander");
        }
        else {
            // update the commander
            dbg("Updating the commander");
        }
    }
}
