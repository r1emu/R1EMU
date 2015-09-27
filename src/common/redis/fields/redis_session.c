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
#include "redis_session.h"
#include "redis_socket_session.h"
#include "redis_game_session.h"


// ------ Structure declaration -------


// ------ Static declaration -------


// ------ Extern function implementation -------

bool redisUpdateSession(Redis *self, Session *session) {

    bool status = false;

    RedisSocketSessionKey socketSessionKey = {
        .routerId = session->socket.routerId,
        .sessionKey = session->socket.sessionKey
    };
    if (!redisUpdateSocketSession(self, &socketSessionKey, &session->socket)) {
        error("Cannot update the socket session.");
        goto cleanup;
    }

    RedisGameSessionKey gameSessionKey = {
        .routerId  = session->socket.routerId,
        .mapId     = session->socket.mapId,
        .accountId = session->socket.accountId
    };
    if (!(redisUpdateGameSession(self, &gameSessionKey, session->socket.sessionKey, &session->game))) {
        error("Cannot update the game session");
        goto cleanup;
    }

    status = true;

cleanup:
    return status;
}

bool redisGetSession(Redis *self, RedisSessionKey *key, Session *session) {

    bool status = false;

    GameSession *gameSession = &session->game;
    SocketSession *socketSession = &session->socket;

    RedisSocketSessionKey *socketSessionKey = &key->socketSessionKey;
    // Search for the Socket Session
    if (!redisGetSocketSession(self, socketSessionKey, socketSession)) {
        error("Cannot get Socket Session.");
        goto cleanup;
    }

    // The client already exist in the game, get Game Session
    RedisGameSessionKey gameKey = {
        .routerId  = socketSession->routerId,
        .mapId     = socketSession->mapId,
        .accountId = socketSession->accountId
    };
    if (!redisGetGameSession(self, &gameKey, gameSession)) {
        error("Cannot get Game Session.");
        goto cleanup;
    }

    status = true;

cleanup:
    return status;
}

bool redisDeleteSession(Redis *self, RedisSessionKey *key) {

    RedisSocketSessionKey *socketSessionKey = &key->socketSessionKey;
    SocketSession socketSession;

    if (!(redisGetSocketSession(self, socketSessionKey, &socketSession))) {
        error("Cannot get the SocketSession for %s.", socketSessionKey->sessionKey);
        return false;
    }

    // Delete the game session
    RedisGameSessionKey gameKey = {
        .routerId = socketSession.routerId,
        .mapId = socketSession.mapId,
        .accountId = socketSession.accountId
    };
    if (!(redisDeleteGameSession(self, &gameKey))) {
        error("Cannot flush the Game Session associated with the Socket Session.");
        return false;
    }

    // Delete the socket session
    if (!(redisDeleteSocketSession(self, socketSessionKey))) {
        error("Cannot flush the Game Session associated with the Socket Session.");
        return false;
    }

    return true;
}
