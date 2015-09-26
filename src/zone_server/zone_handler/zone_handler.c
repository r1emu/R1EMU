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

#include "zone_handler.h"
#include "zone_builder.h"
#include "zone_event_handler.h"
#include "admin_cmd.h"
#include "common/packet/packet.h"
#include "common/redis/fields/redis_game_session.h"
#include "common/redis/fields/redis_socket_session.h"
#include "common/server/event_handler.h"
#include "common/commander/inventory.h"
#include "common/mysql/fields/mysql_commander.h"

/** Connect to the zone server */
static PacketHandlerState zoneHandlerConnect        (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** Client is ready to enter the zone */
static PacketHandlerState zoneHandlerGameReady      (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** Jump handler */
static PacketHandlerState zoneHandlerJump           (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** On air handler */
static PacketHandlerState zoneHandlerOnAir          (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** On ground handler */
static PacketHandlerState zoneHandlerOnGround       (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** On move with keyboard handler */
static PacketHandlerState zoneHandlerKeyboardMove   (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** On stop commander movement */
static PacketHandlerState zoneHandlerMoveStop       (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** On movement information */
static PacketHandlerState zoneHandlerMovementInfo   (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** @unknown */
static PacketHandlerState zoneHandlerCampInfo       (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** On log out */
static PacketHandlerState zoneHandlerCzQuickSlotList(Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** On change config */
static PacketHandlerState zoneHandlerChangeConfig   (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** On log out */
static PacketHandlerState zoneHandlerItemUse        (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** On log out */
static PacketHandlerState zoneHandlerINeedParty     (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** On log out */
static PacketHandlerState zoneHandlerLogout         (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** On cast a spell on the ground */
static PacketHandlerState zoneHandlerSkillGround    (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** On commander sit */
static PacketHandlerState zoneHandlerRestSit        (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** On commander chat */
static PacketHandlerState zoneHandlerChat           (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** On commander chat log */
static PacketHandlerState zoneHandlerChatLog        (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** On commander head rotation */
static PacketHandlerState zoneHandlerHeadRotate     (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** On commander rotation */
static PacketHandlerState zoneHandlerRotate         (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** On commander pose */
static PacketHandlerState zoneHandlerPose           (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** On commander dash run */
static PacketHandlerState zoneHandlerDashRun        (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** On delete an item from inventory/warehouse */
static PacketHandlerState zoneHandlerItemDelete        (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** On commander delete item */
static PacketHandlerState zoneHandlerSwapEtcInvChangeIndex        (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);

/**
 * @brief zoneHandlers is a global table containing all the zone handlers.
 */
const PacketHandler zoneHandlers[PACKET_TYPE_COUNT] = {
    #define REGISTER_PACKET_HANDLER(packetName, handler) \
       [packetName] = {handler, STRINGIFY(packetName)}

    REGISTER_PACKET_HANDLER(CZ_CONNECT, zoneHandlerConnect),
    REGISTER_PACKET_HANDLER(CZ_GAME_READY, zoneHandlerGameReady),
    REGISTER_PACKET_HANDLER(CZ_JUMP, zoneHandlerJump),
    REGISTER_PACKET_HANDLER(CZ_ON_AIR, zoneHandlerOnAir),
    REGISTER_PACKET_HANDLER(CZ_ON_GROUND, zoneHandlerOnGround),
    REGISTER_PACKET_HANDLER(CZ_KEYBOARD_MOVE, zoneHandlerKeyboardMove),
    REGISTER_PACKET_HANDLER(CZ_MOVE_STOP, zoneHandlerMoveStop),
    REGISTER_PACKET_HANDLER(CZ_MOVEMENT_INFO, zoneHandlerMovementInfo),
    REGISTER_PACKET_HANDLER(CZ_CAMPINFO, zoneHandlerCampInfo),
    REGISTER_PACKET_HANDLER(CZ_QUICKSLOT_LIST, zoneHandlerCzQuickSlotList),
    REGISTER_PACKET_HANDLER(CZ_CHANGE_CONFIG, zoneHandlerChangeConfig),
    REGISTER_PACKET_HANDLER(CZ_LOGOUT, zoneHandlerLogout),
    REGISTER_PACKET_HANDLER(CZ_ITEM_USE, zoneHandlerItemUse),
    REGISTER_PACKET_HANDLER(CZ_I_NEED_PARTY, zoneHandlerINeedParty),
    REGISTER_PACKET_HANDLER(CZ_SKILL_GROUND, zoneHandlerSkillGround),
    REGISTER_PACKET_HANDLER(CZ_REST_SIT, zoneHandlerRestSit),
    REGISTER_PACKET_HANDLER(CZ_CHAT, zoneHandlerChat),
    REGISTER_PACKET_HANDLER(CZ_CHAT_LOG, zoneHandlerChatLog),
    REGISTER_PACKET_HANDLER(CZ_HEAD_ROTATE, zoneHandlerHeadRotate),
    REGISTER_PACKET_HANDLER(CZ_ROTATE, zoneHandlerRotate),
    REGISTER_PACKET_HANDLER(CZ_POSE, zoneHandlerPose),
    REGISTER_PACKET_HANDLER(CZ_DASHRUN, zoneHandlerDashRun),
    REGISTER_PACKET_HANDLER(CZ_ITEM_DELETE, zoneHandlerItemDelete),
    REGISTER_PACKET_HANDLER(CZ_SWAP_ETC_INV_CHANGE_INDEX, zoneHandlerSwapEtcInvChangeIndex),

    #undef REGISTER_PACKET_HANDLER
};

static PacketHandlerState zoneHandlerChatLog(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg)
{
    warning ("Not implemented yet.");
    return PACKET_HANDLER_OK;
}

static PacketHandlerState zoneHandlerChat(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg)
{
    // the first 2 bytes of ZC_CHAT are the packet size
    size_t chatTextSize = *((uint16_t *) packet) - sizeof(ClientPacketHeader) - sizeof(uint16_t);

    #pragma pack(push, 1)
    struct {
        uint16_t msgSize;
        uint8_t msgText[chatTextSize];
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

    CHECK_CLIENT_PACKET_SIZE(*clientPacket, packetSize, CZ_CHAT);

    // The chat message sent by the client should always finish by a null byte
    clientPacket->msgText[chatTextSize-1] = '\0';

    // Check for custom admin commands
    if (session->game.accountSession.privilege <= ACCOUNT_SESSION_PRIVILEGES_ADMIN
    && (strncmp(clientPacket->msgText, "/", strlen("/")) == 0)) {
        adminCmdProcess(self, clientPacket->msgText + strlen("/"), session, replyMsg);
    }
    else {
        // normal message : Dispatch a GameEventChat
        size_t gameEventSize = sizeof(GameEventChat) + chatTextSize;
        GameEventChat *event = alloca(gameEventSize);
        memcpy(&event->commander, &session->game.commanderSession.currentCommander, sizeof(event->commander));
        memcpy(event->chatText, clientPacket->msgText, chatTextSize);
        workerDispatchEvent(self, session->socket.sessionKey, EVENT_TYPE_CHAT, event, gameEventSize);
    }

    return PACKET_HANDLER_UPDATE_SESSION;
}

static PacketHandlerState zoneHandlerRestSit(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg)
{
    #pragma pack(push, 1)
    struct {
        // This packet is actually empty
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

    CHECK_CLIENT_PACKET_SIZE(*clientPacket, packetSize, CZ_REST_SIT);

    // notify the players around
    GameEventRestSit event = {
        .pcId = session->game.commanderSession.currentCommander->pcId,
    };
    workerDispatchEvent(self, session->socket.sessionKey, EVENT_TYPE_REST_SIT, &event, sizeof(event));

    return PACKET_HANDLER_OK;
}

static PacketHandlerState zoneHandlerSkillGround(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg)
{
    #pragma pack(push, 1)
    struct {
        uint8_t unk1;
        uint32_t skillId;
        uint32_t unk2;
        PositionXYZ pos1;
        PositionXYZ pos2;
        float unk3;
        uint32_t unk4;
        uint32_t unk5;
        uint8_t unk6;
        uint8_t unk7;
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

    CHECK_CLIENT_PACKET_SIZE(*clientPacket, packetSize, CZ_SKILL_GROUND);

    /*   CzSkillGroundPacket :
         u1 skillId  unk2     x        y        z        x2       y2       z2       u3       u4       u5       u6 u7
         00 419C0000 00000000 DAFD24C4 74768243 1B77F1C3 DAFD24C4 74768243 1B77F1C3 FFFF7FBF 0020B539 00000000 00 01
         00 419C0000 00000000 720344C4 74768243 2178F6C3 720344C4 74768243 2178F6C3 F30435BF F40435BF 00000000 00 01
         00 419C0000 00000000 C9EC91C4 74768243 17060AC4 C9EC91C4 74768243 17060AC4 FFFF7F3F 0020B5B9 00000000 00 00
         00 429C0000 00000000 5A00FAC3 1F7CA143 B1D3E843 5A00FAC3 1F7CA143 B1D3E843 EE04353F F60435BF 00000000 00 00
         00 439C0000 00000000 1E43FFC3 1F7CA143 E130D443 1E43FFC3 1F7CA143 E130D443 EF04353F F80435BF 00000000 00 00
    */

    zoneBuilderPlayAni(replyMsg);

    zoneBuilderNormalUnk8(
        session->game.commanderSession.currentCommander->pcId,
        replyMsg
    );

    return PACKET_HANDLER_OK;
}

static PacketHandlerState zoneHandlerCampInfo(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg)
{
    #pragma pack(push, 1)
    struct {
        uint64_t accountId;
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

    CHECK_CLIENT_PACKET_SIZE(*clientPacket, packetSize, CZ_CAMPINFO);

    zoneBuilderCampInfo(session->socket.accountId, replyMsg);

    return PACKET_HANDLER_OK;
}

static PacketHandlerState zoneHandlerItemUse(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg)
{
    warning("CZ_ITEM_USE not implemented yet.");
    return PACKET_HANDLER_OK;
}

static PacketHandlerState zoneHandlerINeedParty(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg)
{
    warning("CZ_I_NEED_PARTY not implemented yet.");
    // ZoneBuilder_partyInfo(replyMsg);
    // ZoneBuilder_partyList(replyMsg);

    return PACKET_HANDLER_OK;
}

static PacketHandlerState zoneHandlerLogout(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg)
{
    #pragma pack(push, 1)
    struct {
        uint8_t unk1;
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

    CHECK_CLIENT_PACKET_SIZE(*clientPacket, packetSize, CZ_LOGOUT);
    zoneBuilderLogout(replyMsg);

    // notify leave to clients
    GameEventLeave event = {
        .pcId = session->game.commanderSession.currentCommander->pcId,
    };
    workerDispatchEvent(self, session->socket.sessionKey, EVENT_TYPE_LEAVE, &event, sizeof(event));

    return PACKET_HANDLER_OK;
}

static PacketHandlerState zoneHandlerMovementInfo(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg) {
    warning("CZ_MOVEMENT_INFO not implemented yet.");
    return PACKET_HANDLER_OK;
}

static PacketHandlerState zoneHandlerMoveStop(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg)
{
    #pragma pack(push, 1)
    struct {
        uint8_t unk1;
        PositionXYZ position;
        PositionXZ direction;
        float timestamp;
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

    CHECK_CLIENT_PACKET_SIZE(*clientPacket, packetSize, CZ_MOVE_STOP);

    /*  u1 posX     posY     posZ     dirX     dirZ     time
        00 36E203C4 74768243 826F39C4 FFFFFF3F 00000000 D0651543
        00 22F402C4 74768243 826F39C4 FFFFFF3F 00000000 60941543
        00 22F402C4 74768243 4EFC3AC4 00000000 FFFF7FBF 7EF21543
        00 22F402C4 74768243 BE393CC4 00000000 FFFF7FBF B4321643
        00 22F402C4 74768243 8AC63DC4 00000000 FFFF7FBF 526A1643
        00 22F402C4 74768243 62EA3BC4 00000000 FFFF7F3F 51B71643
        00 22F402C4 74768243 F2AC3AC4 00000000 FFFF7F3F 5DE61643
        00 22F402C4 74768243 826F39C4 00000000 FFFF7F3F 74151743
        00 22F402C4 74768243 123238C4 00000000 FFFF7F3F 42401743
        00 22F402C4 74768243 FE4337C4 00000000 FFFF7F3F 0A6B1743
        00 22F402C4 74768243 8E0636C4 00000000 FFFF7F3F D3951743
    */

    // Notify the players around
    GameEventMoveStop event = {
        .updatePosEvent = {
            .mapId = session->socket.mapId,
            .commander = *session->game.commanderSession.currentCommander,
        },
        .position = clientPacket->position,
        .direction = clientPacket->direction,
        .timestamp = clientPacket->timestamp,
    };

    workerDispatchEvent(self, session->socket.sessionKey, EVENT_TYPE_MOVE_STOP, &event, sizeof(event));

    return PACKET_HANDLER_OK;
}

static PacketHandlerState zoneHandlerKeyboardMove(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg)
{
    #pragma pack(push, 1)
    struct {
        uint8_t unk1;
        PositionXYZ position;
        PositionXZ direction;
        uint8_t unk7;
        float movementSpeed;
        uint8_t unk8;
        float timestamp;
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

    /*  u1 posX     posY     posZ     dirX     dirZ     u7 mspeed   u8 time
        00 18190DC4 B663A143 17520644 00000000 FFFF7FBF 01 0000F841 01 7B2C5C44
        00 D4CE1AC4 74768243 4CEF7EC4 F30435BF F204353F 01 00000000 00 5E16FA42
        00 103F1BC4 74768243 107F7EC4 F204353F F304353F 01 00000000 00 3141FA42
        00 5CEE19C4 74768243 5C2E7DC4 1D566733 FFFF7F3F 01 00000000 00 8174FA42
        00 8C5112C4 74768243 78A374C4 FFFF7F3F 00000000 01 00000000 00 727CC244
    */
    CHECK_CLIENT_PACKET_SIZE(*clientPacket, packetSize, CZ_KEYBOARD_MOVE);

    // TODO : Check coordinates

    // update session
    session->game.commanderSession.currentCommander->pos = clientPacket->position;

    // notify the players around
    GameEventCommanderMove event = {
        .updatePosEvent = {
            .mapId = session->socket.mapId,
            .commander = *session->game.commanderSession.currentCommander
        },
        .position = clientPacket->position,
        .direction = clientPacket->direction,
        .timestamp = clientPacket->timestamp,
    };

    workerDispatchEvent(self, session->socket.sessionKey, EVENT_TYPE_COMMANDER_MOVE, &event, sizeof(event));

    return PACKET_HANDLER_UPDATE_SESSION;
}

static PacketHandlerState zoneHandlerCzQuickSlotList(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg)
{
    warning("CZ_QUICKSLOT_LIST not implemented yet.");
    // Answer PacketType : ZC_QUICKSLOT_REGISTER
    return PACKET_HANDLER_OK;
}

static PacketHandlerState zoneHandlerChangeConfig(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg)
{
    warning("CZ_CHANGE_CONFIG not implemented yet.");

    return PACKET_HANDLER_OK;
}

static PacketHandlerState zoneHandlerGameReady(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg)
{
    // CHECK_CLIENT_PACKET_SIZE(*clientPacket, packetSize, CZ_GAME_READY);
    Commander *commander = session->game.commanderSession.currentCommander;

    /// TESTING PURPOSES
    /*
    Inventory *inventory = &session->game.commanderSession.currentCommander->inventory;

    Item items[20];
    items[0].itemId = 1111;
    items[0].itemType = 645001;
    items[0].amount = 5003;
    items[0].itemCategory = INVENTORY_CAT_CONSUMABLE;
    items[0].inventoryIndex = 1;
    //items[0].attributes = itemAttributesNew(0, 0, NULL, NULL, NULL, 0, 0);
    inventoryAddItem(inventory, &items[0]);

    items[1].itemId = 2222;
    items[1].itemType = 640026;
    items[1].amount = 5;
    items[1].itemCategory = INVENTORY_CAT_CONSUMABLE;
    items[1].inventoryIndex = 2;
    //items[1].attributes = itemAttributesNew(0, 0, NULL, NULL, NULL, 0, 0);
    inventoryAddItem(inventory, &items[1]);


    items[2].itemId = 3333;
    items[2].itemType = 531102;
    items[2].amount = 1;
    items[2].itemCategory = INVENTORY_CAT_ARMOR;
    //items[2].equipSlot = EQSLOT_BODY_ARMOR;
    //items[2].attributes = itemAttributesNew(4200, 0, NULL, NULL, NULL, 0, 0);
    inventoryAddItem(inventory, &items[2]);

    items[3].itemId = 4444;
    items[3].itemType = 531101;
    items[3].amount = 1;
    items[3].itemCategory = INVENTORY_CAT_ARMOR;
    //items[3].equipSlot = EQSLOT_BODY_ARMOR;
    //items[3].attributes = itemAttributesNew(4200, 0, NULL, NULL, NULL, 0, 0);
    inventoryAddItem(inventory, &items[3]);


    inventoryPrintBag(inventory, INVENTORY_CAT_CONSUMABLE);
    inventoryPrintBag(inventory, INVENTORY_CAT_ARMOR);

    inventoryRemoveItem(inventory, &items[1]);

    inventoryPrintBag(inventory, INVENTORY_CAT_CONSUMABLE);
    */
    zoneBuilderItemInventoryList(&session->game.commanderSession.currentCommander->inventory, replyMsg);
    zoneBuilderSessionObjects(replyMsg);
    zoneBuilderOptionList(replyMsg);
    zoneBuilderSkillmapList(replyMsg);
    zoneBuilderAchievePointList(replyMsg);
    zoneBuilderChatMacroList(replyMsg);
    zoneBuilderUiInfoList(replyMsg);
    zoneBuilderNpcStateList(replyMsg);

    zoneBuilderHelpList(replyMsg);
    zoneBuilderMyPageMap(replyMsg);
    zoneBuilderGuestPageMap(replyMsg);
    zoneBuilderStartInfo(replyMsg);

    /// FOR TESTIG PURPOSES
    /*
    Inventory *inventory = &session->game.commanderSession.currentCommander->inventory;
    Item items[20];

    items[0].itemId = 1111;
    items[0].itemType = 2;
    items[0].amount = 1;
    items[0].equipSlot = EQSLOT_HEAD_TOP;
    items[0].attributes = itemAttributesNew(0, 0, NULL, NULL, NULL, 0, 0);
    inventoryAddItem(inventory, &items[0]);
    inventoryEquipItem(inventory, items[0].itemId, EQSLOT_HEAD_TOP);


    items[1].itemId = 2222;
    items[1].itemType = 2;
    items[1].amount = 1;
    items[1].equipSlot = EQSLOT_HEAD_MIDDLE;
    items[1].attributes = itemAttributesNew(0, 0, NULL, NULL, NULL, 0, 0);
    inventoryAddItem(inventory, &items[1]);
    inventoryEquipItem(inventory, items[1].itemId, EQSLOT_HEAD_MIDDLE);

    items[2].itemId = 3333;
    items[2].itemType = 4;
    items[2].amount = 1;
    items[2].equipSlot = EQSLOT_UNKOWN1;
    items[2].attributes = itemAttributesNew(0, 0, NULL, NULL, NULL, 0, 0);
    inventoryAddItem(inventory, &items[2]);
    inventoryEquipItem(inventory, items[2].itemId, EQSLOT_UNKOWN1);

    items[3].itemId = 4444;
    items[3].itemType = 531101;
    items[3].amount = 1;
    items[3].equipSlot = EQSLOT_BODY_ARMOR;
    items[3].attributes = itemAttributesNew(4200, 0, NULL, NULL, NULL, 0, 0);
    inventoryAddItem(inventory, &items[3]);
    inventoryEquipItem(inventory, items[3].itemId, EQSLOT_BODY_ARMOR);

    items[4].itemId = 5555;
    items[4].itemType = 6;
    items[4].amount = 1;
    items[4].equipSlot = EQSLOT_GLOVES;
    items[4].attributes = itemAttributesNew(0, 0, NULL, NULL, NULL, 0, 0);
    inventoryAddItem(inventory, &items[4]);
    inventoryEquipItem(inventory, items[4].itemId, EQSLOT_GLOVES);

    items[5].itemId = 6666;
    items[5].itemType = 7;
    items[5].amount = 1;
    items[5].equipSlot = EQSLOT_BOOTS;
    items[5].attributes = itemAttributesNew(0, 0, NULL, NULL, NULL, 0, 0);
    inventoryAddItem(inventory, &items[5]);
    inventoryEquipItem(inventory, items[5].itemId, EQSLOT_BOOTS);

    items[6].itemId = 7777;
    items[6].itemType = 10000;
    items[6].amount = 1;
    items[6].equipSlot = EQSLOT_HELMET;
    items[6].attributes = itemAttributesNew(0, 0, NULL, NULL, NULL, 0, 0);
    inventoryAddItem(inventory, &items[6]);
    inventoryEquipItem(inventory, items[6].itemId, EQSLOT_HELMET);

    items[7].itemId = 8888;
    items[7].itemType = 11000;
    items[7].amount = 1;
    items[7].equipSlot = EQSLOT_BRACELET;
    items[7].attributes = itemAttributesNew(0, 0, NULL, NULL, NULL, 0, 0);
    inventoryAddItem(inventory, &items[7]);
    inventoryEquipItem(inventory, items[7].itemId, EQSLOT_BRACELET);

    items[8].itemId = 9999;
    items[8].itemType = 9999996;
    items[8].amount = 1;
    items[8].equipSlot = EQSLOT_WEAPON;
    items[8].attributes = itemAttributesNew(0, 0, NULL, NULL, NULL, 0, 0);
    inventoryAddItem(inventory, &items[8]);
    inventoryEquipItem(inventory, items[8].itemId, EQSLOT_WEAPON);

    items[9].itemId = 101010;
    items[9].itemType = 9999996;
    items[9].amount = 1;
    items[9].equipSlot = EQSLOT_SHIELD;
    items[9].attributes = itemAttributesNew(0, 0, NULL, NULL, NULL, 0, 0);
    inventoryAddItem(inventory,  &items[9]);
    inventoryEquipItem(inventory, items[9].itemId, EQSLOT_SHIELD);

    items[10].itemId = 101011;
    items[10].itemType = 4;
    items[10].amount = 1;
    items[10].equipSlot = EQSLOT_COSTUME;
    items[10].attributes = itemAttributesNew(0, 0, NULL, NULL, NULL, 0, 0);
    inventoryAddItem(inventory,  &items[10]);
    inventoryEquipItem(inventory, items[10].itemId, EQSLOT_COSTUME);

    items[11].itemId = 111111;
    items[11].itemType = 9;
    items[11].amount = 1;
    items[11].equipSlot = EQSLOT_UNKOWN3;
    items[11].attributes = itemAttributesNew(0, 0, NULL, NULL, NULL, 0, 0);
    inventoryAddItem(inventory,  &items[11]);
    inventoryEquipItem(inventory, items[11].itemId, EQSLOT_UNKOWN3);

    items[12].itemId = 121212;
    items[12].itemType = 9;
    items[12].amount = 1;
    items[12].equipSlot = EQSLOT_UNKOWN4;
    items[12].attributes = itemAttributesNew(0, 0, NULL, NULL, NULL, 0, 0);
    inventoryAddItem(inventory,  &items[12]);
    inventoryEquipItem(inventory, items[12].itemId, EQSLOT_UNKOWN4);

    items[13].itemId = 131313;
    items[13].itemType = 4;
    items[13].amount = 1;
    items[13].equipSlot = EQSLOT_UNKOWN5;
    items[13].attributes = itemAttributesNew(0, 0, NULL, NULL, NULL, 0, 0);
    inventoryAddItem(inventory,  &items[13]);
    inventoryEquipItem(inventory, items[13].itemId, EQSLOT_UNKOWN5);

    items[14].itemId = 141414;
    items[14].itemType = 8;
    items[14].amount = 1;
    items[14].equipSlot = EQSLOT_LEG_ARMOR;
    items[14].attributes = itemAttributesNew(0, 0, NULL, NULL, NULL, 0, 0);
    inventoryAddItem(inventory,  &items[14]);
    inventoryEquipItem(inventory, items[14].itemId, EQSLOT_LEG_ARMOR);

    items[15].itemId = 161616;
    items[15].itemType = 9;
    items[15].amount = 1;
    items[15].equipSlot = EQSLOT_UNKOWN6;
    items[15].attributes = itemAttributesNew(0, 0, NULL, NULL, NULL, 0, 0);
    inventoryAddItem(inventory,  &items[15]);
    inventoryEquipItem(inventory, items[15].itemId, EQSLOT_UNKOWN6);

    items[16].itemId = 171717;
    items[16].itemType = 9;
    items[16].amount = 1;
    items[16].equipSlot = EQSLOT_UNKOWN7;
    items[16].attributes = itemAttributesNew(0, 0, NULL, NULL, NULL, 0, 0);
    inventoryAddItem(inventory,  &items[16]);
    inventoryEquipItem(inventory, items[16].itemId, EQSLOT_UNKOWN7);

    items[17].itemId = 181818;
    items[17].itemType = 9;
    items[17].amount = 1;
    items[17].equipSlot = EQSLOT_RIGHT_LEFT;
    items[17].attributes = itemAttributesNew(0, 0, NULL, NULL, NULL, 0, 0);
    inventoryAddItem(inventory,  &items[17]);
    inventoryEquipItem(inventory, items[17].itemId, EQSLOT_RIGHT_LEFT);

    items[18].itemId = 191919;
    items[18].itemType = 9;
    items[18].amount = 1;
    items[18].equipSlot = EQSLOT_RIGHT_RIGHT;
    items[18].attributes = itemAttributesNew(0, 0, NULL, NULL, NULL, 0, 0);
    inventoryAddItem(inventory,  &items[18]);
    inventoryEquipItem(inventory, items[18].itemId, EQSLOT_RIGHT_RIGHT);

    items[19].itemId = 202020;
    items[19].itemType = 10;
    items[19].amount = 1;
    items[19].equipSlot = EQSLOT_NECKLACE;
    items[19].attributes = itemAttributesNew(0, 0, NULL, NULL, NULL, 0, 0);
    inventoryAddItem(inventory,  &items[19]);
    inventoryEquipItem(inventory, items[19].itemId, EQSLOT_NECKLACE);

    inventoryPrintEquipment(inventory);
    */




    zoneBuilderItemEquipList(&session->game.commanderSession.currentCommander->inventory, replyMsg);
    // ZoneBuilder_skillList(commander->pcId, replyMsg);
    zoneBuilderAbilityList(commander->pcId, replyMsg);
    zoneBuilderCooldownList(commander->socialInfoId, replyMsg);

    zoneBuilderQuickSlotList(replyMsg);

    zoneBuilderNormalUnk1(replyMsg);
    zoneBuilderNormalUnk2(replyMsg);
    zoneBuilderNormalUnk3(replyMsg);
    zoneBuilderNormalUnk4(commander->socialInfoId, replyMsg);
    zoneBuilderNormalUnk5(replyMsg);

    zoneBuilderStartGame(1.0, 0.0, 0.0, 0.0, replyMsg);
    /*
    ZoneBuilder_objectProperty(replyMsg);
    ZoneBuilder_stamina(replyMsg);
    */
    zoneBuilderLoginTime(replyMsg);

    zoneBuilderMyPCEnter(&commander->pos, replyMsg);
    // ZoneBuilder_skillAdd(replyMsg);

    // Notify players around that a new PC has entered
    GameEventEnterPc pcEnterEvent = {
        .updatePosEvent = {
            .mapId = session->socket.mapId,
            .commander = *commander
        }
    };
    workerDispatchEvent(self, session->socket.sessionKey, EVENT_TYPE_ENTER_PC, &pcEnterEvent, sizeof(pcEnterEvent));
    // zoneBuilderEnterPc(&pcEnterEvent.updatePosEvent.info, replyMsg);

    // ZoneBuilder_buffList(commander->appearance.pcId, replyMsg);

    // add NPC at the start screen
    // ZoneBuilder_enterMonster(replyMsg);
    // ZoneBuilder_faction(replyMsg);

    /*
    ZoneBuilder_normalUnk6(
        commander->appearance.commanderName,
        replyMsg
    );

    ZoneBuilder_normalUnk7(
        session->socket.accountId,
        session->game.commanderSession.currentCommander->appearance.pcId,
        session->game.commanderSession.currentCommander->appearance.familyName,
        session->game.commanderSession.currentCommander->appearance.commanderName,
        replyMsg
    );

    ZoneBuilder_jobPts(replyMsg);
    ZoneBuilder_normalUnk9(session->game.commanderSession.currentCommander->appearance.pcId, replyMsg);
    ZoneBuilder_addonMsg(replyMsg);
    */

    zoneBuilderMoveSpeed(session->game.commanderSession.currentCommander->pcId, 31.0f, replyMsg);

    return PACKET_HANDLER_UPDATE_SESSION;
}

static PacketHandlerState zoneHandlerConnect(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg)
{
    PacketHandlerState status = PACKET_HANDLER_ERROR;

    #pragma pack(push, 1)
    struct {
        uint32_t unk1;
        uint64_t accountId;
        uint64_t zoneServerId;
        uint8_t login[ACCOUNT_SESSION_LOGIN_MAXSIZE];
        uint8_t unk4;
        uint32_t zoneServerIndex;
        uint16_t unk3;
        uint8_t channelListId;
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

    AccountSession *accountSession = &session->game.accountSession;

    dbg("zoneHandlerConnect");
    dbg("unk1 %x", clientPacket->unk1);
    dbg("accountId %d", clientPacket->accountId);
    dbg("zoneServerId %d", clientPacket->zoneServerId);
    dbg("zoneServerIndex %d", clientPacket->zoneServerIndex);
    dbg("unk3 %x", clientPacket->unk3);
    dbg("unk4 %x", clientPacket->unk4);
    dbg("channelListId %d", clientPacket->channelListId);
    dbg("login %s", clientPacket->login);

    // Load data from SQL
    if (!(accountSessionCommandersInit(accountSession))) {
        error("Cannot initialize commanders in session.");
        goto cleanup;
    }
    // Get list of Commanders for this AccountId
    size_t commandersCount;
    if (!(mySqlRequestCommandersByAccountId(self->sqlConn, clientPacket->accountId, &commandersCount))) {
        error("Cannot request commanders by accountId = %llx", clientPacket->accountId);
        goto cleanup;
    }

    {
        Commander commanders[commandersCount];

        // Get commanders from SQL
        if (!(mySqlGetCommanders(self->sqlConn, commanders))) {
            error("Cannot get commanders by accountId = %llx", clientPacket->accountId);
            goto cleanup;
        }

        // Add commanders to session.
        for (int i = 0; i < commandersCount; i++) {
            if (!(session->game.accountSession.commanders[i] = commanderNew())) {
                error("Cannot allocate a new commander.");
                goto cleanup;
            }
        }

        // Update the session
        for (int i = 0; i < commandersCount; i++) {
            memcpy(session->game.accountSession.commanders[i], &commanders[i], sizeof(Commander));
            /* memcpy(session->game.accountSession.commanders[i]->appearance.familyName,
                session->game.accountSession.familyName, sizeof(session->game.accountSession.familyName)); */
        }

        session->game.accountSession.commandersCount = commandersCount;
        // FIXME : Determine how to get the correct commander in the commander array
        session->game.commanderSession.currentCommander = session->game.accountSession.commanders[0];
    }


    // TODO : Reverse CZ_CONNECT correctly
    // CHECK_CLIENT_PACKET_SIZE(*clientPacket, packetSize, CZ_CONNECT);

    // Get the Game Session that the Barrack Server moved
    RedisGameSessionKey gameKey = {
        .routerId = self->info.routerId,
        .mapId = -1,
        .accountId = clientPacket->accountId
    };
    if (!(redisGetGameSession(self->redis, &gameKey, &session->game))) {
        error("Cannot retrieve the game session.");
        goto cleanup;
    }

    // Check the client packet here(authentication)
    // TODO improve it
    if (strncmp(session->game.accountSession.login,
                clientPacket->login,
                sizeof(session->game.accountSession.login)) != 0)
    {
        error("Wrong account authentication.(clientPacket account = <%s>, Session account = <%s>",
            clientPacket->login, session->game.accountSession.login);
        goto cleanup;
    }

    // Authentication OK !
    // Update the Socket Session
    socketSessionInit(&session->socket,
        clientPacket->accountId,
        self->info.routerId,
        session->game.commanderSession.currentCommander->mapId,
        session->socket.sessionKey,
        true
    );

    // Move the game Session to the current mapId
    RedisGameSessionKey fromKey = {
        .routerId = session->socket.routerId,
        .mapId = -1,
        .accountId = session->socket.accountId
    };
    RedisGameSessionKey toKey = {
        .routerId = session->socket.routerId,
        .mapId = session->socket.mapId,
        .accountId = session->socket.accountId
    };

    if (!(redisMoveGameSession(self->redis, &fromKey, &toKey))) {
        error("Cannot move the game session to the current mapId.");
        goto cleanup;
    }

    // update the session
    session->game.commanderSession.currentCommander->pos = PositionXYZ_decl(76.0f, 1.0f, 57.0f);

    zoneBuilderConnectOk(
        0, // GameMode
        0, // accountPrivileges
        session->game.commanderSession.currentCommander,
        replyMsg
    );

    status = PACKET_HANDLER_UPDATE_SESSION;

cleanup:
    return status;
}

static PacketHandlerState zoneHandlerJump(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg)
{
    #pragma pack(push, 1)
    struct {
        uint8_t unk1;
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

    CHECK_CLIENT_PACKET_SIZE(*clientPacket, packetSize, CZ_JUMP);

    // Notify the players around
    GameEventJump event = {
        .updatePosEvent = {
            .mapId = session->socket.mapId,
            .commander = *session->game.commanderSession.currentCommander
        },
        .height = COMMANDER_HEIGHT_JUMP
    };
    workerDispatchEvent(self, session->socket.sessionKey, EVENT_TYPE_JUMP, &event, sizeof(event));

    return PACKET_HANDLER_OK;
}

static PacketHandlerState zoneHandlerOnAir(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg)
{
    warning("CZ_ON_AIR not implemented yet.");
    return PACKET_HANDLER_OK;
}

static PacketHandlerState zoneHandlerOnGround(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg) {
    warning("CZ_ON_GROUND not implemented yet.");
    return PACKET_HANDLER_OK;
}

static PacketHandlerState zoneHandlerHeadRotate(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg)
{
    #pragma pack(push, 1)
    struct {
        PositionXZ direction;
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

    CHECK_CLIENT_PACKET_SIZE(*clientPacket, packetSize, CZ_HEAD_ROTATE);

    // notify the players around
    GameEventHeadRotate event = {
        .pcId = session->game.commanderSession.currentCommander->pcId,
        .direction = clientPacket->direction
    };

    workerDispatchEvent(self, session->socket.sessionKey, EVENT_TYPE_HEAD_ROTATE, &event, sizeof(event));

    return PACKET_HANDLER_OK;
}

static PacketHandlerState zoneHandlerRotate(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg)
{
    #pragma pack(push, 1)
    struct {
        PositionXZ direction;
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

    CHECK_CLIENT_PACKET_SIZE(*clientPacket, packetSize, CZ_ROTATE);

    // notify the players around
    GameEventRotate event = {
        .pcId = session->game.commanderSession.currentCommander->pcId,
        .direction = clientPacket->direction
    };

    workerDispatchEvent(self, session->socket.sessionKey, EVENT_TYPE_ROTATE, &event, sizeof(event));

    return PACKET_HANDLER_OK;
}

static PacketHandlerState zoneHandlerPose(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg)
{
    #pragma pack(push, 1)
    struct {
        uint32_t poseId;
        PositionXYZ position;
        PositionXZ direction;
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

    CHECK_CLIENT_PACKET_SIZE(*clientPacket, packetSize, CZ_POSE);

    // notify the players around
    GameEventPose event = {
        .pcId = session->game.commanderSession.currentCommander->pcId,
        .poseId = clientPacket->poseId,
        .position = session->game.commanderSession.currentCommander->pos,
        .direction = clientPacket->direction
    };

    workerDispatchEvent(self, session->socket.sessionKey, EVENT_TYPE_POSE, &event, sizeof(event));

    return PACKET_HANDLER_OK;
}

static PacketHandlerState zoneHandlerDashRun(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg)
{
    /*#pragma pack(push, 1)
    struct {
        uint8_t unk1;
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

    CHECK_CLIENT_PACKET_SIZE(*clientPacket, packetSize, CZ_DASHRUN);
    zoneBuilderMoveSpeed(session->game.commanderSession.currentCommander->pcId, 38.0f, replyMsg);*/

    warning("CZ_DASHRUN not implemented yet.");

    return PACKET_HANDLER_OK;
}

static PacketHandlerState zoneHandlerItemDelete(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg)
{
    #pragma pack(push, 1)
    struct {
        uint16_t unk1;
        uint32_t unk2;
        uint64_t itemId;
        uint64_t unk3;
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

    CHECK_CLIENT_PACKET_SIZE(*clientPacket, packetSize, CZ_ITEM_DELETE);

    // Delete item from inventory
    Inventory *inventory = &session->game.commanderSession.currentCommander->inventory;

    Item *item;
    if (inventoryGetItemByItemId(inventory, clientPacket->itemId, &item)) {
        inventoryRemoveItem(&session->game.commanderSession.currentCommander->inventory, item);
    } else {
        error("Item not found in inventory");
        return PACKET_HANDLER_ERROR;
    }

    ///TODO
    uint8_t removalType = 3; // Destroyed
    uint8_t inventoryType = 0; // 0 (Inventory) , 1 (warehouse)

    zoneBuilderItemRemove(item, removalType, inventoryType,replyMsg);

    return PACKET_HANDLER_OK;
}

static PacketHandlerState zoneHandlerSwapEtcInvChangeIndex(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg)
{
    #pragma pack(push, 1)
    struct {
        uint8_t inventoryType;
        uint64_t itemId1;
        uint32_t inventoryIndex1;
        uint64_t itemId2;
        uint32_t inventoryIndex2;
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

    CHECK_CLIENT_PACKET_SIZE(*clientPacket, packetSize, CZ_SWAP_ETC_INV_CHANGE_INDEX);

    // Delete item from inventory
    Inventory *inventory = &session->game.commanderSession.currentCommander->inventory;

    Item *item1;
    Item *item2;

    if (!inventoryGetItemByItemId(inventory, clientPacket->itemId1, &item1)) {
        error("Item1 not found in inventory");
        return PACKET_HANDLER_ERROR;
    }

    if (!inventoryGetItemByItemId(inventory, clientPacket->itemId2, &item2)) {
        error("Item2 not found in inventory");
        return PACKET_HANDLER_ERROR;
    }

    if (!inventorySwapItems(inventory, &item1, &item2)) {
        error("Error when swapping items in inventory");
        return PACKET_HANDLER_ERROR;
    }

    // No packet in return?

    return PACKET_HANDLER_OK;
}


