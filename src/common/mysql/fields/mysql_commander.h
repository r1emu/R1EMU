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

#define MYSQL_COMMANDER_FIELD_commander_id_str "commander_id"
#define MYSQL_COMMANDER_FIELD_commanderName_str "commander_name"
#define MYSQL_COMMANDER_FIELD_time_deleted_str "time_deleted"
#define MYSQL_COMMANDER_FIELD_level_str "level"
#define MYSQL_COMMANDER_FIELD_exp_str "exp"
#define MYSQL_COMMANDER_FIELD_gender_str "gender"
#define MYSQL_COMMANDER_FIELD_job_id_str "job_id"
#define MYSQL_COMMANDER_FIELD_class_id_str "class_id"
#define MYSQL_COMMANDER_FIELD_hair_id_str "hair_id"
#define MYSQL_COMMANDER_FIELD_map_id_str "map_id"
#define MYSQL_COMMANDER_FIELD_position_x_str "position_x"
#define MYSQL_COMMANDER_FIELD_position_y_str "position_y"
#define MYSQL_COMMANDER_FIELD_position_z_str "position_z"
#define MYSQL_COMMANDER_FIELD_hp_str "hp"
#define MYSQL_COMMANDER_FIELD_mp_str "mp"
#define MYSQL_COMMANDER_FIELD_eqslot_head_top_str "eqslot_head_top"
#define MYSQL_COMMANDER_FIELD_eqslot_head_middle_str "eqslot_head_middle"
#define MYSQL_COMMANDER_FIELD_eqslot_unkown_1_str "eqslot_unkown_1"
#define MYSQL_COMMANDER_FIELD_eqslot_body_armor_str "eqslot_body_armor"
#define MYSQL_COMMANDER_FIELD_eqslot_gloves_str "eqslot_gloves"
#define MYSQL_COMMANDER_FIELD_eqslot_boots_str "eqslot_boots"
#define MYSQL_COMMANDER_FIELD_eqslot_helmet_str "eqslot_helmet_2"
#define MYSQL_COMMANDER_FIELD_eqslot_bracelet_str "eqslot_bracelet"
#define MYSQL_COMMANDER_FIELD_eqslot_weapon_str "eqslot_weapon"
#define MYSQL_COMMANDER_FIELD_eqslot_shield_str "eqslot_shield"
#define MYSQL_COMMANDER_FIELD_eqslot_costume_str "eqslot_costume"
#define MYSQL_COMMANDER_FIELD_eqslot_unkown_3_str "eqslot_unkown_3"
#define MYSQL_COMMANDER_FIELD_eqslot_unkown_4_str "eqslot_unkown_4"
#define MYSQL_COMMANDER_FIELD_eqslot_unkown_5_str "eqslot_unkown_5"
#define MYSQL_COMMANDER_FIELD_eqslot_leg_armor_str "eqslot_leg_armor"
#define MYSQL_COMMANDER_FIELD_eqslot_unkown_6_str "eqslot_unkown_6"
#define MYSQL_COMMANDER_FIELD_eqslot_unkown_7_str "eqslot_unkown_7"
#define MYSQL_COMMANDER_FIELD_eqslot_ring_left_str "eqslot_ring_left"
#define MYSQL_COMMANDER_FIELD_eqslot_ring_right_str "eqslot_ring_right"
#define MYSQL_COMMANDER_FIELD_eqslot_necklace_str "eqslot_necklace"

enum MySqlCommanderFields {
    MYSQL_COMMANDER_FIELD_commander_id,
	MYSQL_COMMANDER_FIELD_commanderName,
	MYSQL_COMMANDER_FIELD_time_deleted,
	MYSQL_COMMANDER_FIELD_level,
	MYSQL_COMMANDER_FIELD_exp,
	MYSQL_COMMANDER_FIELD_gender,
	MYSQL_COMMANDER_FIELD_job_id,
	MYSQL_COMMANDER_FIELD_class_id,
	MYSQL_COMMANDER_FIELD_hair_id,
	MYSQL_COMMANDER_FIELD_map_id,
	MYSQL_COMMANDER_FIELD_position_x,
	MYSQL_COMMANDER_FIELD_position_y,
	MYSQL_COMMANDER_FIELD_position_z,
	MYSQL_COMMANDER_FIELD_hp,
	MYSQL_COMMANDER_FIELD_mp,
	MYSQL_COMMANDER_FIELD_eqslot_head_top,
	MYSQL_COMMANDER_FIELD_eqslot_head_middle,
	MYSQL_COMMANDER_FIELD_eqslot_unkown_1,
	MYSQL_COMMANDER_FIELD_eqslot_body_armor,
	MYSQL_COMMANDER_FIELD_eqslot_gloves,
	MYSQL_COMMANDER_FIELD_eqslot_boots,
	MYSQL_COMMANDER_FIELD_eqslot_helmet,
	MYSQL_COMMANDER_FIELD_eqslot_bracelet,
	MYSQL_COMMANDER_FIELD_eqslot_weapon,
	MYSQL_COMMANDER_FIELD_eqslot_shield,
	MYSQL_COMMANDER_FIELD_eqslot_costume,
	MYSQL_COMMANDER_FIELD_eqslot_unkown_3,
	MYSQL_COMMANDER_FIELD_eqslot_unkown_4,
	MYSQL_COMMANDER_FIELD_eqslot_unkown_5,
	MYSQL_COMMANDER_FIELD_eqslot_leg_armor,
	MYSQL_COMMANDER_FIELD_eqslot_unkown_6,
	MYSQL_COMMANDER_FIELD_eqslot_unkown_7,
	MYSQL_COMMANDER_FIELD_eqslot_ring_left,
	MYSQL_COMMANDER_FIELD_eqslot_ring_right,
	MYSQL_COMMANDER_FIELD_eqslot_necklace,
	MYSQL_COMMANDER_FIELD_COUNT
} MySqlCommanderFields;

int mySqlGetCommandersByAccountId(MySQL *self, uint64_t accountId, CommanderInfo **commanders);

bool mySqlCommanderCreate(MySQL *self, uint64_t accountId, CommanderCreateInfo *commanderCreate);
