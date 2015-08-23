/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file Session.h
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
#include "Common/Session/SocketSession.h"
#include "Common/Session/GameSession.h"

// ---------- Defines -------------


// ------ Structure declaration -------
/**
 * @brief Session is a general structure englobing all the session stuff
 */
struct Session
{
    SocketSession socket;
    GameSession game;
};

typedef struct Session Session;

// ----------- Functions ------------

/**
 * @brief Allocate a new Session structure.
 * @return A pointer to an allocated Session, or NULL if an error occured.
 */
Session *
Session_new (
    void
);


/**
 * @brief Initialize an allocated Session structure.
 * @param self An allocated Session to initialize.
 * @return true on success, false otherwise.
 */
bool
Session_init (
    Session *self
);


/**
 * @brief Free an allocated Session structure and nullify the content of the pointer.
 * @param self A pointer to an allocated Session.
 */
void
Session_destroy (
    Session **self
);

