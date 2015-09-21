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

#include "mysql_account_session.h"
#include "barrack_server/barrack_handler/barrack_builder.h"

bool mySqlGetAccountData(MySQL *self, char *accountName, unsigned char *password, AccountSession * accountSession) {

    bool status = false;
    MYSQL_ROW row;

    char md5Password[33];
    md5BytesToStr(password, md5Password);

    // flush the commander
    if (mySqlQuery(self,
        "SELECT "
            "account_id, "
            "is_banned, "
            "Unix_Timestamp(time_banned), "
            "credits, "
            "privilege_level, "
            "family_name, "
            "barrack_type "
        "FROM accounts "
        "WHERE account_name = '%s' "
        "AND passwd = '%s' "
        "LIMIT 1",
        accountName, md5Password))
    {
        error("SQL Error : %s" , mysql_error(self->handle));
        goto cleanup;
    }

    if (mysql_num_rows(self->result) == 0) {
        dbg("MySQL: Account/Password is incorrect.");
        goto cleanup;
    }

    row = mysql_fetch_row(self->result);

    // update the commander
    strncpy(accountSession->login, accountName, sizeof(accountSession->login));
    accountSession->accountId = strtoll(row[0], NULL, 10);
    accountSession->isBanned = row[1][0] == 'y';
    accountSession->timeBanned = strtol(row[2], NULL, 10); // FIXME : not sure if 32 or 64 bits
    accountSession->credits = strtof(row[3], NULL);
    accountSession->privilege = strtol(row[4], NULL, 10);
    strncpy(accountSession->familyName, row[5], sizeof(accountSession->familyName));
    accountSession->barrackType = strtol(row[6], NULL, 10);

    dbg("MySQL: Account found and loaded.");

    status = true;

cleanup:
    return status;
}

BarrackNameResultType mySqlSetFamilyName(MySQL *self, AccountSession *accountSession, char *familyName) {

    BarrackNameResultType status = BC_BARRACKNAME_CHANGE_ERROR;
    MYSQL_ROW row;

    // Perform query to change name
    if (mySqlQuery(self, "CALL bSetFamilyName(%llx, '%s');", accountSession->accountId, familyName)) {
        error("SQL Error : %s" , mysql_error(self->handle));
        goto cleanup;
    }

    // Check query results
    if (mySqlQuery(self, "SELECT @flag;")) {
        error("SQL Error : %s" , mysql_error(self->handle));
        goto cleanup;
    }

    if (mysql_num_rows(self->result) == 0) {
        error("MySQL: Procedure bSetFamilyName(%llx, '%s') didn't return value",
            accountSession->accountId, familyName);
        goto cleanup;
    }

    row = mysql_fetch_row(self->result);
    status = strtol(row[0], NULL, 10);

cleanup:
    return status;
}
