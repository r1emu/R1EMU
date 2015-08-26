/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file account_session.h
 * @brief
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#pragma once

#include "R1EMU.h"
#include "common/session/socket_session.h"

#define ACCOUNT_SESSION_LOGIN_MAXSIZE 33

/**
 * @brief AccountSessionPrivileges enumerates the different levels of privileges
 *  for an account.
 */
typedef enum AccountSessionPrivileges {
    ACCOUNT_SESSION_PRIVILEGES_ADMIN   = 0,
    ACCOUNT_SESSION_PRIVILEGES_UNKNOWN = 1,
    ACCOUNT_SESSION_PRIVILEGES_GM      = 2,
    ACCOUNT_SESSION_PRIVILEGES_PLAYER  = 3
}   AccountSessionPrivileges;

/**
 * @brief
 */
struct AccountSession {
    // The account login
    uint8_t login[ACCOUNT_SESSION_LOGIN_MAXSIZE];

    // Session key
    uint8_t sessionKey[SOCKET_SESSION_ID_SIZE];

    // Account privilege level
    AccountSessionPrivileges privilege;
};

typedef struct AccountSession AccountSession;

/**
 * @brief Allocate a new AccountSession structure.
 * @return A pointer to an allocated AccountSession.
 */
AccountSession *accountSessionNew(
    uint8_t *accountLogin,
    uint8_t *socketId,
    AccountSessionPrivileges accountPrivilege);

/**
 * @brief Initialize an allocated AccountSession structure.
 * @param self An allocated AccountSession to initialize.
 * @return true on success, false otherwise.
 */
bool accountSessionInit(
    AccountSession *self,
    uint8_t *accountLogin,
    uint8_t *socketId,
    AccountSessionPrivileges accountPrivilege);

/**
 * @brief Prints a AccountSession structure.
 * @param self An allocated AccountSession
 */
void accountSessionPrint(AccountSession *self);

/**
 * @brief Free an allocated AccountSession structure and nullify the content of the pointer.
 * @param self A pointer to an allocated AccountSession.
 */
void accountSessionDestroy(AccountSession **self);

/**
 * @brief Free an allocated AccountSession structure
 * @param self A pointer to an allocated AccountSession.
 */
void accountSessionFree(AccountSession *self);
