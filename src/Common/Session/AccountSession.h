/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file AccountSession.h
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

// ---------- Defines -------------
#define ACCOUNT_SESSION_LOGIN_MAXSIZE 33

// ------ Structure declaration -------
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
struct AccountSession
{
    /** The account login */
    uint8_t login [ACCOUNT_SESSION_LOGIN_MAXSIZE];

    /** Session key */
    uint8_t sessionKey [SOCKET_SESSION_ID_SIZE];

    /** Account privilege level */
    AccountSessionPrivileges privilege;
};

typedef struct AccountSession AccountSession;

// ----------- Functions ------------

/**
 * @brief Allocate a new AccountSession structure.
 * @return A pointer to an allocated AccountSession.
 */
AccountSession *
AccountSession_new (
    uint8_t *accountLogin,
    uint8_t *socketId,
    AccountSessionPrivileges accountPrivilege
);


/**
 * @brief Initialize an allocated AccountSession structure.
 * @param self An allocated AccountSession to initialize.
 * @return true on success, false otherwise.
 */
bool
AccountSession_init (
    AccountSession *self,
    uint8_t *accountLogin,
    uint8_t *socketId,
    AccountSessionPrivileges accountPrivilege
);

/**
 * @brief Prints a AccountSession structure.
 * @param self An allocated AccountSession
 */
void
AccountSession_print (
    AccountSession *self
);

/**
 * @brief Free an allocated AccountSession structure and nullify the content of the pointer.
 * @param self A pointer to an allocated AccountSession.
 */
void
AccountSession_destroy (
    AccountSession **self
);


/**
 * @brief Free an allocated AccountSession structure
 * @param self A pointer to an allocated AccountSession.
 */
void
AccountSession_free (
    AccountSession *self
);
