/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file session.h
 * @brief
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#pragma once

#include "R1EMU.h"
#include "common/session/socket_session.h"
#include "common/session/game_session.h"

/**
 * @brief Session is a general structure englobing all the session stuff
 */
struct Session {
    SocketSession socket;
    GameSession game;
};

typedef struct Session Session;

/**
 * @brief Allocate a new Session structure.
 * @return A pointer to an allocated Session, or NULL if an error occurred.
 */
Session *sessionNew (uint16_t routerId, uint8_t *sessionKey);

/**
 * @brief Initialize an allocated Session structure.
 * @param self An allocated Session to initialize.
 * @return true on success, false otherwise.
 */
bool sessionInit(Session *self, uint16_t routerId, uint8_t *sessionKey);

/**
 * @brief Free an allocated Session structure and nullify the content of the pointer.
 * @param self A pointer to an allocated Session.
 */
void sessionDestroy(Session **self);
