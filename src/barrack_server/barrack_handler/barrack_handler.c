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

#include "barrack_handler.h"
#include "barrack_builder.h"
#include "common/utils/random.h"
#include "common/packet/packet.h"
#include "common/server/worker.h"
#include "common/commander/commander.h"
#include "common/commander/inventory.h"
#include "common/item/item.h"
#include "common/packet/packet_stream.h"
#include "common/redis/fields/redis_game_session.h"
#include "common/redis/fields/redis_socket_session.h"
#include "common/mysql/fields/mysql_account_session.h"
#include "common/mysql/fields/mysql_commander.h"

/** Read the passport and accepts or refuse the authentification */
static PacketHandlerState barrackHandlerLoginByPassport  (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *reply);
/** Read the login / password and accepts or refuse the authentification */
static PacketHandlerState barrackHandlerLogin            (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *reply);
/** Start the barrack : call other handlers that initializes the barrack */
static PacketHandlerState barrackHandlerStartBarrack     (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *reply);
/** Once the commander list has been received, request to start the barrack */
static PacketHandlerState barrackHandlerCurrentBarrack   (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *reply);
/** Change a barrack name */
static PacketHandlerState barrackHandlerBarrackNameChange(Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *reply);
/** Create a commander */
static PacketHandlerState barrackHandlerCommanderCreate  (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *reply);
/** Send a list of zone servers */
static PacketHandlerState barrackHandlerCommanderDestroy (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *reply);
/** Change the commander position in the barrack */
static PacketHandlerState barrackHandlerCommanderMove    (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *reply);
/** Request for the player to enter in game */
static PacketHandlerState barrackHandlerStartGame        (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *reply);
/** Request for the player to logout */
static PacketHandlerState barrackHandlerLogout        (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *reply);

/**
 * @brief barrackHandlers is a global table containing all the barrack handlers.
 */
const PacketHandler barrackHandlers[PACKET_TYPE_COUNT] = {
    #define REGISTER_PACKET_HANDLER(packetName, handler) \
       [packetName] = {handler, STRINGIFY(packetName)}

    REGISTER_PACKET_HANDLER(CB_LOGIN,              barrackHandlerLogin),
    REGISTER_PACKET_HANDLER(CB_LOGIN_BY_PASSPORT,  barrackHandlerLoginByPassport),
    REGISTER_PACKET_HANDLER(CB_START_BARRACK,      barrackHandlerStartBarrack),
    REGISTER_PACKET_HANDLER(CB_CURRENT_BARRACK,    barrackHandlerCurrentBarrack),
    REGISTER_PACKET_HANDLER(CB_BARRACKNAME_CHANGE, barrackHandlerBarrackNameChange),
    REGISTER_PACKET_HANDLER(CB_COMMANDER_CREATE,   barrackHandlerCommanderCreate),
    REGISTER_PACKET_HANDLER(CB_COMMANDER_DESTROY,  barrackHandlerCommanderDestroy),
    REGISTER_PACKET_HANDLER(CB_COMMANDER_MOVE,     barrackHandlerCommanderMove),
    // REGISTER_PACKET_HANDLER(CB_JUMP,               barrackHandlerJump),
    REGISTER_PACKET_HANDLER(CB_START_GAME,         barrackHandlerStartGame),
    REGISTER_PACKET_HANDLER(CB_LOGOUT,         barrackHandlerLogout),

    #undef REGISTER_PACKET_HANDLER
};

static PacketHandlerState barrackHandlerLogin(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *reply)
{
    #pragma pack(push, 1)
    struct {
        uint8_t login[ACCOUNT_SESSION_LOGIN_MAXSIZE];
        uint8_t md5Password[17];
        uint8_t unk1[5]; // Game version?
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

    CHECK_CLIENT_PACKET_SIZE(*clientPacket, packetSize, CB_LOGIN);

    // Check if client/version servers are the same
    /*
    if (clientPacket.clientVersion != _SERVER_VERSION) {
        barrackBuilderMessage(BC_MESSAGE_VERSION_MISSMATCH, nullptr, reply);
        return PACKET_HANDLER_OK;
    }
    */

    // Get accountData from database

    AccountSession *accountSession = &session->game.accountSession;

    // Initialize Account Session
    accountSessionInit(&session->game.accountSession,
        clientPacket->login, session->socket.sessionKey,
        session->game.accountSession.privilege);

    mySqlGetAccountData(self->sqlConn, clientPacket->login, clientPacket->md5Password, accountSession);

    // Check if user/pass incorrect
    if (accountSession->accountId == 0) {
        barrackBuilderMessage(BC_MESSAGE_USER_PASS_INCORRECT_1, "", reply);
        return PACKET_HANDLER_OK;
    } else {
        // Check if user is banned
        if (accountSession->isBanned) {
            barrackBuilderMessage(BC_MESSAGE_ACCOUNT_BLOCKED_2, "", reply);
            return PACKET_HANDLER_OK;
        }
        // Check if user is already logged-in
        /*
        RedisAccountSessionKey accountKey = {
            .accountId = accountSession->accountId
        };

        AccountSession otherAccountSession;

        if (redisGetAccountSession(self->redis, &accountKey, &otherAccountSession)) {
            barrackBuilderMessage(BC_MESSAGE_ALREADY_LOGGEDIN, "", reply);
            return PACKET_HANDLER_OK;
        }
        */
    }

    // authentication OK!
    session->socket.authenticated = true;

    // update the session
    session->socket.accountId = accountSession->accountId;

    info("AccountID %llx generated !", session->socket.accountId);

    barrackBuilderLoginOk(
        session->socket.accountId,
        session->game.accountSession.login,
        "*0FC621B82495C18DEC8D8D956C82297BEAAAA858",
        session->game.accountSession.privilege,
        reply
    );


    size_t memSize;
    void *memory = dumpToMem(
"[03:11:50][main.c:30 in writePacketToFile]  2B 0C FF FF FF FF 18 06 59 00 00 00 8D FA 08 06 | ........Y.......\n"
"[03:11:50][main.c:30 in writePacketToFile]  65 96 7B 4C D5 65 18 C7 9F DF F9 9D C3 5D 7F 47 | ........Y.......\n"
"[03:11:50][main.c:30 in writePacketToFile]  11 2D B5 02 CD CB DC 50 50 48 BC A0 E7 02 28 0A | ........Y.......\n"
"[03:11:50][main.c:30 in writePacketToFile]  5E 52 03 51 F4 1C F5 44 BA 80 D6 12 D3 B5 72 E5 | ........Y.......\n"
"[03:11:50][main.c:30 in writePacketToFile]  0D D9 44 9D B7 1C CD BC 26 59 69 0B A7 E2 C4 3F | ........Y.......\n"
"[03:11:50][main.c:30 in writePacketToFile]  D2 22 D1 A4 59 EA 51 70 5D AC 48 67 EA DC CA CA | ........Y.......\n"
"[03:11:50][main.c:30 in writePacketToFile]  AD 9E F7 7D 9E 97 F3 7B ED 19 63 1C CE E7 7C BF | ........Y.......\n"
"[03:11:50][main.c:30 in writePacketToFile]  CF ED 7D 7F A7 F7 99 58 88 82 E0 AB 1B C2 00 85 | ........Y.......\n"
"[03:11:50][main.c:30 in writePacketToFile]  06 00 FE C8 C8 59 0A F0 F2 D2 10 1C E8 2A 5E 1E | ........Y.......\n"
"[03:11:50][main.c:30 in writePacketToFile]  FA 33 1A 12 20 7D 68 43 83 09 05 36 AA D7 20 00 | ........Y.......\n"
"[03:11:50][main.c:30 in writePacketToFile]  0B FC 2F B5 22 75 CF 6B 5A 82 2D 2A EE 8A 8A 90 | ........Y.......\n"
"[03:11:50][main.c:30 in writePacketToFile]  9E F4 18 3B 16 15 D3 B2 52 D3 48 F1 C7 94 38 A4 | ...;....R.H...8.\n"
"[03:11:50][main.c:30 in writePacketToFile]  92 67 1A 3F 3B 34 C5 05 B7 00 5C A8 82 54 60 C9 | .g.?;4....a..T`.\n"
"[03:11:50][main.c:30 in writePacketToFile]  84 8A B5 82 82 82 EF 8F E9 54 FB 65 80 60 45 59 | .........T.e.`EY\n"
"[03:11:50][main.c:30 in writePacketToFile]  88 B4 A2 64 0D E0 78 A2 D1 90 5A 4E 91 06 C6 04 | ...d..x...ZN....\n"
"[03:11:50][main.c:30 in writePacketToFile]  E1 98 9E 99 8A 54 F3 B0 09 CB 5B 45 0D A1 E4 83 | .....T....[E....\n"
"[03:11:50][main.c:30 in writePacketToFile]  2E 03 26 DB 6A E8 86 35 A4 A5 81 2B 05 B3 EF E1 | ..&.j..5...+....\n"
"[03:11:50][main.c:30 in writePacketToFile]  A3 1A C2 5D 44 0D 16 74 DB E1 80 7C 1B 3B 06 15 | ...]D..t...|.;..\n"
"[03:11:50][main.c:30 in writePacketToFile]  13 EB 77 9E 20 DF 53 61 D1 BB 4D E3 72 B7 12 A5 | ..w. .Sa..M.r...\n"
"[03:11:50][main.c:30 in writePacketToFile]  7C 83 75 00 B3 9A FA 3C 22 6A 98 A4 5A AC B2 6A | |.u....<aj..Z..j\n"
"[03:11:50][main.c:30 in writePacketToFile]  9D 0A 20 05 50 E2 26 6A 9F A4 CA E0 D8 1A 9D 9A | .. .P.&j........\n"
"[03:11:50][main.c:30 in writePacketToFile]  8F 54 83 05 01 A2 26 49 AA D9 57 3E 94 28 87 90 | .T....&I..W>.(..\n"
"[03:11:50][main.c:30 in writePacketToFile]  C0 78 5E 68 79 FA E6 11 B5 E8 AA C8 EB 86 35 7D | .x^hy.........5}\n"
"[03:11:50][main.c:30 in writePacketToFile]  90 4E CD 45 AA D9 57 CC 8E 63 25 05 30 75 00 51 | .N.E..W..c%.0u.Q\n"
"[03:11:50][main.c:30 in writePacketToFile]  6A F2 D3 65 5E 72 02 28 BA 49 3A 02 EC 8C D2 A9 | j..e^r.(.I:.....\n"
"[03:11:50][main.c:30 in writePacketToFile]  12 8D CA 96 5A 4D 56 E2 5D 43 F6 CB 14 49 61 CC | ....ZMV.]C...Ia.\n"
"[03:11:50][main.c:30 in writePacketToFile]  46 CA E3 B9 69 51 5E 25 EC F8 E5 1D A2 70 B2 32 | F...iQ^%.....p.2\n"
"[03:11:50][main.c:30 in writePacketToFile]  8A 91 6A C1 EE 13 15 66 6A FA 46 13 72 6D BD 5F | ..j....fj.F.rm._\n"
"[03:11:50][main.c:30 in writePacketToFile]  88 94 D0 25 AA CD 25 26 74 DA 5A 52 1B 0D 39 36 | ...%..%&t.ZR..96\n"
"[03:11:50][main.c:30 in writePacketToFile]  6A 14 4E A8 D9 77 94 1D DF 4A 92 BB 08 BB 2A A2 | j.N..w...J....*.\n"
"[03:11:50][main.c:30 in writePacketToFile]  24 A5 F2 F2 23 75 A3 D3 71 F7 15 D1 2F 80 79 4C | $...#u..q.../.yL\n"
"[03:11:50][main.c:30 in writePacketToFile]  A9 4E 14 69 35 B6 CE 11 5A 25 EE EF 5E 24 2D 45 | .N.i5...Z%..^$-E\n"
"[03:11:50][main.c:30 in writePacketToFile]  65 A1 96 08 CA 6B 57 19 39 B6 97 EA D4 48 A4 9A | e....kW.9....H..\n"
"[03:11:50][main.c:30 in writePacketToFile]  3A 1D 47 73 8D E7 66 10 15 4D 12 50 A8 39 86 24 | :.Gs..f..M.P.9.$\n"
"[03:11:50][main.c:30 in writePacketToFile]  55 EC 6E 9E 42 94 8B A9 C9 92 9A CD 73 2C E2 09 | U.n.B.......s,..\n"
"[03:11:50][main.c:30 in writePacketToFile]  F5 FA D4 01 A3 6C 9D 78 01 A9 D3 9D 8E 63 A4 56 | .....l.x.....c.V\n"
"[03:11:50][main.c:30 in writePacketToFile]  B2 77 C7 78 13 32 91 52 BB 3A 49 6A 05 BC 94 FD | .w.x.2.R.:Ij....\n"
"[03:11:50][main.c:30 in writePacketToFile]  EE FD D1 58 63 B6 BB B2 36 01 32 6C 5A 3D F1 8C | ...Xc...6.2lZ=..\n"
"[03:11:50][main.c:30 in writePacketToFile]  00 F4 74 D3 E9 A8 A4 53 09 B5 6D F1 1A D5 86 A7 | ..t....S..m.....\n"
"[03:11:50][main.c:30 in writePacketToFile]  72 B6 5B 75 E2 07 79 C2 3B 60 66 13 51 6A 57 17 | r.[u..y.;`f.QjW.\n"
"[03:11:50][main.c:30 in writePacketToFile]  E1 09 C7 10 8E 78 C2 17 CB EC 8F 5B 83 4F 12 D5 | .....x.....[.O..\n"
"[03:11:50][main.c:30 in writePacketToFile]  45 BE 89 67 17 F3 0A 78 63 B9 C6 91 DC AF E4 C3 | E..g...xc.......\n"
"[03:11:50][main.c:30 in writePacketToFile]  71 9A 63 2E 52 3D DD CA 31 95 3B 51 FF A4 01 69 | q.c.R=..1.;Q...i\n"
"[03:11:50][main.c:30 in writePacketToFile]  98 BD 72 CC 11 35 7A EA 72 A9 C6 A2 A7 68 42 77 | ..r..5z.r....hBw\n"
"[03:11:50][main.c:30 in writePacketToFile]  DF 33 61 A8 8D 9A 88 13 7A C5 DB C0 9B F3 D9 7C | .3a.....z......|\n"
"[03:11:50][main.c:30 in writePacketToFile]  13 6F 0B 80 3E DB 89 42 50 46 12 76 22 D6 0D 60 | .o..>..BPF.va..`\n"
"[03:11:50][main.c:30 in writePacketToFile]  BF F1 88 5D E1 9F B8 4D 67 7B C8 AE 39 FD FF 67 | ...]...Mg{..9..g\n"
"[03:11:50][main.c:30 in writePacketToFile]  AF 59 55 F9 3A DB 1D D9 5F D0 C1 CE FE 5B 44 99 | .YU.:..._....[D.\n"
"[03:11:50][main.c:30 in writePacketToFile]  3E C8 22 36 86 52 80 3C CC F4 58 E7 64 3B E8 26 | >.A6.R.<..X.d;.&\n"
"[03:11:50][main.c:30 in writePacketToFile]  F5 16 B4 19 B2 9E 24 A6 42 B2 CF 2B FC 54 F5 27 | ......$.B..+.T.'\n"
"[03:11:50][main.c:30 in writePacketToFile]  DF C4 42 77 EC D5 BD 0F 9D 90 6A 9B 6C E3 3A 81 | ..Bw......j.l.:.\n"
"[03:11:50][main.c:30 in writePacketToFile]  17 B9 3F 17 F7 BC E7 38 FE DE E6 A7 29 1F C1 4F | ..?....8....)..O\n"
"[03:11:50][main.c:30 in writePacketToFile]  88 53 B1 EE B1 4F 9C 90 9F C0 B3 88 19 00 F4 E7 | .S...O..........\n"
"[03:11:50][main.c:30 in writePacketToFile]  9E 9F DF AD EB 2E 10 3D C7 20 F7 1E 61 03 DD 01 | .......=. ..a...\n"
"[03:11:50][main.c:30 in writePacketToFile]  B2 B6 E8 54 75 A2 84 E0 5B F4 4D F7 77 47 C5 26 | ...Tu...[.M.wG.&\n"
"[03:11:50][main.c:30 in writePacketToFile]  BE 81 1B 2F 92 FB 8A 3D 31 30 04 F3 55 27 F7 39 | .../...=10..U'.9\n"
"[03:11:50][main.c:30 in writePacketToFile]  AC 5D 04 E9 B6 5C 22 AA 20 DB 25 29 35 A5 AF 0E | .]...\". .%)5...\n"
"[03:11:50][main.c:30 in writePacketToFile]  DB A9 4B 48 09 F7 F0 48 9D 3A C7 94 A8 7D A5 87 | ..KH...H.:...}..\n"
"[03:11:50][main.c:30 in writePacketToFile]  14 45 07 82 39 D3 78 47 6A 52 E3 61 20 8A AA 4D | .E..9.xGjR.a ..M\n"
"[03:11:50][main.c:30 in writePacketToFile]  1A A1 B9 F7 A7 E7 0E 5C 1F 40 94 CA 71 5C 84 C2 | .......a.@..qa..\n"
"[03:11:50][main.c:30 in writePacketToFile]  3B 79 14 EF AE 59 EB 94 5A 2E 4A 0D FC 5A 87 1E | ;y...Y..Z.J..Z..\n"
"[03:11:50][main.c:30 in writePacketToFile]  FE 4A 95 DC 2F 34 E1 1F 24 44 E7 45 CC D4 A8 AA | .J../4..$D.E....\n"
"[03:11:50][main.c:30 in writePacketToFile]  7E 31 30 02 FF 3B 29 9E 28 55 6F A2 DC 34 80 04 | ~10..;).(Uo..4..\n"
"[03:11:50][main.c:30 in writePacketToFile]  EC A0 C7 23 76 E8 38 3F 8D 57 59 31 50 18 7C 7D | ...#v.8?.WY1P.|}\n"
"[03:11:50][main.c:30 in writePacketToFile]  C6 B2 60 19 AC B5 5C 20 FF 58 2F FE 19 2A 0F 55 | ..`...a .X/..*.U\n"
"[03:11:50][main.c:30 in writePacketToFile]  BC 56 09 43 38 BD 53 D3 0C 78 88 D2 CA 78 96 66 | .V.C8.S..x...x.f\n"
"[03:11:50][main.c:30 in writePacketToFile]  3C 98 A9 BF F1 EE 10 94 6A 88 4F A3 32 B9 21 BD | <.......j.O.2.!.\n"
"[03:11:50][main.c:30 in writePacketToFile]  FF 00 78 80 94 BA 64 86 47 1A 12 B8 95 37 9C A9 | ..x...d.G....7..\n"
"[03:11:50][main.c:30 in writePacketToFile]  DF 6E 13 15 8F A4 08 5F 84 C2 57 8B D9 F1 26 AE | .n....._..W...&.\n"
"[03:11:50][main.c:30 in writePacketToFile]  B1 D0 8A 13 08 C6 3C CD F1 69 D6 DA CB 54 0C 41 | ......<..i...T.A\n"
"[03:11:50][main.c:30 in writePacketToFile]  E0 D5 B4 46 B0 56 29 53 D8 1F 19 5E 4D 2B 83 A9 | ...F.V)S...^M+..\n"
"[03:11:50][main.c:30 in writePacketToFile]  BA 03 00 F7 F1 7D E5 58 AA 51 93 79 E1 E3 B6 E0 | .....}.X.Q.y....\n"
"[03:11:50][main.c:30 in writePacketToFile]  57 1E A4 54 8D D3 34 6A 19 6B 1D DD AC 53 53 35 | W..T..4j.k...SS5\n"
"[03:11:50][main.c:30 in writePacketToFile]  AA 9A 1F 3C 05 E5 06 DC 45 AD 9A 44 FC 85 91 AE | ...<....E..D....\n"
"[03:11:50][main.c:30 in writePacketToFile]  65 1F 4D 5F A8 C0 58 69 C2 1D 7C 57 F5 3E 57 A3 | e.M_..Xi..|W.>W.\n"
"[03:11:50][main.c:30 in writePacketToFile]  B2 D8 B1 FE 4D A2 12 A4 12 C0 0C CD B1 9D A9 94 | ....M...........\n"
"[03:11:50][main.c:30 in writePacketToFile]  D5 06 60 33 3A B3 9F A2 51 29 4C 6D 78 9B 28 B5 | ..`3:...Q)Lmx.(.\n"
"[03:11:50][main.c:30 in writePacketToFile]  66 05 1A 95 C5 9D 28 AE 32 A1 03 B5 14 35 47 A3 | f.....(.2....5G.\n"
"[03:11:50][main.c:30 in writePacketToFile]  06 F1 84 BE F0 11 15 CD 79 79 B4 EC 07 32 15 35 | ........yy...2.5\n"
"[03:11:50][main.c:30 in writePacketToFile]  4E A7 C6 47 28 3C 4A 07 17 D2 85 58 79 C4 A1 39 | N..G(<J....Xy..9\n"
"[03:11:50][main.c:30 in writePacketToFile]  66 47 28 54 AF E2 EC AD 14 80 0B B6 1A F3 B5 BC | fG(T............\n"
"[03:11:50][main.c:30 in writePacketToFile]  96 33 D5 FE 0C 51 2A AF 89 1A 55 C2 8F BA CA 77 | .3...Q*...U....w\n"
"[03:11:50][main.c:30 in writePacketToFile]  5C F0 81 AD C6 F0 65 2A 83 2E 8D A5 5B 63 E5 C3 | a.....e*....[c..\n"
"[03:11:50][main.c:30 in writePacketToFile]  22 67 0F C0 7E 1B 55 1F 24 EA AC BC 60 E8 C2 9C | Ag..~.U.$...`...\n"
"[03:11:50][main.c:30 in writePacketToFile]  C3 8A FE 8F 9D B0 CF C6 5E D3 14 AF F0 C5 36 D0 | ........^.....6.\n"
"[03:11:50][main.c:30 in writePacketToFile]  63 68 54 0B 5F 59 E4 DB C4 35 9C 1C 1C 05 7B 51 | chT._Y...5....{Q\n"
"[03:11:50][main.c:30 in writePacketToFile]  4B 6D 6C 9E 56 C3 3D D6 5A 7B C8 05 DB 6D 8E 17 | Kml.V.=.Z{...m..\n"
"[03:11:50][main.c:30 in writePacketToFile]  34 AD 9F 98 5A 53 E5 04 5C DA CE 69 9E D7 A8 F2 | 4...ZS..a..i....\n"
"[03:11:50][main.c:30 in writePacketToFile]  3B 74 4D 05 16 39 61 A3 8D 5A 28 96 09 03 F3 0A | ;tM..9a..Z(.....\n"
"[03:11:50][main.c:30 in writePacketToFile]  2C C9 AB 67 AD D6 B9 3A F5 B5 A6 D5 C8 D4 D9 8F | ,..g...:........\n"
"[03:11:50][main.c:30 in writePacketToFile]  5C 50 83 9F 55 FB 73 51 A3 1E D5 51 6F 7F 4F 34 | aP..U.sQ...Qo.O4\n"
"[03:11:50][main.c:30 in writePacketToFile]  A1 DA 46 BD 3B 5A F8 01 8C B7 00 DE F7 50 6F A7 | ..F.;Z.......Po.\n"
"[03:11:50][main.c:30 in writePacketToFile]  F2 16 F5 B3 88 55 5F 26 32 B4 FD 28 E5 09 64 F4 | .....U_&2..(..d.\n"
"[03:11:50][main.c:30 in writePacketToFile]  EE 0A 6B 50 41 F9 5E D7 26 30 80 B5 4E 59 3A 35 | ..kPA.^.&0..NY:5\n"
"[03:11:50][main.c:30 in writePacketToFile]  5A D3 2A 63 EA 4C A6 03 56 A1 96 29 52 C2 C8 8C | Z.*c.L..V..)R...\n"
"[03:11:50][main.c:30 in writePacketToFile]  50 78 E3 1D 3A D9 45 DE AB AB 3B 0C 49 1D EE 45 | Px..:.E...;.I..E\n"
"[03:11:50][main.c:30 in writePacketToFile]  D4 5F CF 92 37 4D B3 EF 6D EA AD 79 95 28 95 57 | ._..7M..m..y.(.W\n"
"[03:11:50][main.c:30 in writePacketToFile]  30 D2 DB 37 36 E7 FF 07                         | 0..76...\n",
        NULL, &memSize);
    buffer_print(memory, memSize, NULL);
    struct {
        VariableSizePacketHeader header;
        uint32_t itemsCount;
        ZlibHeader zlibHeader;
        char compressedData[0];
    } *packet1 = memory;
    Zlib zlib;
    zlibDecompress(&zlib, packet1->compressedData, packet1->zlibHeader.size);
    buffer_print(zlib.buffer, zlib.header.size, "Decompressed : ");

    return PACKET_HANDLER_UPDATE_SESSION;
}

static PacketHandlerState barrackHandlerLoginByPassport(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *reply) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint32_t unk1;
        uint8_t unk2; // 08
        uint16_t unk3; // 0110
        uint8_t passport[1011];
        uint32_t unk4;
        uint16_t unk5;
        uint64_t clientId;
        uint32_t clientId2;
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

    CHECK_CLIENT_PACKET_SIZE(*clientPacket, packetSize, CB_LOGIN_BY_PASSPORT);

    // Function disabled
    if (true) {
        char messageToSend[] = "Function disabled.";
        barrackBuilderMessage(BC_MESSAGE_CUSTOM_MSG, messageToSend, reply);
        return PACKET_HANDLER_UPDATE_SESSION;
    }

    // authenticate here
    // TODO

    // authentication OK!
    session->socket.authenticated = true;

    // update the session
    // gives a random account
    session->socket.accountId = r1emuGenerateRandom64(&self->seed);
    accountSessionInit(&session->game.accountSession, session->game.accountSession.login, session->socket.sessionKey, ACCOUNT_SESSION_PRIVILEGES_ADMIN);
    snprintf(session->game.accountSession.login, sizeof(session->game.accountSession.login), "%llX", session->socket.accountId);
    // ==================================
    info("Account %s generated !", session->game.accountSession.login);

    barrackBuilderLoginOk(
        session->socket.accountId,
        session->game.accountSession.login,
        "*0FC621B82495C18DEC8D8D956C82297BEAAAA858",
        session->game.accountSession.privilege,
        reply
    );

    return PACKET_HANDLER_UPDATE_SESSION;
}

static PacketHandlerState barrackHandlerStartGame(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *reply)
{
    #pragma pack(push, 1)
    struct {
        uint16_t routerId;
        uint8_t commanderListId;
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

    CHECK_CLIENT_PACKET_SIZE(*clientPacket, packetSize, CB_START_GAME);

    // Retrieve zone servers IPs from Redis
    // Fake IPs here until we can retrieve the IPs database
    uint32_t zoneServerIps[] = {
        *(uint32_t *)((char[]) {127, 0,   0,   1}),
        *(uint32_t *)((char[]) {46,  105, 97,  46}),
        *(uint32_t *)((char[]) {192, 168, 33,  10}),
        *(uint32_t *)((char[]) {37,  187, 102, 130}),
    };
    int maxServerCount = sizeof_array(zoneServerIps);
    if (clientPacket->routerId >= maxServerCount) {
        error("Invalid RouterId.");
        return PACKET_HANDLER_ERROR;
    }

    // Retrieve zone servers ports from Redis
    // Fake ports here until we can retrieve the ports database
    int zoneServerPorts[] = {
        2004, 2005, 2006, 2007
    };

    uint32_t zoneServerIp = zoneServerIps[clientPacket->routerId];
    int zoneServerPort = zoneServerPorts[clientPacket->routerId];

    // Move the GameSession to the target Zone
    RedisGameSessionKey fromKey = {
        .routerId = session->socket.routerId,
        .mapId = session->socket.mapId,
        .accountId = session->socket.accountId
    };
    RedisGameSessionKey toKey = {
        .routerId = clientPacket->routerId, // target zoneId
        .mapId = -1,
        .accountId = session->socket.accountId
    };
    if (!(redisMoveGameSession(self->redis, &fromKey, &toKey))) {
        error("Cannot move the Game session %s.", session->socket.sessionKey);
        return PACKET_HANDLER_ERROR;
    }

    // Build the answer packet
    barrackBuilderStartGameOk(
        self->info.routerId,
        zoneServerIp,
        zoneServerPort,
        session->game.commanderSession.mapId,
        clientPacket->commanderListId,
        session->game.commanderSession.currentCommander.info.socialInfoId,
        false,
        reply
    );

    return PACKET_HANDLER_OK;
}

static PacketHandlerState
barrackHandlerCommanderMove(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *reply)
{
    #pragma pack(push, 1)
    struct {
        uint8_t commanderListId;
        PositionXYZ position;
        float angleDestX, angleDestY;
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

    CHECK_CLIENT_PACKET_SIZE(*clientPacket, packetSize, CB_COMMANDER_MOVE);

    CommanderInfo *commander = &session->game.commanderSession.currentCommander.info;

    // TODO : Check position of the client

    // Update session
    memcpy(&commander->pos, &clientPacket->position, sizeof(PositionXZ));

    // Build packet
    barrackBuilderCommanderMoveOk(
        session->socket.accountId,
        clientPacket->commanderListId,
        &commander->pos,
        reply
    );

    return PACKET_HANDLER_UPDATE_SESSION;
}

static PacketHandlerState
barrackHandlerStartBarrack(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *reply)
{
    // CHECK_CLIENT_PACKET_SIZE(*clientPacket, packetSize, CB_START_BARRACK);

    // IES Modify List
    /*
    BarrackBuilder_iesModifyList(
        reply
    );
    */

    // ??
    /*
    BarrackBuilder_normalUnk1(
        session->socket.accountId,
        reply
    );
    */

    // Get list of Commanders for this AccountId

    Commander *commanders;
    CommanderInfo *commandersInfo;

    dbg("accountId: %11x", session->socket.accountId);

    int commandersCount = mySqlGetCommandersByAccountId(self->sqlConn, session->socket.accountId, &commandersInfo);

    session->game.accountSession.charactersCreatedCount = commandersCount;

    if (commandersCount == -1) {
        // Error
        /// TODO
        return PACKET_HANDLER_ERROR;
    } else {
        if ((commanders = malloc(sizeof(Commander) * commandersCount)) == NULL) {
            return PACKET_HANDLER_ERROR;
        }
        // Iterate and populate commanders;
        for (int i = 0; i < commandersCount; i++) {
            commanderInit(&commanders[i]);
            commanders[i].info = commandersInfo[i];
        }
    }



    // Send the commander list
    barrackBuilderCommanderList(
        session->socket.accountId,
        &session->game,
        commandersCount,
        commanders,
        reply
    );

    return PACKET_HANDLER_UPDATE_SESSION;
}

static PacketHandlerState barrackHandlerCurrentBarrack(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *reply)
{
    // CHECK_CLIENT_PACKET_SIZE(*clientPacket, packetSize, CB_CURRENT_BARRACK);

    //  [CLIENT SEND] Packet type : <CB_CURRENT_BARRACK>
    //   =================================================
    //    4E00 03000000 F7030000 D1A8014400000000 03000068 42F0968F 41000070 4111E334 3FCF2635 BF
    //    size pktType  checksum     accountId               float    float    float    float

    barrackBuilderPetInformation(reply);
    barrackBuilderZoneTraffics(1002, reply);

    return PACKET_HANDLER_OK;
}

static PacketHandlerState barrackHandlerBarrackNameChange(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *reply)
{

    BarrackNameResultType ResultType = BC_BARRACKNAME_CHANGE_OK;

    #pragma pack(push, 1)
    struct{
        uint8_t barrackName[64];
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

    CHECK_CLIENT_PACKET_SIZE(*clientPacket, packetSize, CB_BARRACKNAME_CHANGE);

    CommanderInfo *commanderInfo = &session->game.commanderSession.currentCommander.info;
    CommanderAppearance *commander = &commanderInfo->appearance;

    // Check if the barrack name is not empty and contains only ASCII characters
    size_t barrackNameLen = strlen(clientPacket->barrackName);

    if (barrackNameLen == 0) {
        error("Empty barrack name");
        ResultType = BC_BARRACKNAME_CHANGE_ERROR;
    }

    for (size_t i = 0; i < barrackNameLen; i++) {
         if (!isprint(clientPacket->barrackName[i])) {
            dbg("Wrong barrack name character in BC_BARRACKNAME_CHANGE");
            ResultType = BC_BARRACKNAME_CHANGE_ERROR;
         }
    }

    dbg("AccountId: %11x", session->game.accountSession.accountId);

    // Try to perform the change
    ResultType = mySqlSetFamilyName(self->sqlConn, &session->game.accountSession, clientPacket->barrackName);

    if (ResultType == BC_BARRACKNAME_CHANGE_OK) {
        // Update the session
        strncpy(commander->familyName, clientPacket->barrackName, sizeof(commander->familyName));
        strncpy(session->game.accountSession.familyName, clientPacket->barrackName, sizeof(session->game.accountSession.familyName));

    }

    // Build the reply packet
    barrackBuilderBarrackNameChange(ResultType, commander->familyName, reply);

    // Update session only if barrack name changed.
    if (ResultType == BC_BARRACKNAME_CHANGE_OK) {
        return PACKET_HANDLER_UPDATE_SESSION;
    } else {
        return PACKET_HANDLER_OK;
    }
}

static PacketHandlerState barrackHandlerCommanderDestroy(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *reply)
{
    #pragma pack(push, 1)
    struct {
        uint8_t charPosition;
    }  *clientPacket = (void *) packet;
    #pragma pack(pop)

    // For future reference, clientPacket->charPosition 0xFF removes all characters.

    CHECK_CLIENT_PACKET_SIZE(*clientPacket, packetSize, CB_COMMANDER_DESTROY);

    // Update session
    if (session->game.barrackSession.charactersCreatedCount > 0) {
        session->game.barrackSession.charactersCreatedCount -= 1;
    }

    // Build the reply packet
    barrackBuilderCommanderDestroy(clientPacket->charPosition, reply);

    return PACKET_HANDLER_UPDATE_SESSION;
}

static PacketHandlerState barrackHandlerCommanderCreate(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *reply)
{
    #pragma pack(push, 1)
    struct {
        uint8_t charPosition;
        uint8_t commanderName[COMMANDER_NAME_SIZE+1];
        uint16_t jobId;
        uint8_t gender;
        float unk4;
        float unk5;
        float unk6;
        uint8_t hairId;
    }  *clientPacket = (void *) packet;
    #pragma pack(pop)

    CHECK_CLIENT_PACKET_SIZE(*clientPacket, packetSize, CB_COMMANDER_CREATE);

    CommanderInfo *commander = &session->game.commanderSession.currentCommander.info;
    CommanderAppearance *appearance = &commander->appearance;

    // CharName
    strncpy(appearance->commanderName, clientPacket->commanderName, sizeof(appearance->commanderName));

    // AccountID
    appearance->accountId = session->socket.accountId;

    // JobID
    switch (clientPacket->jobId) {
        default:
            error("Invalid commander Job ID(%x)", clientPacket->jobId);
            return PACKET_HANDLER_ERROR;
            break;
        case COMMANDER_JOB_WARRIOR:
            appearance->classId = COMMANDER_CLASS_WARRIOR;
            break;
        case COMMANDER_JOB_ARCHER:
            appearance->classId = COMMANDER_CLASS_ARCHER;
            break;
        case COMMANDER_JOB_WIZARD:
            appearance->classId = COMMANDER_CLASS_WIZARD;
            break;
        case COMMANDER_JOB_CLERIC:
            appearance->classId = COMMANDER_CLASS_CLERIC;
            break;
    }

    appearance->jobId = clientPacket->jobId;

    // Gender
    switch (clientPacket->gender) {
        case COMMANDER_GENDER_MALE:
        case COMMANDER_GENDER_FEMALE:
            appearance->gender = clientPacket->gender;
            break;

        case COMMANDER_GENDER_BOTH:
        default:
            error("Invalid gender(%d)", clientPacket->gender);
            return PACKET_HANDLER_ERROR;
            break;
    }

    // Character position
    if (clientPacket->charPosition != session->game.barrackSession.charactersCreatedCount + 1) {
        warning("Client sent a malformed charPosition.");
    }

    // Hair type
    appearance->hairId = clientPacket->hairId;

    // PCID
    session->game.commanderSession.currentCommander.info.pcId = r1emuGenerateRandom(&self->seed);
    info("PCID generated : %x", session->game.commanderSession.currentCommander.info.pcId);

    // CommanderID
    commander->commanderId = r1emuGenerateRandom64(&self->seed);
    info("CommanderID generated : %llx", commander->commanderId);

    // SocialInfoID
    commander->socialInfoId = r1emuGenerateRandom64(&self->seed);
    info("SocialInfoID generated : %llx", commander->socialInfoId);

    // Position : Center of the barrack
    commander->pos = PositionXYZ_decl(19.0, 28.0, 29.0);

    // Default MapId : West Siauliai Woods
    session->game.commanderSession.mapId = 1002;

    // Add the character to the account
    session->game.barrackSession.charactersCreatedCount++;

    // Build the reply packet
    PositionXZ commanderDir = PositionXZ_decl(-0.707107f, 0.707107f);
    CommanderCreateInfo commanderCreate = {
        .appearance = commander->appearance,
        .mapId = session->game.commanderSession.mapId,
        .socialInfoId = commander->socialInfoId,
        .commanderPosition = session->game.barrackSession.charactersCreatedCount,
        .unk4 = SWAP_UINT32(0x02000000), // ICBT
        .unk5 = 0,
        .maxXP = 0xC, // ICBT ; TODO : Implement EXP table
        .unk6 = SWAP_UINT32(0xC01C761C), // ICBT
        .pos = commander->pos,
        .dir = commanderDir,
        .pos2 = commander->pos,
        .dir2 = commanderDir,
    };
    barrackBuilderCommanderCreate(&commanderCreate, reply);

    return PACKET_HANDLER_UPDATE_SESSION;
}

static PacketHandlerState barrackHandlerLogout(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *reply)
{
    /// TODO

    /*
    CHECK_CLIENT_PACKET_SIZE(*clientPacket, packetSize, CB_LOGOUT);
    */


    barrackBuilderLogoutOk(
        reply
    );

    return PACKET_HANDLER_UPDATE_SESSION;
}
