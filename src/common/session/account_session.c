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

AccountSession *accountSessionNew(uint8_t *accountLogin, uint8_t *socketId, AccountSessionPrivileges accountPrivilege) {

    AccountSession *self;

    if ((self = calloc(1, sizeof(AccountSession))) == NULL) {
        return NULL;
    }

    if (!accountSessionInit(self, accountLogin, socketId, accountPrivilege)) {
        accountSessionDestroy (&self);
        error("AccountSession failed to initialize.");
        return NULL;
    }

    return self;
}

bool accountSessionCommandersInit(AccountSession *self) {

    bool status = false;

    if (!(self->commanders = calloc(self->commandersCountMax, sizeof(Commander *)))) {
        error("Cannot allocate the commanders array.");
        goto cleanup;
    }

    status = true;

cleanup:
    return status;
}

Commander *accountSessionGetCommanderByIndex(AccountSession *self, int commanderIndex) {

    // Check commanderIndex boundaries
    if (commanderIndex < 0 || commanderIndex >= self->commandersCountMax) {
        error("The slot '%d' is out of bound.");
        return NULL;
    }

    return self->commanders[commanderIndex];
}

size_t accountSessionGetCommandersCount(AccountSession *self) {
    size_t count = 0;

    for (size_t i = 0; i < self->commandersCountMax; i++) {
        if (self->commanders[i] != NULL) {
            count++;
        }
    }

    return count;
}

bool accountSessionInit(AccountSession *self, uint8_t *login, uint8_t *socketId, AccountSessionPrivileges privilege) {

    memset(self, 0, sizeof(AccountSession));

    self->accountId = 0;
    memcpy(self->login, login, sizeof(self->login));
    memcpy(self->sessionKey, socketId, sizeof(self->sessionKey));
    self->privilege = privilege;

    return true;
}

void accountSessionPrint(AccountSession *self) {
    dbg("==== AccountSession %p ====", self);
}

void accountSessionDestroy(AccountSession **_self) {
    accountSessionFree (*_self);
    *_self = NULL;
}

void accountSessionFree (AccountSession *self) {
    free(self);
}
