/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file SocketSession.h
 * @brief
 *
 *
 *
 * @license <license placeholder>
 */

#pragma once

// ---------- Includes ------------
#include "R1EMU.h"

// ---------- Defines -------------
#define SOCKET_SESSION_UNDEFINED_MAP -1
#define SOCKET_SESSION_UNDEFINED_ACCOUNT -1

#define SOCKET_SESSION_ID_SIZE 11

// ------ Structure declaration -------

/**
 * @brief SocketSession identifies a session socket to an account
 */
struct SocketSession
{
    /** The account ID of the account associated with the socket */
    uint64_t accountId;
    /** The router ID handling the client socket */
    uint16_t routerId;
    /** The map Id of the commander currently played */
    uint16_t mapId;
    /** Session Redis key */
    uint8_t sessionKey [SOCKET_SESSION_ID_SIZE];

    /** States */
    bool authenticated;
};

typedef struct SocketSession SocketSession;

// ----------- Functions ------------

/**
 * @brief Allocate a new SocketSession structure.
 * @param accountId The accoundId associated with the connection
 * @param serverId The serverId
 * @return A pointer to an allocated SocketSession.
 */
SocketSession *
SocketSession_new (
    uint64_t accountId,
    uint16_t serverId,
    uint16_t mapId,
    uint8_t *socketId,
    bool authenticated
);


/**
 * @brief Initialize an allocated SocketSession structure.
 * @param self An allocated SocketSession to initialize.
 * @return true on success, false otherwise.
 */
bool
SocketSession_init (
    SocketSession *self,
    uint64_t accountId,
    uint16_t serverId,
    uint16_t mapId,
    uint8_t *socketId,
    bool authenticated
);

/**
 * @brief Format a session key from the session id
 * @param sessionId The sessionId of the session requested
 * @param[out] sessionKey The sessionKey generated
 * @return
 */
void
SocketSession_genSessionKey (
    uint8_t *sessionId,
    uint8_t sessionKey[SOCKET_SESSION_ID_SIZE]
);


/**
 * @brief Format a session id from the session key
 * @param sessionKey The sessionKey of the session requested
 * @param[out] sessionId The sessionId generated
 * @return
 */
void
SocketSession_genId (
    uint8_t *sessionKey,
    uint8_t sessionId[5]
);

/**
 * @brief Prints a SocketSession structure.
 * @param self An allocated SocketSession
 */
void
SocketSession_print (
    SocketSession *self
);

/**
 * @brief Free an allocated SocketSession structure and nullify the content of the pointer.
 * @param self A pointer to an allocated SocketSession.
 */
void
SocketSession_destroy (
    SocketSession **self
);

