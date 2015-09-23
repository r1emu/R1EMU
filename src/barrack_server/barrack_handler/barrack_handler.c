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
    if (session->game.accountSession.charactersCreatedCount > 0) {
        session->game.accountSession.charactersCreatedCount -= 1;
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
    if (clientPacket->charPosition != session->game.accountSession.charactersCreatedCount + 1) {
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
    session->game.accountSession.charactersCreatedCount++;

    // Build the reply packet
    PositionXZ commanderDir = PositionXZ_decl(-0.707107f, 0.707107f);
    CommanderCreateInfo commanderCreate = {
        .appearance = commander->appearance,
        .mapId = session->game.commanderSession.mapId,
        .socialInfoId = commander->socialInfoId,
        .commanderPosition = session->game.accountSession.charactersCreatedCount,
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
