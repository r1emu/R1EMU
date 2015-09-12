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
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#pragma once

#include "R1EMU.h"
#include "common/session/session.h"
#include "common/server/worker.h"

/**
 * @brief : Decide which admin commands is going to be executed based on the command line
 */
void adminCmdProcess(Worker *self, char *command, Session *session, zmsg_t *replyMsg);

/**
 * @brief : Spawn a commander at the place of the current commander.
 */
void adminCmdSpawnPc(Worker *self, Session *session, char *args, zmsg_t *replyMsg);

/**
 * @brief : Makes the current commander teleport to a random coordinate or to x, y, z.
 */
void adminCmdJump(Worker *self, Session *session, char *args, zmsg_t *replyMsg);

/**
 * @brief : Add any item in the commander inventory
 *          /addItem <itemId> <quantity>
 */
void adminCmdAddItem(Worker *self, Session *session, char *args, zmsg_t *replyMsg);

/**
 * @brief : A test command
 *          /test
 */
void adminCmdTest(Worker *self, Session *session, char *args, zmsg_t *replyMsg);

/**
 * @brief : Initialize admin commands
 */
bool adminCmdInit(void);

/**
 * @brief : Returns the player's position
 */
void adminCmdWhere(Worker *self, Session *session, char *args, zmsg_t *replyMsg);

/**
 * @brief : Change the camera position
 */
void adminCmdChangeCamera(Worker *self, Session *session, char *args, zmsg_t *replyMsg);

/**
 * @brief : Set player stamina
 */
 void adminCmdSetStamina(Worker *self, Session *session, char *args, zmsg_t *replyMsg);

/**
 * @brief : Set player SP
 */
 void adminCmdSetSP(Worker *self, Session *session, char *args, zmsg_t *replyMsg);

 /**
 * @brief : Set player level
 */
 void adminCmdSetLevel(Worker *self, Session *session, char *args, zmsg_t *replyMsg);
