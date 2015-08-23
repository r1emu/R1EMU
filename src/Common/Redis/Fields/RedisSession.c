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
#include "RedisSession.h"
#include "RedisSocketSession.h"
#include "RedisGameSession.h"


// ------ Structure declaration -------


// ------ Static declaration -------


// ------ Extern function implementation -------

bool
Redis_getSession (
    Redis *self,
    RedisSessionKey *key,
    Session *session
) {
    GameSession *gameSession = &session->game;
    SocketSession *socketSession = &session->socket;
    RedisSocketSessionKey *socketKey = &key->socketKey;

    // Search for the Socket Session
    if (!Redis_getSocketSession (self, socketKey, socketSession)) {
        error ("Cannot get Socket Session.");
        return false;
    }

    if (!socketSession->authenticated) {
        // This is the first time the client connect.
        // Initialize an empty game session
        CommanderInfo commanderInfo;
        CommanderInfo_init (&commanderInfo);
        GameSession_init (gameSession, &commanderInfo);
        dbg ("Welcome, SOCKET_%s ! A new session has been initialized for you.", socketKey->sessionKey);
    } else {
        // The client already exist in the game, get Game Session
        RedisGameSessionKey gameKey = {
            .routerId  = socketSession->routerId,
            .mapId     = socketSession->mapId,
            .accountId = socketSession->accountId
        };
        if (!Redis_getGameSession (self, &gameKey, gameSession)) {
            error ("Cannot get Game Session.");
            return false;
        }
        // dbg ("Welcome back, SOCKET_%s !", sessionKey);
    }

    return true;
}


bool
Redis_updateSession (
    Redis *self,
    Session *session
) {
    RedisSocketSessionKey socketKey = {
        .routerId = session->socket.routerId,
        .sessionKey = session->socket.sessionKey
    };
    if (!Redis_updateSocketSession (self, &socketKey, &session->socket)) {
        error ("Cannot update the socket session.");
        return false;
    }
    RedisGameSessionKey gameKey = {
        .routerId  = session->socket.routerId,
        .mapId     = session->socket.mapId,
        .accountId = session->socket.accountId
    };

    if (!(Redis_updateGameSession (self, &gameKey, session->socket.sessionKey, &session->game))) {
        error ("Cannot update the game session");
        return false;
    }

    return true;
}


bool
Redis_flushSession (
    Redis *self,
    RedisSessionKey *key
) {
    RedisSocketSessionKey *socketKey = &key->socketKey;
    // Retrieve the entire SocketSession
    SocketSession socketSession;

    if (!(Redis_getSocketSession (self, socketKey, &socketSession))) {
        error ("Cannot get the SocketSession for %s.", socketKey->sessionKey);
        return false;
    }

    // Flush the game session
    RedisGameSessionKey gameKey = {
        .routerId = socketSession.routerId,
        .mapId = socketSession.mapId,
        .accountId = socketSession.accountId
    };
    if (!(Redis_flushGameSession (self, &gameKey))) {
        error ("Cannot flush the Game Session associated with the Socket Session.");
        return false;
    }

    // Flush the socket session
    if (!(Redis_flushSocketSession (self, socketKey))) {
        error ("Cannot flush the Game Session associated with the Socket Session.");
        return false;
    }

    return true;
}
