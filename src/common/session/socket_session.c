/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */


// ---------- Includes ------------
#include "socket_session.h"


// ------ Structure declaration -------


// ------ Static declaration -------


// ------ Extern function implementation -------

SocketSession *
socketSessionNew (
    uint64_t accountId,
    uint16_t routerId,
    uint16_t mapId,
    uint8_t *socketId,
    bool authenticated
) {
    SocketSession *self;

    if ((self = calloc(1, sizeof(SocketSession))) == NULL) {
        return NULL;
    }

    if (!socketSessionInit(self, accountId, routerId, mapId, socketId, authenticated)) {
        socketSessionDestroy (&self);
        error("SocketSession failed to initialize.");
        return NULL;
    }

    return self;
}

bool
socketSessionInit(
    SocketSession *self,
    uint64_t accountId,
    uint16_t routerId,
    uint16_t mapId,
    uint8_t *sessionKey,
    bool authenticated
) {
    self->accountId = accountId;
    self->routerId = routerId;
    self->mapId = mapId;
    memcpy(self->sessionKey, sessionKey, sizeof(self->sessionKey));

    self->authenticated = authenticated;

    return true;
}

void
socketSessionGenSessionKey (
    uint8_t *sessionId,
    uint8_t sessionKey[SOCKET_SESSION_ID_SIZE]
) {
    // Format the session key from the sessionId
    snprintf (sessionKey, SOCKET_SESSION_ID_SIZE,
        "%02X%02X%02X%02X%02X", sessionId[0], sessionId[1], sessionId[2], sessionId[3], sessionId[4]);
}

void
socketSessionGenId (
    uint8_t *sessionKey,
    uint8_t sessionId[5]
) {
    uint64_t identity = strtoull(sessionKey, NULL, 16);
    // Format the sessionId from the session key
    // Swap the bytes
    sessionId[0] = (identity >> 32) & 0xFF;
    sessionId[1] = (identity >> 24) & 0xFF;
    sessionId[2] = (identity >> 16) & 0xFF;
    sessionId[3] = (identity >>  8) & 0xFF;
    sessionId[4] =  identity        & 0xFF;
}

void
socketSessionPrint (
    SocketSession *self
) {
    dbg("==== SocketSession %p ====", self);
    dbg("sessionKey = <%s>", self->sessionKey);
    dbg("accountId = <%llx>", self->accountId);
    dbg("routerId = <%x>", self->routerId);
    dbg("authenticated = <%x>", self->authenticated);
}


void
socketSessionDestroy (
    SocketSession **_self
) {
    SocketSession *self = *_self;

    free(self);
    *_self = NULL;
}


size_t socketSessionGetPacketSize(SocketSession *self) {
    return sizeof(*self);
}

void socketSessionSPacket(SocketSession *self, PacketStream *stream) {
    packetStreamIn(stream, &self->accountId);
    packetStreamIn(stream, &self->routerId);
    packetStreamIn(stream, &self->mapId);
    packetStreamIn(stream, self->sessionKey);
    packetStreamIn(stream, &self->authenticated);
}

void socketSessionUnpacket(SocketSession *self, PacketStream *stream) {
    packetStreamOut(stream, &self->accountId);
    packetStreamOut(stream, &self->routerId);
    packetStreamOut(stream, &self->mapId);
    packetStreamOut(stream, self->sessionKey);
    packetStreamOut(stream, &self->authenticated);
}
