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
#include "common/actor/item/item_factory.h"
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
static PacketHandlerState barrackHandlerLogout           (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *reply);

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
    // REGISTER_PACKET_HANDLER(CB_JUMP,            barrackHandlerJump),
    REGISTER_PACKET_HANDLER(CB_START_GAME,         barrackHandlerStartGame),
    REGISTER_PACKET_HANDLER(CB_LOGOUT,             barrackHandlerLogout),

    #undef REGISTER_PACKET_HANDLER
};

static PacketHandlerState barrackHandlerLogin(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *reply)
{
    PacketHandlerState status = PACKET_HANDLER_ERROR;

    #pragma pack(push, 1)
    struct {
        uint8_t accountName[ACCOUNT_SESSION_ACCOUNT_NAME_MAXSIZE];
        uint8_t md5Password[17];
        uint8_t unk1[5]; // Game version?
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

    CHECK_CLIENT_PACKET_SIZE(*clientPacket, packetSize, CB_LOGIN);

    // Get accountSession from database
    AccountSession accountSession;
    bool goodCredentials = false;

    if (!(mySqlGetAccountData(
        self->sqlConn,
        clientPacket->accountName,
        clientPacket->md5Password,
        &accountSession,
        &goodCredentials)))
    {
        error("Cannot get SQL account data.");
        goto cleanup;
    }

    // Check if user/pass incorrect
    if (!goodCredentials) {
        barrackBuilderMessage(BC_MESSAGE_USER_PASS_INCORRECT_1, "", reply);
        status = PACKET_HANDLER_OK;
        goto cleanup;
    }

    // Check if user is banned
    if (accountSession.isBanned) {
        barrackBuilderMessage(BC_MESSAGE_ACCOUNT_BLOCKED_2, "", reply);
        status = PACKET_HANDLER_OK;
        goto cleanup;
    }

    // Check if user is already logged-in
    // TODO

    // update the session
    session->socket.authenticated = true;
    session->socket.accountId = accountSession.accountId;
    session->game.accountSession = accountSession;

    info("AccountID %llx (%s) connected !",
         session->socket.accountId, session->game.accountSession.accountName);

    barrackBuilderLoginOk(
        session->socket.accountId,
        session->game.accountSession.accountName,
        "*0FC621B82495C18DEC8D8D956C82297BEAAAA858",
        session->game.accountSession.privilege,
        reply
    );

    status = PACKET_HANDLER_UPDATE_SESSION;

cleanup:
    return status;
}

static PacketHandlerState barrackHandlerLoginByPassport(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *reply)
{
    PacketHandlerState status = PACKET_HANDLER_ERROR;

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

    // authenticate here
    // TODO

    // update the session
    // authentication OK!
    session->socket.authenticated = true;
    // ==== gives a random account ====
    session->socket.accountId = r1emuGenerateRandom64(&self->seed);

    if (!(accountSessionInit(
        &session->game.accountSession,
        session->game.accountSession.accountName,
        session->socket.sessionKey,
        ACCOUNT_SESSION_PRIVILEGES_ADMIN)))
    {
        error("Cannot initialize the account session.");
        goto cleanup;
    }

    snprintf(session->game.accountSession.accountName,
      sizeof(session->game.accountSession.accountName), "%llX", session->socket.accountId);
    info("Account %s generated !", session->game.accountSession.accountName);
    // ==================================

    barrackBuilderLoginOk(
        session->socket.accountId,
        session->game.accountSession.accountName,
        "*0FC621B82495C18DEC8D8D956C82297BEAAAA858",
        session->game.accountSession.privilege,
        reply
    );

    status = PACKET_HANDLER_UPDATE_SESSION;

cleanup:
    return status;
}

static PacketHandlerState barrackHandlerStartGame(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *reply)
{
    PacketHandlerState status = PACKET_HANDLER_ERROR;

    #pragma pack(push, 1)
    struct {
        RouterId_t routerId;
        uint8_t commanderIndex;
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

    CHECK_CLIENT_PACKET_SIZE(*clientPacket, packetSize, CB_START_GAME);

    // Check if commanderIndex exists
    if (!session->game.accountSession.commanders[clientPacket->commanderIndex - 1]) {
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
        goto cleanup;
    }

    // Retrieve zone servers ports from Redis
    // Fake ports here until we can retrieve the ports database
    int zoneServerPorts[] = {
        2004, 2005, 2006, 2007
    };

    uint32_t zoneServerIp = zoneServerIps[clientPacket->routerId];
    int zoneServerPort = zoneServerPorts[clientPacket->routerId];

    // Get the commander index from the user packet
    size_t commanderIndex = clientPacket->commanderIndex - 1;

    // Prepare "current commander"
    Session tmpSession = *session;
    CommanderSession *tmpCommanderSession = &tmpSession.game.commanderSession;
    AccountSession *tmpAccountSession = &tmpSession.game.accountSession;
    tmpCommanderSession->currentCommander = tmpAccountSession->commanders[commanderIndex];

    // Force update session in redis
    if (!(redisUpdateSession(self->redis, &tmpSession))) {
        error("Cannot update the Redis session.");
        goto cleanup;
    }

    dbg("routerId %x", tmpSession.socket.routerId);
    dbg("mapId %x", tmpSession.socket.mapId);
    dbg("accountId %llx", tmpSession.socket.accountId);
    dbg("S PcId %x", tmpCommanderSession->currentCommander->pcId);
    dbg("S socialInfoId %llx", tmpCommanderSession->currentCommander->socialInfoId);
    dbg("S commanderId %llx", tmpCommanderSession->currentCommander->commanderId);

    // Move the GameSession to the target Zone
    RedisGameSessionKey fromKey = {
        .routerId = tmpSession.socket.routerId,
        .mapId = tmpSession.socket.mapId,
        .accountId = tmpSession.socket.accountId
    };
    RedisGameSessionKey toKey = {
        .routerId = clientPacket->routerId, // target zoneId
        .mapId = -1,
        .accountId = tmpSession.socket.accountId
    };
    if (!(redisMoveGameSession(self->redis, &fromKey, &toKey))) {
        error("Cannot move the Game session %s.", tmpSession.socket.sessionKey);
        goto cleanup;
    }

    // Update the session
    session->game.commanderSession.currentCommander = tmpAccountSession->commanders[commanderIndex];

    // Build the answer packet
    barrackBuilderStartGameOk(
        self->info.routerId,
        zoneServerIp,
        zoneServerPort,
        session->game.commanderSession.currentCommander->mapId,
        clientPacket->commanderIndex,
        session->game.commanderSession.currentCommander->socialInfoId,
        false,
        reply
    );

    status = PACKET_HANDLER_UPDATE_SESSION;

cleanup:
    return status;
}

static PacketHandlerState barrackHandlerCommanderMove(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *reply)
{
    PacketHandlerState status = PACKET_HANDLER_ERROR;

    #pragma pack(push, 1)
    struct {
        uint8_t commanderIndex;
        PositionXYZ position;
        float angleDestX, angleDestY;
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

    CHECK_CLIENT_PACKET_SIZE(*clientPacket, packetSize, CB_COMMANDER_MOVE);

    size_t commanderIndex = clientPacket->commanderIndex - 1;

    Commander *commander = NULL;
    if (!accountSessionGetCommanderByIndex(&session->game.accountSession, commanderIndex, &commander)) {
        error("Cannot get commander by index.");
        goto cleanup;
    }

    // TODO : Check position of the client

    // Update session
    session->game.commanderSession.currentCommander = commander;
    memcpy(&commander->pos, &clientPacket->position, sizeof(PositionXZ));

    // Build packet
    barrackBuilderCommanderMoveOk(
        session->socket.accountId,
        clientPacket->commanderIndex,
        &commander->pos,
        reply
    );

    status = PACKET_HANDLER_UPDATE_SESSION;

cleanup:
    return status;
}

static PacketHandlerState barrackHandlerStartBarrack(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *reply)
{
    PacketHandlerState status = PACKET_HANDLER_ERROR;

    // TODO : Define CB_START_BARRACK structure
    // CHECK_CLIENT_PACKET_SIZE(*clientPacket, packetSize, CB_START_BARRACK);

    AccountSession tmpAccountSession = session->game.accountSession;
    size_t commandersCount;

    // Get list of Commanders for this AccountId
    if (!(mySqlLoadAccountCommanders(self->sqlConn, &tmpAccountSession, session->socket.accountId, &commandersCount))) {
        error("Cannot load commanders.");
        goto cleanup;
    }

    // Update session
    session->game.accountSession = tmpAccountSession;


        /// TESTING PURPOSES
    Inventory *inventory = &tmpAccountSession.commanders[0]->inventory;
    Item *items[20];
    items[0] = itemFactoryCreate(645001, 5003);
    inventoryAddItem(inventory, items[0]);
    items[1] = itemFactoryCreate(640026, 5);
    inventoryAddItem(inventory, items[1]);

    items[2] = itemFactoryCreate(531101, 1);
    inventoryAddItem(inventory, items[2]);

    items[3] = itemFactoryCreate(531152, 1);
    inventoryAddItem(inventory, items[3]);

    items[4] = itemFactoryCreate(531151, 1);
    inventoryAddItem(inventory, items[4]);

    items[5] = itemFactoryCreate(511151, 1);
    inventoryAddItem(inventory, items[5]);

    dbg("Pringing initial items (consumable and armor)");
    dbg("Inventory count: %d", inventoryGetItemsCount(inventory));
    inventoryPrintBag(inventory, ITEM_CAT_CONSUMABLE);
    inventoryPrintBag(inventory, ITEM_CAT_ARMOR);

    dbg("Equiping first item in ARMOR BAG (531101)");
    inventoryEquipItem(inventory, actorGetUId(items[2]), EQSLOT_BODY_ARMOR);
    dbg("Inventory count: %d", inventoryGetItemsCount(inventory));
    inventoryPrintBag(inventory, ITEM_CAT_ARMOR);
    dbg("Equiping first item in ARMOR BAG (531152)");
    inventoryEquipItem(inventory, actorGetUId(items[3]), EQSLOT_BODY_ARMOR);
    dbg("Inventory count: %d", inventoryGetItemsCount(inventory));
    inventoryPrintBag(inventory, ITEM_CAT_ARMOR);

    /*
    Inventory *inventory = &tmpAccountSession.commanders[0]->inventory;

    Item items[20];

    items[0].actor.uid = 1111;
    items[0].id = 2;
    items[0].amount = 1;
    items[0].category = ITEM_CAT_CONSUMABLE;
    inventoryAddItem(inventory, &items[0]);

    ItemEquipable equipItem[20];

    equipItem[0].item.actor.uid = 2222;
    equipItem[0].item.id = 531001;
    equipItem[0].item.amount = 1;
    equipItem[0].item.category = ITEM_CAT_ARMOR;
    inventoryAddItem(inventory, (Item*) &equipItem[0]);

    equipItem[1].item.actor.uid = 3333;
    equipItem[1].item.id = 666666;
    equipItem[1].item.amount = 1;
    equipItem[1].item.category = ITEM_CAT_ARMOR;
    inventoryAddItem(inventory, (Item*) &equipItem[1]);

    dbg("Inventory count: %d", inventoryGetItemsCount(inventory));
    dbg("Print inventory");
    inventoryPrintBag(inventory, ITEM_CAT_CONSUMABLE);
    inventoryPrintBag(inventory, ITEM_CAT_ARMOR);

    inventoryEquipItem(inventory, 2222, EQSLOT_BODY_ARMOR);

    inventoryPrintEquipment(inventory);
    inventoryPrintBag(inventory, ITEM_CAT_ARMOR);

    inventoryEquipItem(inventory, 3333, EQSLOT_BODY_ARMOR);

    inventoryPrintEquipment(inventory);
    inventoryPrintBag(inventory, ITEM_CAT_ARMOR);
    */

    // Send the commander list
    barrackBuilderCommanderList(
        session->socket.accountId,
        &session->game,
        session->game.accountSession.commanders,
        commandersCount,
        reply
    );

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
    PacketHandlerState status = PACKET_HANDLER_ERROR;
    BarrackNameChangeStatus changeStatus = BC_BARRACKNAME_CHANGE_ERROR;

    #pragma pack(push, 1)
    struct{
        uint8_t barrackName[64];
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

    CHECK_CLIENT_PACKET_SIZE(*clientPacket, packetSize, CB_BARRACKNAME_CHANGE);

    // Check if the barrack name is not empty and contains only ASCII characters
    size_t barrackNameLen = strlen(clientPacket->barrackName);

    if (barrackNameLen == 0) {
        error("Empty barrack name");
        goto cleanup;
    }

    for (size_t i = 0; i < barrackNameLen; i++) {
         if (!isprint(clientPacket->barrackName[i])) {
            error("Wrong barrack name character in BC_BARRACKNAME_CHANGE");
            goto cleanup;
         }
    }

    // Check if we are changing for the same name
    if (memcmp(&clientPacket->barrackName,
               &session->game.accountSession.familyName,
               sizeof(clientPacket->barrackName)) == 0) {
        // Consider we've done the changes
        changeStatus = BC_BARRACKNAME_CHANGE_OK;
        status = PACKET_HANDLER_UPDATE_SESSION;
        goto cleanup;
    }

    // Try to perform the change
    if ((changeStatus = mySqlSetFamilyName(
        self->sqlConn,
        session->game.accountSession.accountId,
        clientPacket->barrackName) != BC_BARRACKNAME_CHANGE_OK))
    {
        error("Cannot change the family name '%s' to '%s'.",
            session->game.accountSession.familyName, clientPacket->barrackName);
        goto cleanup;
    }

    // Update the session
    strncpy(session->game.accountSession.familyName,
        clientPacket->barrackName, sizeof(session->game.accountSession.familyName));

    status = PACKET_HANDLER_UPDATE_SESSION;

cleanup:
    // Build the reply packet
    barrackBuilderBarrackNameChange(changeStatus, session->game.accountSession.familyName, reply);

    if (changeStatus != BC_BARRACKNAME_CHANGE_OK) {
        // The error is displayed to the client, don't update the session though
        status = PACKET_HANDLER_OK;
    }

    return status;
}

static PacketHandlerState barrackHandlerCommanderDestroy(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *reply)
{
    PacketHandlerState status = PACKET_HANDLER_ERROR;

    #pragma pack(push, 1)
    struct {
        uint8_t commanderIndex;
    }  *clientPacket = (void *) packet;
    #pragma pack(pop)

    dbg("clientPacket->commanderIndex %d", clientPacket->commanderIndex);

    // For future reference, clientPacket->commanderIndex 0xFF removes all characters.

    CHECK_CLIENT_PACKET_SIZE(*clientPacket, packetSize, CB_COMMANDER_DESTROY);

    Commander *commanderToDelete = NULL;

    // Update session
    size_t commanderIndex = clientPacket->commanderIndex - 1;

    if (!(commanderToDelete = session->game.accountSession.commanders[commanderIndex])) {
        error("Cannot delete the commander [%d] because it doesn't exist.", commanderIndex);
        goto cleanup;
    }

    // Remove commander from MySQL
    // TODO : mark to delete instead of really deleting it
    if (!(MySqlCommanderDelete(self->sqlConn, commanderToDelete->commanderId))) {
        error("Cannot remove commander '%llx' from MySQL", commanderToDelete->commanderId);
        barrackBuilderMessage(BC_MESSAGE_CUSTOM_MSG, "There was a problem while deleting your Character. Please try again.", reply);
        status = PACKET_HANDLER_OK;
        goto cleanup;
    }

    // Update the session
    session->game.commanderSession.currentCommander = NULL;
    commanderDestroy(&session->game.accountSession.commanders[commanderIndex]);

    // Build the reply packet
    barrackBuilderCommanderDestroy(commanderIndex + 1, reply);

    status = PACKET_HANDLER_UPDATE_SESSION;

cleanup:
    return status;
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
        uint8_t commanderIndex;
        uint8_t commanderName[COMMANDER_NAME_SIZE+1];
        uint16_t jobId;
        uint8_t gender;
        float positionX;
        float positionY;
        float positionZ;
        uint8_t hairId;
    }  *clientPacket = (void *) packet;
    #pragma pack(pop)

    size_t commanderIndex = clientPacket->commanderIndex - 1;
    AccountSession *accountSession = &session->game.accountSession;

    CHECK_CLIENT_PACKET_SIZE(*clientPacket, packetSize, CB_COMMANDER_CREATE);

    if (!accountSessionIsCommanderSlotEmpty(accountSession, commanderIndex)) {
        goto cleanup;
    }

    Commander newCommander;
    commanderInit(&newCommander);
    newCommander.mapId = 1002; // FIXME : Start map could be loaded from

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
            error("Invalid commander Job ID '%x'", clientPacket->jobId);
            msgType = BC_MESSAGE_CREATE_COMMANDER_FAIL;
            goto cleanup;
            break;

        case COMMANDER_JOB_WARRIOR:
            newCommander.classId = COMMANDER_CLASS_WARRIOR;
            break;

        case COMMANDER_JOB_ARCHER:
            newCommander.classId = COMMANDER_CLASS_ARCHER;
            break;

        case COMMANDER_JOB_WIZARD:
            newCommander.classId = COMMANDER_CLASS_WIZARD;
            break;

        case COMMANDER_JOB_CLERIC:
            newCommander.classId = COMMANDER_CLASS_CLERIC;
            break;
    }

    newCommander.jobId = clientPacket->jobId;

    // Gender
    switch (clientPacket->gender) {
        case COMMANDER_GENDER_MALE:
        case COMMANDER_GENDER_FEMALE:
            newCommander.gender = clientPacket->gender;
            break;

        case COMMANDER_GENDER_BOTH:
        default:
            error("Invalid gender '%d'", clientPacket->gender);
            msgType = BC_MESSAGE_CREATE_COMMANDER_FAIL;
            goto cleanup;
            break;
    }

    // Name
    strncpy(newCommander.commanderName, clientPacket->commanderName, sizeof(newCommander.commanderName));
    strncpy(newCommander.familyName, accountSession->familyName, sizeof(newCommander.familyName));

    // AccountID
    newCommander.accountId = session->socket.accountId;

    // Hair type
    newCommander.hairId = clientPacket->hairId;

    // PCID
    // TODO : check for unicity of the generated pcId
    newCommander.pcId = r1emuGenerateRandom(&self->seed);

    // SocialInfoID
    // TODO : MySQL should generate this ID
    newCommander.socialInfoId = r1emuGenerateRandom64(&self->seed);

    // Position : Center of the barrack
    newCommander.pos = PositionXYZ_decl(19.0, 28.0, 29.0);

    // Add default equiped items
    ItemId_t defaultEquipment[][EQSLOT_COUNT] = {

        [COMMANDER_JOB_WARRIOR] = {
            EMPTYEQSLOT_NoHat, // HAT
            EMPTYEQSLOT_NoHat, // HAT_L
            EMPTYEQSLOT_NoOuter, // UNKOWN1
            531101, // BODY_ARMOR : Light Armor
            EMPTYEQSLOT_NoGloves, // GLOVES
            EMPTYEQSLOT_NoBoots,// BOOTS
            EMPTYEQSLOT_NoHelmet, // HELMET
            EMPTYEQSLOT_NoArmband, // BRACELET
            201101, // WEAPON : Old Wooden Club
            EMPTYEQSLOT_NoWeapon, // SHIELD
            EMPTYEQSLOT_NoOuter, // COSTUME
            EMPTYEQSLOT_NoRing, // UNKOWN3
            EMPTYEQSLOT_NoRing, // UNKOWN4
            EMPTYEQSLOT_NoOuter, // UNKOWN5
            521101, // LEG_ARMOR : Light Pants
            EMPTYEQSLOT_NoRing, // UNKOWN6
            EMPTYEQSLOT_NoRing, // UNKOWN7
            EMPTYEQSLOT_NoRing, // RING_LEFT
            EMPTYEQSLOT_NoRing, // RING_RIGHT
            EMPTYEQSLOT_NoNeck,// NECKLACE
        },

        [COMMANDER_JOB_CLERIC] = {
            EMPTYEQSLOT_NoHat, // HAT
            EMPTYEQSLOT_NoHat, // HAT_L
            EMPTYEQSLOT_NoOuter, // UNKOWN1
            531101, // BODY_ARMOR : Light Armor
            EMPTYEQSLOT_NoGloves, // GLOVES
            EMPTYEQSLOT_NoBoots,// BOOTS
            EMPTYEQSLOT_NoHelmet, // HELMET
            EMPTYEQSLOT_NoArmband, // BRACELET
            201101, // WEAPON : Old Wooden Club
            EMPTYEQSLOT_NoWeapon, // SHIELD
            EMPTYEQSLOT_NoOuter, // COSTUME
            EMPTYEQSLOT_NoRing, // UNKOWN3
            EMPTYEQSLOT_NoRing, // UNKOWN4
            EMPTYEQSLOT_NoOuter, // UNKOWN5
            521101, // LEG_ARMOR : Light Pants
            EMPTYEQSLOT_NoRing, // UNKOWN6
            EMPTYEQSLOT_NoRing, // UNKOWN7
            EMPTYEQSLOT_NoRing, // RING_LEFT
            EMPTYEQSLOT_NoRing, // RING_RIGHT
            EMPTYEQSLOT_NoNeck,// NECKLACE
        },

        [COMMANDER_JOB_WIZARD] = {
            EMPTYEQSLOT_NoHat, // HAT
            EMPTYEQSLOT_NoHat, // HAT_L
            EMPTYEQSLOT_NoOuter, // UNKOWN1
            531101, // BODY_ARMOR : Light Armor
            EMPTYEQSLOT_NoGloves, // GLOVES
            EMPTYEQSLOT_NoBoots,// BOOTS
            EMPTYEQSLOT_NoHelmet, // HELMET
            EMPTYEQSLOT_NoArmband, // BRACELET
            201101, // WEAPON : Old Wooden Club
            EMPTYEQSLOT_NoWeapon, // SHIELD
            EMPTYEQSLOT_NoOuter, // COSTUME
            EMPTYEQSLOT_NoRing, // UNKOWN3
            EMPTYEQSLOT_NoRing, // UNKOWN4
            EMPTYEQSLOT_NoOuter, // UNKOWN5
            521101, // LEG_ARMOR : Light Pants
            EMPTYEQSLOT_NoRing, // UNKOWN6
            EMPTYEQSLOT_NoRing, // UNKOWN7
            EMPTYEQSLOT_NoRing, // RING_LEFT
            EMPTYEQSLOT_NoRing, // RING_RIGHT
            EMPTYEQSLOT_NoNeck,// NECKLACE
        },

        [COMMANDER_JOB_ARCHER] = {
            EMPTYEQSLOT_NoHat, // HAT
            EMPTYEQSLOT_NoHat, // HAT_L
            EMPTYEQSLOT_NoOuter, // UNKOWN1
            531101, // BODY_ARMOR : Light Armor
            EMPTYEQSLOT_NoGloves, // GLOVES
            EMPTYEQSLOT_NoBoots,// BOOTS
            EMPTYEQSLOT_NoHelmet, // HELMET
            EMPTYEQSLOT_NoArmband, // BRACELET
            201101, // WEAPON : Old Wooden Club
            EMPTYEQSLOT_NoWeapon, // SHIELD
            EMPTYEQSLOT_NoOuter, // COSTUME
            EMPTYEQSLOT_NoRing, // UNKOWN3
            EMPTYEQSLOT_NoRing, // UNKOWN4
            EMPTYEQSLOT_NoOuter, // UNKOWN5
            521101, // LEG_ARMOR : Light Pants
            EMPTYEQSLOT_NoRing, // UNKOWN6
            EMPTYEQSLOT_NoRing, // UNKOWN7
            EMPTYEQSLOT_NoRing, // RING_LEFT
            EMPTYEQSLOT_NoRing, // RING_RIGHT
            EMPTYEQSLOT_NoNeck,// NECKLACE
        }
    };
    for (ItemEquipmentSlot slot = 0; slot < EQSLOT_COUNT; slot++) {
        newCommander.inventory.equippedItems[slot] = (ItemEquipable *) itemFactoryCreate(
            defaultEquipment[newCommander.jobId][slot],
            1
        );
    }

    if (!mySqlCommanderInsert(self->sqlConn, &newCommander)) {
        error("Cannot create the commander in the SQL.");
        goto cleanup;
    }

    info("New Commander Created!");
    info("PCID generated : %x", newCommander.pcId);
    info("SocialInfoID generated : %llx", newCommander.socialInfoId);
    info("accountId %llx", newCommander.accountId);

    Commander *dupCommander = NULL;
    if (!(dupCommander = commanderDup(&newCommander))) {
        error("Cannot duplicate the commander.");
        goto cleanup;
    }

    // Update the session
    accountSession->commanders[commanderIndex] = dupCommander;

    // Build the reply packet
    barrackBuilderCommanderCreate(dupCommander, commanderIndex + 1, reply);

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

    return PACKET_HANDLER_OK;
}
