/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file game_session.h
 * @brief
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#pragma once

#include "R1EMU.h"
#include "common/commander/commander.h"
#include "common/session/socket_session.h"
#include "common/session/barrack_session.h"
#include "common/session/commander_session.h"
#include "common/session/account_session.h"

// max size of the Account Login
#define GAME_SESSION_KEY_MAXSIZE 64

/**
 * @brief GameSession is a session created when a client authenticates
 *
 * GameSession is created during the Barrack phase.
 * This structure size must be kept as small as possible.
 * /!\ It shouldn't contain any pointer, because it is sent to others worker via TCP
 */
struct GameSession {
    // Account session variables
    AccountSession accountSession;

    // Barrack session variables
    BarrackSession barrackSession;

    // Commander session variables
    CommanderSession commanderSession;
};

typedef struct GameSession GameSession;

/**
 * @brief Allocate a new GameSession structure.
 * @param commander An initialized commander information
 * @return A pointer to an allocated GameSession.
 */
GameSession *gameSessionNew(Commander *commander);

/**
 * @brief Initialize an allocated GameSession structure.
 * @param self An allocated GameSession to initialize.
 * @param commander An initialized commander information
 * @return true on success, false otherwise.
 */
bool gameSessionInit(GameSession *self, Commander *commander);

/**
 * @brief Prints a GameSession structure.
 * @param self An allocated GameSession
 */
void gameSessionPrint(GameSession *self);

/**
 * @brief Free an allocated GameSession structure and nullify the content of the pointer.
 * @param self A pointer to an allocated GameSession.
 */
void gameSessionDestroy(GameSession **self);

/**
 * @brief Free an allocated GameSession structure
 * @param self A pointer to an allocated GameSession.
 */
void gameSessionFree(GameSession *self);
