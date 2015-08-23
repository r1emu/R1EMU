/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file admin_cmd.h
 * @brief
 *
 *
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#pragma once

// ---------- Includes ------------
#include "R1EMU.h"
#include "Common/Session/Session.h"
#include "Common/Server/Worker.h"

// ---------- Defines -------------

// ------ Structure declaration -------

// ----------- Functions ------------
/**
 * @brief : Decide which admin commands is going to be executed based on the command line
 */
void
AdminCmd_process (
    Worker *self,
    char *command,
    Session *session,
    zmsg_t *replyMsg
);

/**
 * @brief : Spawn a commander at the place of the current commander.
 */
void
AdminCmd_spawnPc (
    Worker *self,
    Session *session,
    zmsg_t *replyMsg
);
