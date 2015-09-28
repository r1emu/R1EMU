/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file commander_session.h
 * @brief
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#pragma once

#include "R1EMU.h"
#include "common/commander/commander.h"

/**
 * @brief
 */
struct CommanderSession {
    // current commander
    Commander *currentCommander;
};

typedef struct CommanderSessionPacket {
    int currentCommanderIndex; // -1 if not initialized
}   CommanderSessionPacket;

typedef struct CommanderSession CommanderSession;

/**
 * @brief Allocate a new CommanderSession structure.
 * @return A pointer to an allocated CommanderSession.
 */
CommanderSession *commanderSessionNew(Commander *commander);

/**
 * @brief Initialize an allocated CommanderSession structure.
 * @param self An allocated CommanderSession to initialize.
 * @param commander An allocated Commander
 * @return true on success, false otherwise.
 */
bool commanderSessionInit(CommanderSession *self, Commander *commander);

/**
 * @brief Prints a CommanderSession structure.
 * @param self An allocated CommanderSession
 */
void commanderSessionPrint(CommanderSession *self);

/**
 * @brief Free an allocated CommanderSession structure and nullify the content of the pointer.
 * @param self A pointer to an allocated CommanderSession.
 */
void commanderSessionDestroy(CommanderSession **self);

/**
 * @brief Free an allocated CommanderSession structure
 * @param self A pointer to an allocated CommanderSession.
 */
void commanderSessionFree(CommanderSession *self);


size_t commanderSessionGetPacketSize(CommanderSession *self);
void commanderSessionSerialize(CommanderSession *self, PacketStream *stream);
