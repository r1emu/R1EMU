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
#include "common/actor/item/item_factory.h"

static const char *mySqlCommanderStrFields[] = {
    FOREACH_MYSQL_COMMANDER(GENERATE_STRING)
};

static const char *mySqlCommanderTypeFields[] = {
    FOREACH_MYSQL_COMMANDER(GENERATE_TYPE)
};

bool mySqlCommanderSessionFlush(MySQL *self, CommanderSession *commanderSession) {

    bool status = false;

    Commander *commander = commanderSession->currentCommander;
    if (!(mySqlCommanderFlush(self, commander))) {
        error("Cannot flush commander.");
        goto cleanup;
    }

    status = true;
cleanup:

    return status;
}

bool mySqlCommanderFlush(MySQL *self, Commander *commander) {

    bool status = false;
    MYSQL_ROW count;

    // flush the commander
    if (mySqlQuery(self, "SELECT count(*) FROM commanders WHERE commander_id = %u", commander->commanderId)) {
        error("SQL Error : %s" , mysql_error(self->handle));
        goto cleanup;
    }

    count = mysql_fetch_row(self->result);

    if (atoi(count[0]) == 0) {
        // insert the commander
        if (!(mySqlCommanderInsert(self, commander))) {
            error("Cannot insert a new commander.");
            goto cleanup;
        }
    }
    else {
        if (!(mySqlCommanderUpdate(self, commander->commanderId, commander))) {
            error("Cannot update the commander '%llx'.", commander->accountId);
            goto cleanup;
        }
    }

    status = true;
cleanup:

    return status;
}

bool mySqlGetCommanders(MySQL *self, char *familyName, Commander **commanders) {

    MYSQL_ROW row;

    for (int i = 0; (row = mysql_fetch_row(self->result)); i++) {
        Commander *curCommander = commanders[i];
        commanderInit(curCommander);

        strncpy(curCommander->familyName, familyName, sizeof(curCommander->familyName));
        strncpy(curCommander->commanderName, row[MYSQL_COMMANDER_commander_name], sizeof(curCommander->commanderName));
        curCommander->accountId = strtol(row[MYSQL_COMMANDER_account_id], NULL, 10);
        curCommander->classId = strtol(row[MYSQL_COMMANDER_class_id], NULL, 10);
        curCommander->jobId = strtol(row[MYSQL_COMMANDER_job_id], NULL, 10);
        curCommander->gender = strtol(row[MYSQL_COMMANDER_gender], NULL, 10);
        curCommander->level = strtol(row[MYSQL_COMMANDER_level], NULL, 10);
        curCommander->hairId = strtol(row[MYSQL_COMMANDER_hair_id], NULL, 10);
        curCommander->pose = 0; // IDLE
        curCommander->pos = PositionXYZ_decl(
            strtof(row[MYSQL_COMMANDER_position_x], NULL),
            strtof(row[MYSQL_COMMANDER_position_y], NULL),
            strtof(row[MYSQL_COMMANDER_position_z], NULL)
        );
        curCommander->barrackPos = PositionXYZ_decl(
            strtof(row[MYSQL_COMMANDER_barrack_position_x], NULL),
            strtof(row[MYSQL_COMMANDER_barrack_position_y], NULL),
            strtof(row[MYSQL_COMMANDER_barrack_position_z], NULL)
        );
        /*
        curCommander->dir = PositionXZ_decl(
            strtof(row[MYSQL_COMMANDER_direction_x], NULL),
            strtof(row[MYSQL_COMMANDER_direction_y], NULL),
        );
        */
        curCommander->currentXP = strtol(row[MYSQL_COMMANDER_exp], NULL, 10);
        curCommander->maxXP = 1337; /** TODO : Get max XP from XP tables */
        curCommander->pcId = rand(); /** TODO : Get unique PCID */
        curCommander->socialInfoId = strtoll(row[MYSQL_COMMANDER_commander_id], NULL, 10); /** TODO : Get socialInfoId from MYSQL */
        curCommander->commanderId = strtoll(row[MYSQL_COMMANDER_commander_id], NULL, 10);
        curCommander->currentHP = strtol(row[MYSQL_COMMANDER_hp], NULL, 10);
        curCommander->maxHP = curCommander->currentHP; /** TODO : Get maxHP from MYSQL */
        curCommander->currentSP = strtol(row[MYSQL_COMMANDER_mp], NULL, 10);
        curCommander->maxSP = curCommander->currentSP; /** TODO : Get maxHP from MYSQL */
        curCommander->currentStamina = 25000; /** TODO : Get currentStamina from MYSQL */
        curCommander->maxStamina = curCommander->currentStamina; /** TODO : Get maxStamina from MYSQL */
        curCommander->mapId = strtol(row[MYSQL_COMMANDER_map_id], NULL, 10);

        // load equipped items

        for (ItemEquipmentSlot_t slot = 0; slot < EQSLOT_COUNT; slot++) {
            MySqlCommanderEnumField field = MYSQL_COMMANDER_eqslot_head_top + slot;
            ItemId_t itemId = strtol(row[field], NULL, 10);
            curCommander->inventory.equippedItems[slot] = itemFactoryCreateEquipable(itemId, 1, slot);
        }
    }

    return true;
}

bool mySqlRequestCommandersByAccountId(MySQL *self, uint64_t accountId, size_t *commandersCount) {

    bool status = false;
	char query[MAX_QUERY_SIZE] = "SELECT ";

	for (MySqlCommanderEnumField field = 0; field < MYSQL_COMMANDER_COUNT; field++) {
        snprintf(query, sizeof(query), "%s%s, ", query, mySqlCommanderStrFields[field]);
        if (field == MYSQL_COMMANDER_COUNT - 1) {
            // No comma for the last field
            query[strlen(query) - strlen(", ")] = ' ';
        }
	}

	snprintf(query, sizeof(query), "%s FROM commanders ", query);
	snprintf(query, sizeof(query), "%s WHERE %s = %s AND %s = 'n'", query,
        mySqlCommanderStrFields[MYSQL_COMMANDER_account_id],
        mySqlCommanderTypeFields[MYSQL_COMMANDER_account_id],
        mySqlCommanderStrFields[MYSQL_COMMANDER_is_deleted]
    );

    // Check if current commander exists
    if (mySqlQuery(self, query, accountId)) {
        error("SQL Error : %s" , mysql_error(self->handle));
        goto cleanup;
    }

    *commandersCount = mysql_num_rows(self->result);
    status = true;

cleanup:
    return status;
}

bool mySqlCommanderUpdate(MySQL *self, CommanderId_t commanderId, Commander *commander) {

    bool status = false;

    // Insert a new commander
	char query[MAX_QUERY_SIZE] = "UPDATE commanders SET ";

	for (MySqlCommanderEnumField field = 0; field < MYSQL_COMMANDER_COUNT; field++) {
        // Exceptions :
        if (field == MYSQL_COMMANDER_commander_id
         || field == MYSQL_COMMANDER_is_deleted
         || field == MYSQL_COMMANDER_time_deleted) {
            continue;
        }

        snprintf(query, sizeof(query), "%s%s = %s, ", query,
            mySqlCommanderStrFields[field], mySqlCommanderTypeFields[field]);

        if (field == MYSQL_COMMANDER_COUNT - 1) {
            // No comma for the last field
            query[strlen(query) - strlen(", ")] = ' ';
        }
	}

	snprintf(query, sizeof(query), "%s WHERE %s = %s AND %s = 'n'", query,
        mySqlCommanderStrFields[MYSQL_COMMANDER_commander_id],
        mySqlCommanderTypeFields[MYSQL_COMMANDER_commander_id],
        mySqlCommanderStrFields[MYSQL_COMMANDER_is_deleted]
    );

    if (mySqlQuery(self, query,
        commander->accountId,
        commander->commanderName,
        commander->level,
        commander->currentXP,
        commander->gender,
        commander->jobId,
        commander->classId,
        commander->hairId,
        commander->mapId,
        commander->pos.x,
        commander->pos.y,
        commander->pos.z,
        commander->barrackPos.x,
        commander->barrackPos.y,
        commander->barrackPos.z,
        commander->currentHP,
        commander->currentSP,
        itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_HAT]),
        itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_HAT_L]),
        itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_UNKNOWN1]),
        itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_BODY_ARMOR]),
        itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_GLOVES]),
        itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_BOOTS]),
        itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_HELMET]),
        itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_BRACELET]),
        itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_WEAPON]),
        itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_SHIELD]),
        itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_COSTUME]),
        itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_UNKNOWN3]),
        itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_UNKNOWN4]),
        itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_UNKNOWN5]),
        itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_LEG_ARMOR]),
        itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_UNKNOWN6]),
        itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_UNKNOWN7]),
        itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_RING_LEFT]),
        itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_RING_RIGHT]),
        itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_NECKLACE]),
        commanderId))
    {
        error("SQL Error : %s" , mysql_error(self->handle));
        goto cleanup;
    }

    status = true;

cleanup:
    return status;
}

bool mySqlCommanderInsert(MySQL *self, Commander *commander) {

    bool status = false;

    // Insert a new commander
	char query[MAX_QUERY_SIZE] = "INSERT INTO commanders SET ";

	for (MySqlCommanderEnumField field = 0; field < MYSQL_COMMANDER_COUNT; field++) {

        // Exceptions :
        if (field == MYSQL_COMMANDER_commander_id
         || field == MYSQL_COMMANDER_is_deleted
         || field == MYSQL_COMMANDER_time_deleted) {
            continue;
        }

        snprintf(query, sizeof(query), "%s%s = %s, ", query,
        mySqlCommanderStrFields[field], mySqlCommanderTypeFields[field]);

        if (field == MYSQL_COMMANDER_COUNT - 1) {
            // No comma for the last field
            query[strlen(query) - strlen(", ")] = ' ';
        }
	}

    // Insert a new commander
    if (mySqlQuery(self, query,
        commander->accountId,
        commander->commanderName,
        commander->level,
        commander->currentXP,
        commander->gender,
        commander->jobId,
        commander->classId,
        commander->hairId,
        commander->mapId,
        commander->pos.x,
        commander->pos.y,
        commander->pos.z,
        commander->barrackPos.x,
        commander->barrackPos.y,
        commander->barrackPos.z,
        commander->currentHP,
        commander->currentSP,
        itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_HAT]),
        itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_HAT_L]),
        itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_UNKNOWN1]),
        itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_BODY_ARMOR]),
        itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_GLOVES]),
        itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_BOOTS]),
        itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_HELMET]),
        itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_BRACELET]),
        itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_WEAPON]),
        itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_SHIELD]),
        itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_COSTUME]),
        itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_UNKNOWN3]),
        itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_UNKNOWN4]),
        itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_UNKNOWN5]),
        itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_LEG_ARMOR]),
        itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_UNKNOWN6]),
        itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_UNKNOWN7]),
        itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_RING_LEFT]),
        itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_RING_RIGHT]),
        itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_NECKLACE])))
    {
        error("SQL Error : %s" , mysql_error(self->handle));
        goto cleanup;
    }

    commander->commanderId = mysql_insert_id(self->handle);
    // TODO : check last insert id

    status = true;

cleanup:
    return status;
}

bool MySqlCommanderDelete(MySQL *self, CommanderId_t commanderId) {

    bool status = false;
	char query[MAX_QUERY_SIZE] = "DELETE FROM commanders";

    snprintf(query, sizeof(query), "%s WHERE %s = %s", query,
        mySqlCommanderStrFields[MYSQL_COMMANDER_commander_id],
        mySqlCommanderTypeFields[MYSQL_COMMANDER_commander_id]);

    if (mySqlQuery(self, query, commanderId)) {
        error("SQL Error : %s" , mysql_error(self->handle));
        goto cleanup;
    }

    status = true;

cleanup:
    return status;
}
