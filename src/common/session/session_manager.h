/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file session_manager.h
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
#include "common/Session/Session.h"

// ---------- Defines -------------
#define SESSION_MANAGER_ENDPOINT "inproc://sessionManagerRep-%d"

// ------ Structure declaration -------
typedef struct SessionManagerStartupInfo {
    uint16_t routerId;
}   SessionManagerStartupInfo;

typedef struct SessionManager SessionManager;

typedef enum {
    SESSION_MANAGER_STORE_SESSION,
    SESSION_MANAGER_LOAD_SESSION,
} SessionManagerPacketType;

typedef enum {
    SESSION_MANAGER_STATUS_SUCCESS,
    SESSION_MANAGER_CANNOT_LOAD,
    SESSION_MANAGER_CANNOT_STORE,
} SessionManagerStatus;

// ----------- Functions ------------

/**
 * Allocate a new SessionManager structure.
 * @param startInfo The info for starting the SessionManager
 * @return A pointer to an allocated SessionManager, or NULL if an error occured.
 */
SessionManager *sessionManagerNew(SessionManagerStartupInfo *startInfo);

/**
 * Initialize an allocated SessionManager structure.
 * @param self An allocated SessionManager to initialize.
 * @return true on success, false otherwise.
 */
bool sessionManagerInit(SessionManager *self, SessionManagerStartupInfo *startInfo);

/**
 * Start the SessionManager.
 * @param self An allocated SessionManager to initialize.
 * @return true on success, false otherwise.
 */
bool sessionManagerStart(SessionManager *self);

/**
 * Store a session in the session manager
 * @param self An allocated SessionManager
 * @param sessionKey a unique session key
 * @param session an allocated Session to store
 * @param true on success, false otherwise
 */
bool sessionManagerStoreSession(SessionManager *self, uint8_t *sessionKey, Session *session);

/**
 * Retrieve a session in the session manager
 * @param self An allocated SessionManager
 * @param sessionKey a unique session key
 * @param[out] session an allocated Session to load
 * @param true on success, false otherwise
 */
bool sessionManagerLoadSession(SessionManager *self, uint8_t *sessionKey, Session *session);

/**
 * Free an allocated SessionManager structure.
 * @param self A pointer to an allocated SessionManager.
 */
void sessionManagerFree(SessionManager *self);

/**
 * Free an allocated SessionManager structure and nullify the content of the pointer.
 * @param self A pointer to an allocated SessionManager.
 */
void sessionManagerDestroy(SessionManager **self);


