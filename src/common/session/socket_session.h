/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file socket_session.h
 * @brief
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#pragma once

#include "R1EMU.h"

#define SOCKET_SESSION_UNDEFINED_MAP -1
#define SOCKET_SESSION_UNDEFINED_ACCOUNT -1

#define SOCKET_SESSION_ID_SIZE 11

/**
 * @brief SocketSession identifies a session socket to an account
 */
struct SocketSession {
    // the account ID of the account associated with the socket
    uint64_t accountId;

    // the router ID handling the client socket
    RouterId_t routerId;

    // the map Id of the commander currently played
    MapId_t mapId;

    // session Redis key
    uint8_t sessionKey[SOCKET_SESSION_ID_SIZE];

    // states
    bool authenticated;
};

typedef struct SocketSession SocketSession;

/**
 * @brief Allocate a new SocketSession structure.
 * @param accountId The accoundId associated with the connection
 * @param serverId The serverId
 * @return A pointer to an allocated SocketSession.
 */
SocketSession *socketSessionNew(
    uint64_t accountId,
    uint16_t serverId,
    MapId_t mapId,
    uint8_t *socketId,
    bool authenticated);

/**
 * @brief Initialize an allocated SocketSession structure.
 * @param self An allocated SocketSession to initialize.
 * @return true on success, false otherwise.
 */
bool socketSessionInit(
    SocketSession *self,
    uint64_t accountId,
    uint16_t serverId,
    MapId_t mapId,
    uint8_t *socketId,
    bool authenticated);

/**
 * @brief Format a session key from the session id
 * @param sessionId The sessionId of the session requested
 * @param[out] sessionKey The sessionKey generated
 * @return
 */
void socketSessionGenSessionKey(uint8_t *sessionId, uint8_t sessionKey[SOCKET_SESSION_ID_SIZE]);

/**
 * @brief Format a session id from the session key
 * @param sessionKey The sessionKey of the session requested
 * @param[out] sessionId The sessionId generated
 * @return
 */
void socketSessionGenId(uint8_t *sessionKey, uint8_t sessionId[5]);

/**
 * @brief Prints a SocketSession structure.
 * @param self An allocated SocketSession
 */
void socketSessionPrint(SocketSession *self);

/**
 * @brief Free an allocated SocketSession structure and nullify the content of the pointer.
 * @param self A pointer to an allocated SocketSession.
 */
void socketSessionDestroy(SocketSession **self);
