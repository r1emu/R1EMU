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

/**
 * @brief Get Account Data from MySQL
 */
bool mySqlGetAccountData(MySQL *self, char *accountName, unsigned char *password, AccountSession * accountSession);

/**
 * @brief Set Family name into MySQL for a given AccountSession
 */
BarrackNameResultType mySqlSetFamilyName(MySQL *self, AccountSession *accountSession, char *familyName);

