/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file mysql_account_session.h
 * @brief MySQLSession contains functions for storing and loading an Account Session in a MySQL database
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#pragma once

#include "R1EMU.h"
#include "common/mysql/mysql.h"
#include "common/session/session.h"
#include "barrack_server/barrack_handler/barrack_builder.h"

#define MYSQL_ACCOUNT_SESSION_FIELD_account_id_str      "account_id"
#define MYSQL_ACCOUNT_SESSION_FIELD_is_banned_str       "is_banned"
#define MYSQL_ACCOUNT_SESSION_FIELD_time_banned_str     "time_banned"
#define MYSQL_ACCOUNT_SESSION_FIELD_credits_str         "credits"
#define MYSQL_ACCOUNT_SESSION_FIELD_privilege_level_str "privilege_level"
#define MYSQL_ACCOUNT_SESSION_FIELD_family_name_str     "family_name"
#define MYSQL_ACCOUNT_SESSION_FIELD_barrack_type_str    "barrack_type"
#define MYSQL_ACCOUNT_SESSION_FIELD_account_name_str    "account_name"
#define MYSQL_ACCOUNT_SESSION_FIELD_passwd_str          "passwd"

/**
 * @brief Get Account Data from MySQL
 */
bool mySqlGetAccountData(
    MySQL *self,
    char *accountName,
    unsigned char *password,
    AccountSession *accountSession,
    bool *goodCredentials);

/**
 * @brief Set Family name into MySQL for a given AccountSession
 */
BarrackNameResultType mySqlSetFamilyName(MySQL *self, AccountSession *accountSession, char *familyName);

