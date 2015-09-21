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
#include "common/commander/inventory.h"
#include "common/item/item.h"
#include "common/redis/fields/redis_game_session.h"
#include "common/redis/fields/redis_socket_session.h"
#include "common/session/session.h"
#include "common/server/worker.h"
#include "common/server/event_handler.h"
#include "zone_server/zone_handler/zone_builder.h"
#include "zone_server/zone_handler/zone_event_handler.h"

zhash_t *adminCommands = NULL;

bool adminCmdInit(void) {
    // initialize the admin commands hashtable
    if (!(adminCommands = zhash_new())) {
        error ("Cannot initialize admin commands hashtable correctly.");
        return false;
    }

    zhash_insert(adminCommands, "spawn",          adminCmdSpawnPc);
    zhash_insert(adminCommands, "jump",           adminCmdJump);
    zhash_insert(adminCommands, "additem",        adminCmdAddItem);
    zhash_insert(adminCommands, "test",           adminCmdTest);
    zhash_insert(adminCommands, "where",          adminCmdWhere);
    zhash_insert(adminCommands, "changeCamera",   adminCmdChangeCamera);
    zhash_insert(adminCommands, "setStamina",     adminCmdSetStamina);
    zhash_insert(adminCommands, "setSP",          adminCmdSetSP);
    zhash_insert(adminCommands, "setLevel",       adminCmdSetLevel);

    return true;
}

void adminCmdProcess(Worker *self, char *command, Session *session, zmsg_t *replyMsg) {

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
    Commander fakePc;
    commanderInit(&fakePc);

    fakePc.info.pos = session->game.commanderSession.currentCommander.info.pos;
    fakePc.info.appearance.accountId = r1emuGenerateRandom64(&self->seed);
    fakePc.info.socialInfoId = r1emuGenerateRandom64(&self->seed);
    fakePc.info.pcId = r1emuGenerateRandom(&self->seed);
    fakePc.info.commanderId = r1emuGenerateRandom64(&self->seed);
    snprintf(fakePc.info.appearance.familyName, sizeof(fakePc.info.appearance.familyName),
        "PcID_%x", fakePc.info.pcId);
    snprintf(fakePc.info.appearance.commanderName, sizeof(fakePc.info.appearance.commanderName),
        "AccountID_%llx", fakePc.info.appearance.accountId);

    // register the fake socket session
    SocketSession fakeSocketSession;
    uint32_t sessionKey = r1emuGenerateRandom(&self->seed);
    uint8_t sessionKeyStr[SOCKET_SESSION_ID_SIZE];

    socketSessionGenSessionKey((uint8_t *)&sessionKey, sessionKeyStr);
    sprintf(sessionKeyStr, "%.08x", sessionKey);
    socketSessionInit(&fakeSocketSession, fakePc.info.appearance.accountId, self->info.routerId, session->socket.mapId,
        sessionKeyStr, true);

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
    info("Fake PC spawned.(SocketID=%s, SocialID=%I64x, AccID=%I64x, PcID=%x, CommID=%I64x)",
         sessionKeyStr, fakePc.info.socialInfoId, fakePc.info.appearance.accountId, fakePc.info.pcId, fakePc.info.commanderId);

    GameEventEnterPc event = {
        .updatePosEvent = {
            .mapId = fakeSocketSession.mapId,
            .info = fakePc.info
        }
    };

    workerDispatchEvent(self, sessionKeyStr, EVENT_TYPE_ENTER_PC, &event, sizeof(event));
}

void adminCmdAddItem(Worker *self, Session *session, char *args, zmsg_t *replyMsg) {

    uint32_t itemType = strtol(args, &args, 10);
    args++;
    uint32_t amount = strtol(args, &args, 10);

    uint32_t itemPosition = 1;

    Inventory *inventory = &session->game.commanderSession.currentCommander.inventory;

    Item newItem;
    newItem.itemId = r1emuGenerateRandom64(&self->seed);
    newItem.itemType = itemType;
    newItem.amount =  (!amount) ? 1 : amount;
    newItem.equipSlot = EQSLOT_BODY_ARMOR;
    newItem.attributes = itemAttributesNew(4200, 0, NULL, NULL, NULL, 0, 0);
    newItem.inventoryIndex = INVENTORY_CAT_SIZE * INVENTORY_CAT_CONSUMABLE + itemPosition;
    inventoryAddItem(inventory, &newItem);



    zoneBuilderItemAdd(&newItem, INVENTORY_ADD_PICKUP, replyMsg);
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
            session->game.commanderSession.currentCommander.info.pos = position;
            zoneBuilderSetPos(session->game.commanderSession.currentCommander.info.pcId, &position, replyMsg);
        }
        free(arg);
    }
}

void adminCmdTest(Worker *self, Session *session, char *args, zmsg_t *replyMsg) {
    info("Test command launched.");
    size_t memSize;
    void *memory = dumpToMem(
        "[03:12:32][main.c:30 in writePacketToFile]  E9 0C 73 2A 86 02 35 00 A2 4E 00 00 02 01 05 00 | ..s*..5..N......\n"
        "[03:12:32][main.c:30 in writePacketToFile]  4E 61 6D 65 00 0C 00 44 61 72 6B 48 6F 72 69 7A | Name...DarkHoriz\n"
        "[03:12:32][main.c:30 in writePacketToFile]  6F 6E 00 04 00 57 68 6F 00 0A 00 4C 6F 74 68 62 | on...Who...Lothb\n"
        "[03:12:32][main.c:30 in writePacketToFile]  72 6F 6F 6B 00                                  | rook."
      , NULL, &memSize
    );

    zmsg_add(replyMsg, zframe_new(memory, memSize));
}

void adminCmdWhere(Worker *self, Session *session, char *args, zmsg_t *replyMsg) {
    const uint16_t MAX_LEN = 128;
    char message[MAX_LEN];
    PositionXYZ position;
    position = session->game.commanderSession.currentCommander.info.pos;
    snprintf(message, sizeof(message), "[%hu] x = %.0f, y = %.0f, z = %.0f",
        session->game.commanderSession.mapId,
        position.x, position.y, position.z);

    zoneBuilderChat(&session->game.commanderSession.currentCommander.info, message, replyMsg);
}

void adminCmdChangeCamera(Worker *self, Session *session, char *args, zmsg_t *replyMsg) {
    const uint16_t MAX_LEN = 128;
    char message[MAX_LEN];
    PositionXYZ pos;
    float fspd;
    float ispd;

    info("Change Camera command used, args = %s", args);
    if (strlen (args) == 0) {
        pos.x = 0;
        pos.y = 0;
        pos.z = 0;
        zoneBuilderChangeCamera((uint8_t)0, &pos, (float)0, (float)0, replyMsg);
    }
    else {
        char **arg;
        int argc;

        arg = strSplit(args, ' ');
        argc = 0;
        while (arg[++argc] != NULL);
        if (argc >= 3) {
            pos.x = (strlen(arg[0]) == 1 && arg[0][0] == 'c') ?
                session->game.commanderSession.currentCommander.info.pos.x : atof(arg[0]);
            pos.y = (strlen(arg[1]) == 1 && arg[1][0] == 'c') ?
                session->game.commanderSession.currentCommander.info.pos.y : atof(arg[1]);
            pos.z = (strlen(arg[2]) == 1 && arg[2][0] == 'c') ?
                session->game.commanderSession.currentCommander.info.pos.z : atof(arg[2]);
        }
        if (argc == 3)
            zoneBuilderChangeCamera((uint8_t)1, &pos, (float)10, (float)0.7, replyMsg);
        else if (argc == 5) {
            fspd = atof(arg[3]);
            ispd = atof(arg[4]);
            zoneBuilderChangeCamera((uint8_t)1, &pos, fspd, ispd, replyMsg);
        }
        else {
            snprintf(message, sizeof(message), "Bad usage /changeCamera <x> <y> <z> {<fspd> <ispd>}");
            zoneBuilderChat(&session->game.commanderSession.currentCommander.info, message, replyMsg);
        }
        free(arg);
    }
}

void adminCmdSetStamina(Worker *self, Session *session, char *args, zmsg_t *replyMsg) {
    if (strlen (args) == 0) {
        info("Set stamina needs a argument!");
    }
    else {
        char **arg;
        int argc;

        info("Set stamina with argument: %s", args);
        arg = strSplit(args, ' ');
        argc = 0;
        while (arg[++argc] != NULL);
        if (argc != 1) {
            info("Wrong number of arguments, must be 1.");
        }
        else {
            uint32_t stamina = atoi(arg[0]) * 1000;
            info("Setting stamina to %d.", stamina);
            session->game.commanderSession.currentCommander.info.currentStamina = stamina;
            zoneBuilderStamina(stamina, replyMsg);
        }
        free(arg);
    }
}

void adminCmdSetSP(Worker *self, Session *session, char *args, zmsg_t *replyMsg) {
    if (strlen (args) == 0) {
        info("Set SP needs a argument!");
    }
    else {
        char **arg;
        int argc;

        info("Set SP with argument: %s", args);
        arg = strSplit(args, ' ');
        argc = 0;
        while (arg[++argc] != NULL);
        if (argc != 1) {
            info("Wrong number of arguments, must be 1.");
        }
        else {
            uint32_t sp = atoi(arg[0]);
            info("Setting SP to %d.", sp);
            session->game.commanderSession.currentCommander.info.currentSP = sp;
            zoneBuilderUpdateSP(session->game.commanderSession.currentCommander.info.pcId, sp, replyMsg);
        }
        free(arg);
    }
}

void adminCmdSetLevel(Worker *self, Session *session, char *args, zmsg_t *replyMsg) {
    if (strlen (args) == 0) {
        info("Set level needs a argument!");
    }
    else {
        char **arg;
        int argc;

        info("Set level with argument: %s", args);
        arg = strSplit(args, ' ');
        argc = 0;
        while (arg[++argc] != NULL);
        if (argc != 1) {
            info("Wrong number of arguments, must be 1.");
        }
        else {
            uint32_t level = atoi(arg[0]);
            info("Setting level to %d.", level);
            session->game.commanderSession.currentCommander.info.appearance.level = level;
            zoneBuilderPCLevelUp(session->game.commanderSession.currentCommander.info.pcId, level, replyMsg);
        }
        free(arg);
    }
}
