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

zhash_t *adminCommands = NULL;

void adminCmdProcess(Worker *self, char *command, Session *session, zmsg_t *replyMsg) {

    if (adminCommands == NULL) {
        // initialize the admin commands hashtable
        adminCommands = zhash_new();
        zhash_insert(adminCommands, "spawn",   adminCmdSpawnPc);
        zhash_insert(adminCommands, "jump",    adminCmdJump);
        zhash_insert(adminCommands, "itemAdd", adminCmdAddItem);
    }

    void (*handler) (Worker *self, Session *session, char *args, zmsg_t *replyMsg);
    char *commandName = strtok(command, " ");
    if (!commandName) {
        warning ("Cannot read command '%s'", command);
        return;
    }

    handler = zhash_lookup(adminCommands, commandName);
    if (!handler) {
        warning ("No admin command '%s' found. (entire command : '%s')", commandName, command);
        return;
    }

    handler(self, session, command + strlen (commandName) + 1, replyMsg);
}

void adminCmdSpawnPc(Worker *self, Session *session, char *args, zmsg_t *replyMsg) {
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

void adminCmdAddItem(Worker *self, Session *session, char *args, zmsg_t *replyMsg) {
    uint32_t itemId = strtol(args, &args, 10);
    args++;
    uint32_t amount = strtol(args, &args, 10);

    ItemPkt item = {
        .uniqueId = r1emuGenerateRandom64(&self->seed),
        .amount = (!amount) ? 1 : amount,
        .inventoryIndex = INVENTORY_MAX_SIZE * INVENTORY_CAT_CONSUMABLE + 1,
        .id = itemId
    };
    zoneBuilderItemAdd(&item, INVENTORY_ADD_PICKUP, replyMsg);
}

void adminCmdJump(Worker *self, Session *session, char *args, zmsg_t *replyMsg) {
    if (strlen (args) == 0) {
        info("Jump without argument!");
        // we must add a random with the map max x/y
    }
    else {
        char **arg;
        int argc;

        info("Jump with argument: %s", args);
        arg = strSplit(args, ' ');
        argc = 0;
        while (arg[++argc] != NULL);
        if (argc != 3) {
            info("Wrong number of argument, must be 3.");
        }
        else {
            PositionXYZ position;
            position.x = atof(arg[0]);
            info("x = %.6f", position.x);
            position.y = atof(arg[1]);
            info("y = %.6f", position.y);
            position.z = atof(arg[2]);
            info("z = %.6f", position.z);
            session->game.commanderSession.currentCommander.pos = position;
            zoneBuilderSetPos(session->game.commanderSession.currentCommander.pcId, &position, replyMsg);
        }
        free(arg);
    }
}
