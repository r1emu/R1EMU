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
#include "common/utils/memory.h"

AccountSession *accountSessionNew(uint8_t *accountName, uint8_t *socketId, AccountSessionPrivileges accountPrivilege) {

    AccountSession *self;

    if ((self = calloc(1, sizeof(AccountSession))) == NULL) {
        return NULL;
    }

    if (!accountSessionInit(self, accountName, socketId, accountPrivilege)) {
        accountSessionDestroy (&self);
        error("AccountSession failed to initialize.");
        return NULL;
    }

    return self;
}

bool accountSessionCommandersInit(AccountSession *self, size_t commandersCountMax, size_t commandersCount) {

    bool status = false;

    if (!(self->commanders = calloc(commandersCountMax, sizeof(Commander *)))) {
        error("Cannot allocate the commanders array.");
        goto cleanup;
    }

    for (int i = 0; i < commandersCount; i++) {
        if (!(self->commanders[i] = commanderNew())) {
            error("Cannot allocate a new commander.");
            goto cleanup;
        }
    }

    status = true;

cleanup:
    if (!status) {
        for (int i = 0; i < commandersCount; i++) {
            commanderDestroy(&self->commanders[i]);
        }
        destroy(&self->commanders);
    }

    return status;
}

bool accountSessionGetCommanderByIndex(AccountSession *self, int commanderIndex, Commander **commander) {

    bool status = false;

    // Check commanderIndex boundaries
    if (commanderIndex < 0 || commanderIndex >= self->commandersCountMax) {
        error("The slot '%d' is out of bound.", commanderIndex);
        goto cleanup;
    }

    if (!self->commanders[commanderIndex]) {
        goto cleanup;
    }

    *commander = self->commanders[commanderIndex];

    status = true;

cleanup:
    return status;

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

bool accountSessionInit(AccountSession *self, uint8_t *accountName, uint8_t *socketId, AccountSessionPrivileges privilege) {

    memset(self, 0, sizeof(AccountSession));

    self->accountId = 0;
    memcpy(self->accountName, accountName, sizeof(self->accountName));
    memcpy(self->sessionKey, socketId, sizeof(self->sessionKey));
    self->privilege = privilege;

    return true;
}

bool accountSessionIsCommanderSlotEmpty(AccountSession *self, int commanderIndex) {

    bool status = false;

    // Check commanderIndex boundaries
    if (commanderIndex < 0 || commanderIndex >= self->commandersCountMax) {
        error("The slot '%d' is out of bound.", commanderIndex);
        goto cleanup;
    }

    // Check if slot is occupied
    if (self->commanders[commanderIndex]) {
        error ("The slot %d is occupied.", commanderIndex);
        goto cleanup;
    }

    status = true;

cleanup:
    return status;

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


size_t accountSessionGetSPacketSize(AccountSession *self) {
    size_t packetSize = 0;

    packetSize += sizeof(AccountSessionSPacket);

    for (size_t i = 0; i < self->commandersCountMax; i++) {
        if (self->commanders[i] != NULL) {
            packetSize += commanderGetSPacketSize(self->commanders[i]);
        }
    }

    return packetSize;
}

void accountSessionSerializeSPacket(AccountSession *self, PacketStream *stream) {

    packetStreamDebugStart(stream, accountSessionGetSPacketSize(self));
    packetStreamIn(stream, self->accountName);
    packetStreamIn(stream, self->familyName);
    packetStreamIn(stream, self->sessionKey);
    packetStreamIn(stream, &self->privilege);
    packetStreamIn(stream, &self->isBanned);
    packetStreamIn(stream, &self->timeBanned);
    packetStreamIn(stream, &self->credits);
    packetStreamIn(stream, &self->timeLastLogin);
    packetStreamIn(stream, &self->barrackType);
    packetStreamIn(stream, &self->commandersCountMax);

    size_t commandersCount = accountSessionGetCommandersCount(self);
    packetStreamIn(stream, &commandersCount);

    for (size_t i = 0; i < self->commandersCountMax; i++) {
        if (self->commanders[i] != NULL) {
            commanderSerializeSPacket(self->commanders[i], stream);
        }
    }
    packetStreamDebugEnd(stream);
}

bool accountSessionUnserialize(AccountSession *self, PacketStream *stream) {

    packetStreamDebugStart(stream, accountSessionGetSPacketSize(self));
    packetStreamOut(stream, self->accountName);
    packetStreamOut(stream, self->familyName);
    packetStreamOut(stream, self->sessionKey);
    packetStreamOut(stream, &self->privilege);
    packetStreamOut(stream, &self->isBanned);
    packetStreamOut(stream, &self->timeBanned);
    packetStreamOut(stream, &self->credits);
    packetStreamOut(stream, &self->timeLastLogin);
    packetStreamOut(stream, &self->barrackType);
    packetStreamOut(stream, &self->commandersCountMax);

    size_t commandersCount;
    packetStreamOut(stream, &commandersCount);

    if (!(self->commanders = malloc(sizeof(Commander *) * commandersCount))) {
        error("Cannot allocate commanders array.");
        return false;
    }

    for (size_t i = 0; i < commandersCount; i++) {
        if (!(commanderUnserializeSPacket(self->commanders[i], stream))) {
            error("Cannot unserialize the commander %d.", i);
            return false;
        }
    }

    packetStreamDebugEnd(stream);
    return true;
}
