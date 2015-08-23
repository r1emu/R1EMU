/**
*
*   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
*   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
*   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
*   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
*   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
*   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
*
* @file MySQLSession.h
* @brief MySQLSession contains functions for storing and loading a Session in a MySQL database
*
* @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
*/

#pragma once

// ---------- Includes ------------
#include "R1EMU.h"
#include "Common/MySQL/MySQL.h"
#include "Common/Session/Session.h"

// ---------- Defines -------------

// ------ Structure declaration -------


// ----------- Functions ------------

/**
 * @brief Flush a Session in the MySQL database
 */
void
MySQL_flushSession (
    MySQL *self,
    Session *session
);
