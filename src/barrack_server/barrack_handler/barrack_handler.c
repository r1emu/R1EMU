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
#include "common/redis/fields/redis_session.h"
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
        uint8_t commanderIndex;
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

    CHECK_CLIENT_PACKET_SIZE(*clientPacket, packetSize, CB_START_GAME);

    dbg("clientPacket->commanderIndex %d", clientPacket->commanderIndex);
    dbg("session->game.accountSession.commandersCount %d", session->game.accountSession.commandersCount);

    // Check if commanderIndex exists
    if (clientPacket->commanderIndex > session->game.accountSession.commandersCount) {
        error("Selected commander index doesnt exist in account");
        return PACKET_HANDLER_ERROR;
    }

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

    // Prepare "current commander"
    commanderPrint(session->game.accountSession.commanders[clientPacket->commanderIndex]);

    session->game.commanderSession.currentCommander = *session->game.accountSession.commanders[clientPacket->commanderIndex-1];
    session->game.commanderSession.mapId = session->game.commanderSession.currentCommander.mapId;

    // Force update session in redis
    if (!(redisUpdateSession(self->redis, session))) {
        error("Cannot update the Redis session.");
        return PACKET_HANDLER_ERROR;
    }

    dbg("routerId %d", session->socket.routerId);
    dbg("mapId %d", session->socket.mapId);
    dbg("accountId %d", session->socket.accountId);
    dbg("S PcId %d", session->game.commanderSession.currentCommander.pcId);
    dbg("S socialInfoId %d", session->game.commanderSession.currentCommander.socialInfoId);
    dbg("S commanderId %d", session->game.commanderSession.currentCommander.commanderId);

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
        clientPacket->commanderIndex,
        session->game.commanderSession.currentCommander.socialInfoId,
        false,
        reply
    );

    // Update the session
    session->game.commanderSession.currentCommander = *session->game.accountSession.commanders[clientPacket->commanderIndex];
    session->game.commanderSession.mapId = session->game.commanderSession.currentCommander.mapId;

    return PACKET_HANDLER_UPDATE_SESSION;
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

    Commander *commander = &session->game.commanderSession.currentCommander;

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
    PacketHandlerState status = PACKET_HANDLER_ERROR;

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
    size_t commandersCount;

    if (!(mySqlRequestCommandersByAccountId(self->sqlConn, session->socket.accountId, &commandersCount))) {
        error("Cannot request commanders by accountId = %llx", session->socket.accountId);
        goto cleanup;
    }

    {
        Commander commanders[commandersCount];
        session->game.accountSession.commandersCount = commandersCount;
        if (!(mySqlGetCommanders(self->sqlConn, commanders))) {
            error("Cannot get commanders by accountId = %llx", session->socket.accountId);
            goto cleanup;
        }

        // Add commander to session.
        for (int i = 0; i < commandersCount; i++) {
            session->game.accountSession.commanders[i] = malloc(sizeof(Commander));
            memcpy(session->game.accountSession.commanders[i], &commanders[i], sizeof(Commander));
            memcpy(session->game.accountSession.commanders[i]->appearance.familyName,
                session->game.accountSession.familyName, sizeof(session->game.accountSession.familyName));
            //session->game.accountSession.commanders[i] = &commanders[i];
            commanderPrint(session->game.accountSession.commanders[i]);
        }

        // Send the commander list
        barrackBuilderCommanderList(
            session->socket.accountId,
            &session->game,
            commanders,
            commandersCount,
            reply
        );
    }

    status = PACKET_HANDLER_UPDATE_SESSION;

cleanup:
    return status;
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

    CommanderAppearance *commanderAppearance = &session->game.commanderSession.currentCommander.appearance;

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

    dbg("AccountId: %llx", session->game.accountSession.accountId);

    // Try to perform the change
    ResultType = mySqlSetFamilyName(self->sqlConn, &session->game.accountSession, clientPacket->barrackName);

    if (ResultType == BC_BARRACKNAME_CHANGE_OK) {
        // Update the session
        strncpy(commanderAppearance->familyName, clientPacket->barrackName, sizeof(commanderAppearance->familyName));
        strncpy(session->game.accountSession.familyName,
            clientPacket->barrackName, sizeof(session->game.accountSession.familyName));
    }

    // Build the reply packet
    barrackBuilderBarrackNameChange(ResultType, commanderAppearance->familyName, reply);

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

    Commander *commanderToDelete;

    // Update session
    commanderToDelete = session->game.accountSession.commanders[clientPacket->charPosition - 1];
    if (commanderToDelete) {
        /// TODO
        // Remove commander from MySQL (or mark to remove?)
        if (true) {
            // Reorder array of commanders
            for (int i = 0; i < session->game.accountSession.commandersCount - 1; i++) {
                session->game.accountSession.commanders[i] = session->game.accountSession.commanders[i + 1];
            }
            // Update the commanders count
            if (session->game.accountSession.commandersCount > 0) {
                session->game.accountSession.commandersCount -= 1;
            }
        } else {
            dbg("Problem removing commander from MySQL");
            barrackBuilderMessage(BC_MESSAGE_CUSTOM_MSG, "There was a problem while deleting your Character. Please try again.", reply);
            return PACKET_HANDLER_OK;
        }
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
    PacketHandlerState status = PACKET_HANDLER_ERROR;
    BcMessageType msgType = BC_MESSAGE_NO_MSG;

    #pragma pack(push, 1)
    struct {
        uint8_t charPosition;
        uint8_t commanderName[COMMANDER_NAME_SIZE+1];
        uint16_t jobId;
        uint8_t gender;
        float positionX;
        float positionY;
        float positionZ;
        uint8_t hairId;
    }  *clientPacket = (void *) packet;
    #pragma pack(pop)

    CHECK_CLIENT_PACKET_SIZE(*clientPacket, packetSize, CB_COMMANDER_CREATE);

    Commander newCommander;
    commanderInit(&newCommander);
    newCommander.mapId = 1002;

    CommanderAppearance *commanderAppearance = &newCommander.appearance;

    // Validate all parameters

    // Check name
    size_t commanderNameLen = strlen(clientPacket->commanderName);

    if (commanderNameLen == 0) {
        error("Empty commander name");
        msgType = BC_MESSAGE_COMMANDER_NAME_TOO_SHORT;
        goto cleanup;
    }

    for (size_t i = 0; i < commanderNameLen; i++) {
         if (!isprint(clientPacket->commanderName[i])) {
            error("Wrong commander name character in Commander");
            msgType = BC_MESSAGE_NAME_ALREADY_EXIST;
            goto cleanup;
         }
    }

    // Check valid hairId
    /// TODO

    // Check JobID
    switch (clientPacket->jobId) {

        default:
            error("Invalid commander Job ID(%x)", clientPacket->jobId);
            msgType = BC_MESSAGE_CREATE_COMMANDER_FAIL;
            goto cleanup;
            break;

        case COMMANDER_JOB_WARRIOR:
            commanderAppearance->classId = COMMANDER_CLASS_WARRIOR;
            break;

        case COMMANDER_JOB_ARCHER:
            commanderAppearance->classId = COMMANDER_CLASS_ARCHER;
            break;

        case COMMANDER_JOB_WIZARD:
            commanderAppearance->classId = COMMANDER_CLASS_WIZARD;
            break;

        case COMMANDER_JOB_CLERIC:
            commanderAppearance->classId = COMMANDER_CLASS_CLERIC;
            break;
    }

    commanderAppearance->jobId = clientPacket->jobId;

    // Gender
    switch (clientPacket->gender) {
        case COMMANDER_GENDER_MALE:
        case COMMANDER_GENDER_FEMALE:
            commanderAppearance->gender = clientPacket->gender;
            break;

        case COMMANDER_GENDER_BOTH:
        default:
            error("Invalid gender(%d)", clientPacket->gender);
            msgType = BC_MESSAGE_CREATE_COMMANDER_FAIL;
            goto cleanup;
            break;
    }

    // Character position
    if (clientPacket->charPosition != session->game.accountSession.commandersCount + 1) {
        error("Client sent a malformed charPosition.");
        msgType = BC_MESSAGE_CREATE_COMMANDER_FAIL;
        goto cleanup;
    }

    // CharName
    strncpy(commanderAppearance->commanderName, clientPacket->commanderName, sizeof(commanderAppearance->commanderName));

    // AccountID
    commanderAppearance->accountId = session->socket.accountId;

    // Hair type
    commanderAppearance->hairId = clientPacket->hairId;

    // PCID
    // TODO : check for unicity of the generated pcId
    newCommander.pcId = r1emuGenerateRandom(&self->seed);

    // SocialInfoID
    // TODO : MySQL should generate this ID
    newCommander.socialInfoId = r1emuGenerateRandom64(&self->seed);

    // Position : Center of the barrack
    newCommander.pos = PositionXYZ_decl(19.0, 28.0, 29.0);

    if (!mySqlCommanderInsert(self->sqlConn, session->socket.accountId, &newCommander)) {
        error("Cannot create the commander in the SQL.");
        goto cleanup;
    }

    info("New Commander Created!");
    info("PCID generated : %x", newCommander.pcId);
    info("SocialInfoID generated : %llx", newCommander.socialInfoId);
    info("accountId %llx", commanderAppearance->accountId);

    // Update the session
    Commander *dupCommander = commanderDup(&newCommander);
    session->game.accountSession.commanders[session->game.accountSession.commandersCount] = dupCommander;
    session->game.accountSession.commandersCount++;

    barrackBuilderCommanderCreate(dupCommander, session->game.accountSession.commandersCount, reply);

    status = PACKET_HANDLER_UPDATE_SESSION;

cleanup:
    if (msgType != BC_MESSAGE_NO_MSG) {
        // The error is handled correctly, reply back to the client but don't update the session.
        barrackBuilderMessage(msgType, "", reply);
        status = PACKET_HANDLER_OK;
    }

    return status;
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
