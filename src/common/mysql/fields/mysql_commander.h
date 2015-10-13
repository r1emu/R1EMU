/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file mysql_commander.h
 * @brief Mysql fields for commander structure
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#pragma once

#include "mysql_session.h"
#include "barrack_server/barrack_handler/barrack_builder.h"

#define FOREACH_MYSQL_COMMANDER(GENERATOR)                          \
    GENERATOR(MYSQL_COMMANDER, commander_id, '%llu')                \
    GENERATOR(MYSQL_COMMANDER, is_deleted, '%c')                    \
    GENERATOR(MYSQL_COMMANDER, account_id, '%llu')                  \
    GENERATOR(MYSQL_COMMANDER, commander_name, '%s')                \
    GENERATOR(MYSQL_COMMANDER, time_deleted, FROM_UNIXTIME('%u'))   \
    GENERATOR(MYSQL_COMMANDER, level, '%u')                         \
    GENERATOR(MYSQL_COMMANDER, exp, '%u')                           \
    GENERATOR(MYSQL_COMMANDER, gender, '%u')                        \
    GENERATOR(MYSQL_COMMANDER, job_id, '%u')                        \
    GENERATOR(MYSQL_COMMANDER, class_id, '%u')                      \
    GENERATOR(MYSQL_COMMANDER, hair_id, '%u')                       \
    GENERATOR(MYSQL_COMMANDER, map_id, '%u')                        \
    GENERATOR(MYSQL_COMMANDER, position_x, '%f')                    \
    GENERATOR(MYSQL_COMMANDER, position_y, '%f')                    \
    GENERATOR(MYSQL_COMMANDER, position_z, '%f')                    \
    GENERATOR(MYSQL_COMMANDER, barrack_position_x, '%f')            \
    GENERATOR(MYSQL_COMMANDER, barrack_position_y, '%f')            \
    GENERATOR(MYSQL_COMMANDER, barrack_position_z, '%f')            \
    GENERATOR(MYSQL_COMMANDER, hp, '%d')                            \
    GENERATOR(MYSQL_COMMANDER, mp, '%d')                            \
    GENERATOR(MYSQL_COMMANDER, eqslot_head_top, '%u')               \
    GENERATOR(MYSQL_COMMANDER, eqslot_head_middle, '%u')            \
    GENERATOR(MYSQL_COMMANDER, eqslot_unknown_1, '%u')               \
    GENERATOR(MYSQL_COMMANDER, eqslot_body_armor, '%u')             \
    GENERATOR(MYSQL_COMMANDER, eqslot_gloves, '%u')                 \
    GENERATOR(MYSQL_COMMANDER, eqslot_boots, '%u')                  \
    GENERATOR(MYSQL_COMMANDER, eqslot_helmet, '%u')                 \
    GENERATOR(MYSQL_COMMANDER, eqslot_bracelet, '%u')               \
    GENERATOR(MYSQL_COMMANDER, eqslot_weapon, '%u')                 \
    GENERATOR(MYSQL_COMMANDER, eqslot_shield, '%u')                 \
    GENERATOR(MYSQL_COMMANDER, eqslot_costume, '%u')                \
    GENERATOR(MYSQL_COMMANDER, eqslot_unknown_3, '%u')               \
    GENERATOR(MYSQL_COMMANDER, eqslot_unknown_4, '%u')               \
    GENERATOR(MYSQL_COMMANDER, eqslot_unknown_5, '%u')               \
    GENERATOR(MYSQL_COMMANDER, eqslot_leg_armor, '%u')              \
    GENERATOR(MYSQL_COMMANDER, eqslot_unknown_6, '%u')               \
    GENERATOR(MYSQL_COMMANDER, eqslot_unknown_7, '%u')               \
    GENERATOR(MYSQL_COMMANDER, eqslot_ring_left, '%u')              \
    GENERATOR(MYSQL_COMMANDER, eqslot_ring_right, '%u')             \
    GENERATOR(MYSQL_COMMANDER, eqslot_necklace, '%u')

typedef enum {
    FOREACH_MYSQL_COMMANDER(GENERATE_ENUM)
	MYSQL_COMMANDER_COUNT
} MySqlCommanderEnumField;

/**
 * Request to SQL a set of commanders by the account ID
 */
bool mySqlRequestCommandersByAccountId(MySQL *self, uint64_t accountId, size_t *commandersCount);

/**
 * Convert SQL results to in memory structures
 */
bool mySqlGetCommanders(MySQL *self, char *familyName, Commander **commanders);

bool mySqlCommanderInsert(MySQL *self, Commander *commander);
bool mySqlCommanderUpdate(MySQL *self, CommanderId_t commanderId, Commander *commander);
bool MySqlCommanderDelete(MySQL *self, CommanderId_t commanderId);
bool mySqlCommanderFlush(MySQL *self, Commander *commander);
bool mySqlCommanderSessionFlush(MySQL *self, CommanderSession *commanderSession);
