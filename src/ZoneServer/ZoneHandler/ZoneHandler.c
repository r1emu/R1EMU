/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @license <license placeholder>
 */


// ---------- Includes ------------
#include "ZoneHandler.h"
#include "ZoneBuilder.h"
#include "AdminCmd.h"
#include "Common/Packet/Packet.h"
#include "Common/Redis/Fields/RedisGameSession.h"
#include "Common/Redis/Fields/RedisSocketSession.h"
#include "Common/Server/EventHandler.h"

// ------ Static declaration -------
/** Connect to the zone server */
static PacketHandlerState ZoneHandler_connect         (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** Client is ready to enter the zone */
static PacketHandlerState ZoneHandler_gameReady       (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** Jump handler */
static PacketHandlerState ZoneHandler_jump            (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** On air handler */
static PacketHandlerState ZoneHandler_onAir           (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** On ground handler */
static PacketHandlerState ZoneHandler_onGround        (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** On move with keyboard handler */
static PacketHandlerState ZoneHandler_keyboardMove    (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** On stop commander movement */
static PacketHandlerState ZoneHandler_moveStop        (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** On movement information */
static PacketHandlerState ZoneHandler_movementInfo    (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** On commander rotation */
static PacketHandlerState ZoneHandler_rotate          (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** On commander head rotation */
static PacketHandlerState ZoneHandler_headRotate      (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** @unknown */
static PacketHandlerState ZoneHandler_campInfo        (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** On log out */
static PacketHandlerState ZoneHandler_czQuickSlotList (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** On log out */
static PacketHandlerState ZoneHandler_itemUse         (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** On log out */
static PacketHandlerState ZoneHandler_iNeedParty      (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** On log out */
static PacketHandlerState ZoneHandler_logout          (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** On cast a spell on the ground */
static PacketHandlerState ZoneHandler_skillGround     (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** On commander sit */
static PacketHandlerState ZoneHandler_restSit         (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** On commander chat */
static PacketHandlerState ZoneHandler_chat            (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);

// ------ Structure declaration -------
/**
 * @brief zoneHandlers is a global table containing all the zone handlers.
 */
const PacketHandler zoneHandlers [PACKET_TYPE_COUNT] = {
    #define REGISTER_PACKET_HANDLER(packetName, handler) \
        [packetName] = {handler, STRINGIFY (packetName)}

    REGISTER_PACKET_HANDLER (CZ_CONNECT, ZoneHandler_connect),
    REGISTER_PACKET_HANDLER (CZ_GAME_READY, ZoneHandler_gameReady),
    REGISTER_PACKET_HANDLER (CZ_JUMP, ZoneHandler_jump),
    REGISTER_PACKET_HANDLER (CZ_ON_AIR, ZoneHandler_onAir),
    REGISTER_PACKET_HANDLER (CZ_ON_GROUND, ZoneHandler_onGround),
    REGISTER_PACKET_HANDLER (CZ_KEYBOARD_MOVE, ZoneHandler_keyboardMove),
    REGISTER_PACKET_HANDLER (CZ_MOVE_STOP, ZoneHandler_moveStop),
    REGISTER_PACKET_HANDLER (CZ_MOVEMENT_INFO, ZoneHandler_movementInfo),
    REGISTER_PACKET_HANDLER (CZ_ROTATE, ZoneHandler_rotate),
    REGISTER_PACKET_HANDLER (CZ_HEAD_ROTATE, ZoneHandler_headRotate),
    REGISTER_PACKET_HANDLER (CZ_CAMPINFO, ZoneHandler_campInfo),
    REGISTER_PACKET_HANDLER (CZ_QUICKSLOT_LIST, ZoneHandler_czQuickSlotList),
    REGISTER_PACKET_HANDLER (CZ_LOGOUT, ZoneHandler_logout),
    REGISTER_PACKET_HANDLER (CZ_ITEM_USE, ZoneHandler_itemUse),
    REGISTER_PACKET_HANDLER (CZ_I_NEED_PARTY, ZoneHandler_iNeedParty),
    REGISTER_PACKET_HANDLER (CZ_SKILL_GROUND, ZoneHandler_skillGround),
    REGISTER_PACKET_HANDLER (CZ_REST_SIT, ZoneHandler_restSit),
    REGISTER_PACKET_HANDLER (CZ_CHAT, ZoneHandler_chat),

    #undef REGISTER_PACKET_HANDLER
};

static PacketHandlerState
ZoneHandler_chat (
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg
) {
    // The first 2 bytes of ZC_CHAT are the packet size
    size_t chatTextSize = *((uint16_t *) packet) - sizeof (ClientPacketHeader) - sizeof (uint16_t);

    #pragma pack(push, 1)
    struct {
        uint16_t msgSize;
        uint8_t msgText [chatTextSize];
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

    if (sizeof (*clientPacket) != packetSize) {
        error ("The packet size received isn't correct. (packet size = %d, correct size = %d)",
            packetSize, sizeof (*clientPacket));

        return PACKET_HANDLER_ERROR;
    }

    // The chat message sent by the client should always finish by a null byte
    clientPacket->msgText [chatTextSize-1] = '\0';

    // Check for custom admin commands
    if (session->game.accountSession.privilege <= ACCOUNT_SESSION_PRIVILEGES_ADMIN
    && (strncmp (clientPacket->msgText, "/cmd ", sizeof (clientPacket->msgText)) == 0)
    ) {
        AdminCmd_process (self, clientPacket->msgText + strlen ("/cmd "), session, replyMsg);
    }

    else {
        // Normal message : Dispatch a GameEventChat
        size_t gameEventSize = sizeof (GameEventChat) + chatTextSize;
        GameEventChat *event = alloca (gameEventSize);
        event->pcId = session->game.commanderSession.currentCommander.pcId;
        memcpy (event->sessionKey, session->socket.sessionKey, sizeof (event->sessionKey));
        memcpy (event->familyName, session->game.commanderSession.currentCommander.base.familyName, sizeof (event->familyName));
        memcpy (event->commanderName, session->game.commanderSession.currentCommander.base.commanderName, sizeof (event->commanderName));
        memcpy (event->chatText, clientPacket->msgText, chatTextSize);
        Worker_dispatchEvent (self, EVENT_SERVER_TYPE_CHAT, event, gameEventSize);
    }

    return PACKET_HANDLER_OK;
}

static PacketHandlerState
ZoneHandler_restSit (
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        // This packet is actually empty
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

    if (sizeof (*clientPacket) != packetSize) {
        error ("The packet size received isn't correct. (packet size = %d, correct size = %d)",
            packetSize, sizeof (*clientPacket));

        return PACKET_HANDLER_ERROR;
    }

    // Notify the players around
    GameEventRestSit event = {
        .pcId = session->game.commanderSession.currentCommander.pcId,
        .sessionKey = SOCKET_ID_ARRAY (session->socket.sessionKey)
    };
    Worker_dispatchEvent (self, EVENT_SERVER_TYPE_REST_SIT, &event, sizeof (event));

    return PACKET_HANDLER_OK;
}

static PacketHandlerState
ZoneHandler_skillGround (
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg
) {
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

    if (sizeof (*clientPacket) != packetSize) {
        error ("The packet size received isn't correct. (packet size = %d, correct size = %d)",
            packetSize, sizeof (*clientPacket));

        return PACKET_HANDLER_ERROR;
    }

    /*   CzSkillGroundPacket :
         u1 skillId  unk2     x        y        z        x2       y2       z2       u3       u4       u5       u6 u7
         00 419C0000 00000000 DAFD24C4 74768243 1B77F1C3 DAFD24C4 74768243 1B77F1C3 FFFF7FBF 0020B539 00000000 00 01
         00 419C0000 00000000 720344C4 74768243 2178F6C3 720344C4 74768243 2178F6C3 F30435BF F40435BF 00000000 00 01
         00 419C0000 00000000 C9EC91C4 74768243 17060AC4 C9EC91C4 74768243 17060AC4 FFFF7F3F 0020B5B9 00000000 00 00
         00 429C0000 00000000 5A00FAC3 1F7CA143 B1D3E843 5A00FAC3 1F7CA143 B1D3E843 EE04353F F60435BF 00000000 00 00
         00 439C0000 00000000 1E43FFC3 1F7CA143 E130D443 1E43FFC3 1F7CA143 E130D443 EF04353F F80435BF 00000000 00 00
    */

    ZoneBuilder_playAni (replyMsg);

    ZoneBuilder_normalUnk8 (
        session->game.commanderSession.currentCommander.pcId,
        replyMsg
    );

    return PACKET_HANDLER_OK;
}

static PacketHandlerState
ZoneHandler_campInfo (
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        uint64_t accountId;
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

    // Check packet size
    CHECK_CLIENT_PACKET_SIZE (*clientPacket, CZ_CAMPINFO);
    if (sizeof (*clientPacket) != packetSize) {
        error ("The packet size received isn't correct. (packet size = %d, correct size = %d)",
            packetSize, sizeof (*clientPacket));
        return PACKET_HANDLER_ERROR;
    }

    ZoneBuilder_campInfo (session->socket.accountId, replyMsg);

    return PACKET_HANDLER_OK;
}

static PacketHandlerState
ZoneHandler_itemUse (
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg
) {
    warning ("CZ_ITEM_USE not implemented yet.");
    return PACKET_HANDLER_OK;
}

static PacketHandlerState
ZoneHandler_iNeedParty (
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg
) {
    warning ("CZ_I_NEED_PARTY not implemented yet.");
    // ZoneBuilder_partyInfo (replyMsg);
    // ZoneBuilder_partyList (replyMsg);

    return PACKET_HANDLER_OK;
}

static PacketHandlerState
ZoneHandler_logout (
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg
) {
    warning ("CZ_LOGOUT not implemented yet.");
    return PACKET_HANDLER_OK;
}

static PacketHandlerState
ZoneHandler_headRotate (
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg
) {
    warning ("CZ_HEAD_ROTATE not implemented yet.");
    return PACKET_HANDLER_OK;
}

static PacketHandlerState
ZoneHandler_rotate (
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg
) {
    warning ("CZ_ROTATE not implemented yet.");
    return PACKET_HANDLER_OK;
}

static PacketHandlerState
ZoneHandler_movementInfo (
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg
) {
    warning ("CZ_MOVEMENT_INFO not implemented yet.");
    return PACKET_HANDLER_OK;
}

static PacketHandlerState
ZoneHandler_moveStop (
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        uint8_t unk1;
        PositionXYZ position;
        PositionXZ direction;
        float timestamp;
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

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
    if (sizeof (*clientPacket) != packetSize) {
        error ("The packet size received isn't correct. (packet size = %d, correct size = %d)",
            packetSize, sizeof (*clientPacket));

        return PACKET_HANDLER_ERROR;
    }

    // Notify the players around
    GameEventMoveStop event = {
        .updatePosEvent = {
            .mapId = session->socket.mapId,
            .commanderInfo = session->game.commanderSession.currentCommander,
            .sessionKey = SOCKET_ID_ARRAY (session->socket.sessionKey)
        },
        .position = clientPacket->position,
        .direction = clientPacket->direction,
        .timestamp = clientPacket->timestamp,
    };
    Worker_dispatchEvent (self, EVENT_SERVER_TYPE_MOVE_STOP, &event, sizeof (event));

    return PACKET_HANDLER_OK;
}

static PacketHandlerState
ZoneHandler_keyboardMove (
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg
) {
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
    if (sizeof (*clientPacket) != packetSize) {
        error ("The packet size received isn't correct. (packet size = %d, correct size = %d)",
            packetSize, sizeof (*clientPacket));

        return PACKET_HANDLER_ERROR;
    }

    // TODO : Check coordinates

    // Update session
    session->game.commanderSession.currentCommander.pos = clientPacket->position;

    // Notify the players around
    GameEventCommanderMove event = {
        .updatePosEvent = {
            .mapId = session->socket.mapId,
            .commanderInfo = session->game.commanderSession.currentCommander,
            .sessionKey = SOCKET_ID_ARRAY (session->socket.sessionKey)
        },
        .position = clientPacket->position,
        .direction = clientPacket->direction,
        .timestamp = clientPacket->timestamp,
    };
    Worker_dispatchEvent (self, EVENT_SERVER_TYPE_COMMANDER_MOVE, &event, sizeof (event));

    return PACKET_HANDLER_UPDATE_SESSION;
}

static PacketHandlerState
ZoneHandler_czQuickSlotList (
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg
) {
    warning ("CZ_QUICKSLOT_LIST not implemented yet.");
    // Answer PacketType : ZC_QUICKSLOT_REGISTER
    return PACKET_HANDLER_OK;
}

static PacketHandlerState
ZoneHandler_gameReady (
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg
) {
    CommanderInfo *commanderInfo = &session->game.commanderSession.currentCommander;

    ZoneBuilder_itemInventoryList (replyMsg);
    ZoneBuilder_sessionObjects (replyMsg);
    ZoneBuilder_optionList (replyMsg);
    ZoneBuilder_skillmapList (replyMsg);
    ZoneBuilder_achievePointList (replyMsg);
    ZoneBuilder_chatMacroList (replyMsg);
    ZoneBuilder_uiInfoList (replyMsg);
    ZoneBuilder_npcStateList (replyMsg);

    ZoneBuilder_helpList (replyMsg);
    ZoneBuilder_myPageMap (replyMsg);
    ZoneBuilder_guestPageMap (replyMsg);
    ZoneBuilder_startInfo (replyMsg);

    ZoneBuilder_itemEquipList (replyMsg);
    // ZoneBuilder_skillList (commanderInfo->pcId, replyMsg);
    ZoneBuilder_abilityList (commanderInfo->pcId, replyMsg);
    ZoneBuilder_cooldownList (commanderInfo->socialInfoId, replyMsg);

    ZoneBuilder_quickSlotList (replyMsg);

    ZoneBuilder_normalUnk1 (replyMsg);
    ZoneBuilder_normalUnk2 (replyMsg);
    ZoneBuilder_normalUnk3 (replyMsg);
    ZoneBuilder_normalUnk4 (commanderInfo->socialInfoId, replyMsg);
    ZoneBuilder_normalUnk5 (replyMsg);

    ZoneBuilder_startGame (1.0, 0.0, 0.0, 0.0, replyMsg);
    /*
    ZoneBuilder_objectProperty (replyMsg);
    ZoneBuilder_stamina (replyMsg);
    */
    ZoneBuilder_loginTime (replyMsg);

    ZoneBuilder_MyPCEnter (&commanderInfo->pos, replyMsg);
    // ZoneBuilder_skillAdd (replyMsg);

    // Notify players around that a new PC has entered
    GameEventPcEnter pcEnterEvent = {
        .updatePosEvent = {
            .mapId = session->socket.mapId,
            .commanderInfo = *commanderInfo,
            .sessionKey = SOCKET_ID_ARRAY (session->socket.sessionKey)
        }
    };
    Worker_dispatchEvent (self, EVENT_SERVER_TYPE_ENTER_PC, &pcEnterEvent, sizeof (pcEnterEvent));
    ZoneBuilder_enterPc (&pcEnterEvent.updatePosEvent.commanderInfo, replyMsg);

    // ZoneBuilder_buffList (commanderInfo->base.pcId, replyMsg);

    // Add NPC at the start screen
    // ZoneBuilder_enterMonster (replyMsg);
    // ZoneBuilder_faction (replyMsg);

    /*
    ZoneBuilder_normalUnk6 (
        commanderInfo->base.commanderName,
        replyMsg
    );

    ZoneBuilder_normalUnk7 (
        session->socket.accountId,
        session->game.commanderSession.currentCommander.base.pcId,
        session->game.commanderSession.currentCommander.base.familyName,
        session->game.commanderSession.currentCommander.base.commanderName,
        replyMsg
    );

    ZoneBuilder_jobPts (replyMsg);
    ZoneBuilder_normalUnk9 (session->game.commanderSession.currentCommander.base.pcId, replyMsg);
    ZoneBuilder_addonMsg (replyMsg);
    */

    ZoneBuilder_moveSpeed (session->game.commanderSession.currentCommander.pcId, 31.0f, replyMsg);

    return PACKET_HANDLER_UPDATE_SESSION;
}

static PacketHandlerState
ZoneHandler_connect (
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        uint32_t unk1;
        uint64_t accountId;
        uint64_t zoneServerId;
        uint8_t login [ACCOUNT_SESSION_LOGIN_MAXSIZE];
        uint8_t unk4;
        uint32_t zoneServerIndex;
        uint16_t unk3;
        uint8_t channelListId;
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

    // TODO : Reverse CZ_CONNECT correctly
    /*
    CHECK_CLIENT_PACKET_SIZE (*clientPacket, CZ_CONNECT);
    if (sizeof (*clientPacket) != packetSize) {
        error ("The packet size received isn't correct. (packet size = %d, correct size = %d)",
            packetSize, sizeof (*clientPacket));

        return PACKET_HANDLER_ERROR;
    }
    */

    // Get the Game Session that the Barrack Server moved
    RedisGameSessionKey gameKey = {
        .routerId = self->info.routerId,
        .mapId = -1,
        .accountId = clientPacket->accountId
    };
    if (!(Redis_getGameSession (self->redis, &gameKey, &session->game))) {
        error ("Cannot retrieve the game session.");
        return PACKET_HANDLER_ERROR;
    }

    // Check the client packet here (authentication)
    if (strncmp (session->game.accountSession.login, clientPacket->login, sizeof (session->game.accountSession.login)) != 0) {
        error ("Wrong account authentication. (clientPacket account = <%s>, Session account = <%s>",
            clientPacket->login, session->game.accountSession.login);
        return PACKET_HANDLER_ERROR;
    }

    // Authentication OK !
    // Update the Socket Session
    SocketSession_init (&session->socket,
        clientPacket->accountId,
        self->info.routerId,
        session->game.commanderSession.mapId,
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

    if (!(Redis_moveGameSession (self->redis, &fromKey, &toKey))) {
        error ("Cannot move the game session to the current mapId.");
        return PACKET_HANDLER_ERROR;
    }

    // Position : Official starting point position (tutorial map)
    session->game.commanderSession.currentCommander.pos = PositionXYZ_decl (-628.0f, 260.0f, -1025.0f);

    ZoneBuilder_connectOk (
        session->game.commanderSession.currentCommander.pcId,
        0, // GameMode
        0, // accountPrivileges
        &session->game.commanderSession.currentCommander, // Current commander
        replyMsg
    );

    return PACKET_HANDLER_UPDATE_SESSION;
}

static PacketHandlerState
ZoneHandler_jump (
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        uint8_t unk1;
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

    if (sizeof (*clientPacket) != packetSize) {
        error ("The packet size received isn't correct. (packet size = %d, correct size = %d)",
            packetSize, sizeof (*clientPacket));

        return PACKET_HANDLER_ERROR;
    }

    // Notify the players around
    GameEventJump event = {
        .updatePosEvent = {
            .mapId = session->socket.mapId,
            .commanderInfo = session->game.commanderSession.currentCommander,
            .sessionKey = SOCKET_ID_ARRAY (session->socket.sessionKey)
        },
        .height = COMMANDER_HEIGHT_JUMP
    };
    Worker_dispatchEvent (self, EVENT_SERVER_TYPE_JUMP, &event, sizeof (event));

    return PACKET_HANDLER_OK;
}

static PacketHandlerState
ZoneHandler_onAir (
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg
) {
    warning ("CZ_ON_AIR not implemented yet.");
    return PACKET_HANDLER_OK;
}

static PacketHandlerState
ZoneHandler_onGround (
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg
) {
    warning ("CZ_ON_GROUND not implemented yet.");
    return PACKET_HANDLER_OK;
}
