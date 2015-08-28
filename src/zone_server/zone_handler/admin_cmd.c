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

#include "admin_cmd.h"
#include "common/commander/commander.h"
#include "common/redis/fields/redis_game_session.h"
#include "common/redis/fields/redis_socket_session.h"
#include "zone_server/zone_handler/zone_builder.h"
#include "common/session/session.h"
#include "common/server/worker.h"

void adminCmdProcess(Worker *self, char *command, Session *session,zmsg_t *replyMsg) {
    if (strncmp(command, "spawn", strlen("spawn")) == 0) {
        adminCmdSpawnPc(self, session, replyMsg);
    }
    else if (strncmp(command, "jump", strlen("jump")) == 0) {
        if (strlen(command) > strlen("jump") + 1) {
            adminCmdJump(session, replyMsg, command + strlen("jump") + 1);
        }
        else {
            adminCmdJump(session, replyMsg, NULL);
        }
    }
}

void adminCmdSpawnPc(Worker *self, Session *session, zmsg_t *replyMsg) {
    // add a fake commander with a fake account
    CommanderInfo fakePc;
    commanderInfoInit(&fakePc);

    fakePc.pos = session->game.commanderSession.currentCommander.pos;
    fakePc.base.accountId = r1emuGenerateRandom64(&self->seed);
    fakePc.pcId = r1emuGenerateRandom(&self->seed);
    strncpy(fakePc.base.familyName, "Dummy", sizeof(fakePc.base.familyName));
    strncpy(fakePc.base.commanderName, "Fake", sizeof(fakePc.base.commanderName));
    zoneBuilderEnterPc(&fakePc, replyMsg);

    // register the fake socket session
    SocketSession fakeSocketSession;
    uint64_t sessionKey = r1emuGenerateRandom64(&self->seed);
    uint8_t sessionKeyStr[SOCKET_SESSION_ID_SIZE];

    socketSessionGenSessionKey((uint8_t *)&sessionKey, sessionKeyStr);
    sprintf(sessionKeyStr, "%.10I64x", sessionKey);
    socketSessionInit(&fakeSocketSession, fakePc.base.accountId, self->info.routerId, session->socket.mapId, sessionKeyStr, true);

    RedisSocketSessionKey socketKey = {
        .routerId = self->info.routerId,
        .sessionKey = sessionKeyStr
    };

    redisUpdateSocketSession(self->redis, &socketKey, &fakeSocketSession);

    // register the fake game session
    GameSession fakeGameSession;
    gameSessionInit(&fakeGameSession, &fakePc);
    accountSessionInit(&fakeGameSession.accountSession, "DummyPC", sessionKeyStr, ACCOUNT_SESSION_PRIVILEGES_ADMIN);

    RedisGameSessionKey gameKey = {
        .routerId  = fakeSocketSession.routerId,
        .mapId     = fakeSocketSession.mapId,
        .accountId = fakeSocketSession.accountId
    };

    redisUpdateGameSession(self->redis, &gameKey, sessionKeyStr, &fakeGameSession);
    info("Fake PC spawned.(SocketId=%s, Acc=%I64x, PcID=%#x)", sessionKeyStr, fakePc.base.accountId, fakePc.pcId);
}

void adminCmdJump(Session *session, zmsg_t *replyMsg, char *args) {
    if (args == NULL) {
        info("Jump without argument!");
        // we must add a random with the map max x/y
    }
    else {
        char *argsParse;

        info("Jump with argument: %s", args);
        argsParse = strtok(args, " ");
        int argc = 0;
        while (argsParse != NULL) {
            argc++;
            argsParse = strtok(NULL, " ");
        }
        if (argc != 3) {
            info("Wrong number of argument, must be 3.");
        }
        else {
            PositionXYZ position;
            argsParse = strtok(args, " ");
            position.x = atoi(argsParse);
            argsParse = strtok(NULL, " ");
            position.y = atoi(argsParse);
            argsParse = strtok(NULL, " ");
            position.z = atoi(argsParse);
            argsParse = strtok(NULL, " ");
            zoneBuilderSetPos(session->game.commanderSession.currentCommander.pcId, &position, replyMsg);
        }
    }
}
