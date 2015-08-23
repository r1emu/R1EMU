/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file GameSession.h
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
#include "Common/Session/SocketSession.h"
#include "Common/Session/BarrackSession.h"
#include "Common/Session/CommanderSession.h"
#include "Common/Session/AccountSession.h"

// ---------- Defines -------------
/** Max size of the Account Login */
#define GAME_SESSION_KEY_MAXSIZE 64

// ------ Structure declaration -------
/**
 * @brief GameSession is a session created when a client authenticates
 *
 * GameSession is created during the Barrack phase.
 * This structure size must be kept as small as possible.
 * /!\ It shouldn't contain any pointer, because it is sent to others worker via TCP
 */
struct GameSession
{
    /** Account session variables */
    AccountSession accountSession;

    /** Barrack session variables */
    BarrackSession barrackSession;

    /** Commander session variables */
    CommanderSession commanderSession;
};

typedef struct GameSession GameSession;

// ----------- Functions ------------

/**
 * @brief Allocate a new GameSession structure.
 * @param commanderInfo An initialized commander information
 * @return A pointer to an allocated GameSession.
 */
GameSession *
GameSession_new (
    CommanderInfo *commanderInfo
);


/**
 * @brief Initialize an allocated GameSession structure.
 * @param self An allocated GameSession to initialize.
 * @param commanderInfo An initialized commander information
 * @return true on success, false otherwise.
 */
bool
GameSession_init (
    GameSession *self,
    CommanderInfo *commanderInfo
);

/**
 * @brief Prints a GameSession structure.
 * @param self An allocated GameSession
 */
void
GameSession_print (
    GameSession *self
);

/**
 * @brief Free an allocated GameSession structure and nullify the content of the pointer.
 * @param self A pointer to an allocated GameSession.
 */
void
GameSession_destroy (
    GameSession **self
);


/**
 * @brief Free an allocated GameSession structure
 * @param self A pointer to an allocated GameSession.
 */
void
GameSession_free (
    GameSession *self
);
