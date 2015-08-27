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

// ---------- Includes ------------
#include "account_session.h"

AccountSession *
accountSessionNew (
    uint8_t *accountLogin,
    uint8_t *socketId,
    AccountSessionPrivileges accountPrivilege
) {
    AccountSession *self;

    if ((self = calloc (1, sizeof (AccountSession))) == NULL) {
        return NULL;
    }

    if (!accountSessionInit (self, accountLogin, socketId, accountPrivilege)) {
        accountSessionDestroy (&self);
        error ("AccountSession failed to initialize.");
        return NULL;
    }

    return self;
}

bool
accountSessionInit (
    AccountSession *self,
    uint8_t *login,
    uint8_t *socketId,
    AccountSessionPrivileges privilege
) {
    memset (self, 0, sizeof (AccountSession));

    memcpy (self->login, login, sizeof (self->login));
    memcpy (self->sessionKey, socketId, sizeof (self->sessionKey));
    self->privilege = privilege;

    return true;
}

void
accountSessionPrint (
    AccountSession *self
) {
    dbg ("==== AccountSession %p ====", self);
}

void
accountSessionDestroy (
    AccountSession **_self
) {
    accountSessionFree (*_self);
    *_self = NULL;
}

void
accountSessionFree (
    AccountSession *self
) {
    free (self);
}
