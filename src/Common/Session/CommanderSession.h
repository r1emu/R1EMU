/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file CommanderSession.h
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
#include "Common/Commander/Commander.h"

// ---------- Defines -------------

// ------ Structure declaration -------
/**
 * @brief
 */
struct CommanderSession
{
    /** Current mapId **/
    uint16_t mapId;

    /** Current commander */
    CommanderInfo currentCommander;
};

typedef struct CommanderSession CommanderSession;

// ----------- Functions ------------

/**
 * @brief Allocate a new CommanderSession structure.
 * @return A pointer to an allocated CommanderSession.
 */
CommanderSession *
CommanderSession_new (
    CommanderInfo *commanderInfo
);


/**
 * @brief Initialize an allocated CommanderSession structure.
 * @param self An allocated CommanderSession to initialize.
 * @param commanderInfo An allocated CommanderInfo
 * @return true on success, false otherwise.
 */
bool
CommanderSession_init (
    CommanderSession *self,
    CommanderInfo *commanderInfo
);

/**
 * @brief Prints a CommanderSession structure.
 * @param self An allocated CommanderSession
 */
void
CommanderSession_print (
    CommanderSession *self
);

/**
 * @brief Free an allocated CommanderSession structure and nullify the content of the pointer.
 * @param self A pointer to an allocated CommanderSession.
 */
void
CommanderSession_destroy (
    CommanderSession **self
);


/**
 * @brief Free an allocated CommanderSession structure
 * @param self A pointer to an allocated CommanderSession.
 */
void
CommanderSession_free (
    CommanderSession *self
);
