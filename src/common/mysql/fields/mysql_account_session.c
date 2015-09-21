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
    MYSQL_ROW row;

    char md5Password[33];
    for (int i = 0; i < 16; i++) {
        sprintf (&md5Password[i*2], "%.02x", password[i]);
    }

    // flush the commander
    if (mySqlQuery(self, "SELECT account_id, is_banned, Unix_Timestamp(time_banned), credits, privilege_level, family_name, barrack_type FROM accounts WHERE account_name = '%s' AND passwd = '%s' LIMIT 1",
    //if (mySqlQuery(self, "SELECT account_id, is_banned, Unix_Timestamp(time_banned), credits, privilege_level, family_name, barrack_type FROM accounts WHERE account_name = '%s' LIMIT 1",
                   accountName, md5Password)) {
        error("SQL Error : %s" , mysql_error(self->handle));
    }
    else {

        if (mysql_num_rows(self->result) == 0) {
            dbg("MySQL: Account/Password is incorrect.");
        }
        else {
            row = mysql_fetch_row(self->result);
            // update the commander
            strncpy(accountSession->login, accountName, sizeof(accountSession->login));
            accountSession->accountId = strtoll(row[0], NULL, 10);
            accountSession->isBanned = strcmp(row[1], "y") == 0 ? true : false;
            accountSession->timeBanned = strtol(row[2], NULL, 10); // not sure if 32 or 64 bits
            accountSession->credits = strtof(row[3], NULL);
            accountSession->privilege = strtol(row[4], NULL, 10);
            strncpy(accountSession->familyName, row[5], sizeof(accountSession->familyName));
            accountSession->barrackType = strtol(row[6], NULL, 10);

            dbg("MySQL: Account found and loaded.");
            return true;
        }
    }
    return false;
}

BarrackNameResultType mySqlSetFamilyName(MySQL *self, AccountSession *accountSession, char *familyName) {
    MYSQL_ROW row;

    dbg("accountId: %llx", accountSession->accountId);
    dbg("newName: %s", familyName);

    // Perform query to change name
    if (mySqlQuery(self, "CALL bSetFamilyName(%llx, '%s');", accountSession->accountId, familyName)) {
        error("SQL Error : %s" , mysql_error(self->handle));
        return BC_BARRACKNAME_CHANGE_ERROR;
    }
    // Check query results
    if (mySqlQuery(self, "SELECT @flag;")) {
        error("SQL Error : %s" , mysql_error(self->handle));
        return BC_BARRACKNAME_CHANGE_ERROR;
    } else {
        if (mysql_num_rows(self->result) == 0) {
            dbg("MySQL: Procedure bSetFamilyName(accountId, newName) didnt return value", accountSession->accountId, familyName);
            return BC_BARRACKNAME_CHANGE_ERROR;
        }
        else {

            row = mysql_fetch_row(self->result);
            //
            BarrackNameResultType operationResult = strtol(row[0], NULL, 10);

            dbg("MySQL: SetFamilyName result: %x", operationResult);
            return operationResult;
        }
    }
    return false;
}
