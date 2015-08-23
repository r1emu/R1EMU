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
#include "AdminCmd.h"
#include "Common/Commander/Commander.h"
#include "Common/Redis/Fields/RedisGameSession.h"
#include "Common/Redis/Fields/RedisSocketSession.h"
#include "ZoneServer/ZoneHandler/ZoneBuilder.h"
#include "Common/Session/Session.h"
#include "Common/Server/Worker.h"


// ------ Structure declaration -------


// ------ Static declaration -------


// ------ Extern function implementation -------
void
AdminCmd_process (
    Worker *self,
    char *command,
    Session *session,
    zmsg_t *replyMsg
) {
    if (strncmp (command, "spawn", strlen ("spawn")) == 0) {
        AdminCmd_spawnPc (self, session, replyMsg);
    }
}

void
AdminCmd_spawnPc (
    Worker *self,
    Session *session,
    zmsg_t *replyMsg
) {
    // Add a fake commander with a fake account
    CommanderInfo fakePc;
    CommanderInfo_init (&fakePc);

    fakePc.pos = session->game.commanderSession.currentCommander.pos;
    fakePc.base.accountId = R1EMU_generate_random64 (&self->seed);
    fakePc.pcId = R1EMU_generate_random (&self->seed);
    strncpy (fakePc.base.familyName, "Dummy", sizeof (fakePc.base.familyName));
    strncpy (fakePc.base.commanderName, "Fake", sizeof (fakePc.base.commanderName));
    ZoneBuilder_enterPc (&fakePc, replyMsg);

    // Register the fake socket session
    SocketSession fakeSocketSession;
    uint64_t sessionKey = R1EMU_generate_random64 (&self->seed);
    uint8_t sessionKeyStr [SOCKET_SESSION_ID_SIZE];
    SocketSession_genSessionKey ((uint8_t *) &sessionKey, sessionKeyStr);
    sprintf (sessionKeyStr, "%.10I64x", sessionKey);
    SocketSession_init (&fakeSocketSession, fakePc.base.accountId, self->info.routerId, session->socket.mapId, sessionKeyStr, true);
    RedisSocketSessionKey socketKey = {
        .routerId = self->info.routerId,
        .sessionKey = sessionKeyStr
    };
    Redis_updateSocketSession (self->redis, &socketKey, &fakeSocketSession);

    // Register the fake game session
    GameSession fakeGameSession;
    GameSession_init (&fakeGameSession, &fakePc);
    AccountSession_init (&fakeGameSession.accountSession, "DummyPC", sessionKeyStr, ACCOUNT_SESSION_PRIVILEGES_ADMIN);

    RedisGameSessionKey gameKey = {
        .routerId  = fakeSocketSession.routerId,
        .mapId     = fakeSocketSession.mapId,
        .accountId = fakeSocketSession.accountId
    };
    Redis_updateGameSession (self->redis, &gameKey, sessionKeyStr, &fakeGameSession);

    info ("Fake PC spawned. (SocketId=%s, Acc=%I64x, PcID=%#x)", sessionKeyStr, fakePc.base.accountId, fakePc.pcId);
}
