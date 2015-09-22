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

#include "barrack_builder.h"
#include "common/server/worker.h"
#include "common/packet/packet.h"
#include "common/packet/packet_stream.h"
#include "common/packet/packet_type.h"
#include "common/commander/inventory.h"
#include "common/item/item.h"

void barrackBuilderMessage(uint8_t msgType, uint8_t *message, zmsg_t *replyMsg) {

    // Length of Message
    int messageLength = strlen(message);

    #pragma pack(push, 1)
    struct {
        VariableSizePacketHeader variableSizeHeader;
        uint8_t msgType;
        uint8_t unk[40];
        uint8_t message[messageLength+1];
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = BC_MESSAGE;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        variableSizePacketHeaderInit(&replyPacket.variableSizeHeader, packetType, sizeof(replyPacket));
        replyPacket.msgType = msgType;
        strncpy(replyPacket.message, message, sizeof(replyPacket.message));
    }
}

void barrackBuilderLoginOk(
    uint64_t accountId,
    uint8_t *accountLogin,
    uint8_t *sessionKey,
    AccountSessionPrivileges accountPrivileges,
    zmsg_t *replyMsg)
{
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint16_t unk1;
        uint64_t accountId;
        uint8_t accountLogin[ACCOUNT_SESSION_LOGIN_MAXSIZE];
        uint32_t accountPrivileges;
        uint8_t sessionKey[GAME_SESSION_KEY_MAXSIZE];
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = BC_LOGINOK;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        serverPacketHeaderInit(&replyPacket.header, packetType);
        replyPacket.unk1 = 0x3E9; // from iCBT1
        replyPacket.accountId = accountId;
        replyPacket.accountPrivileges = accountPrivileges;
        strncpy(replyPacket.accountLogin, accountLogin, sizeof(replyPacket.accountLogin));
        strncpy(replyPacket.sessionKey, sessionKey, sizeof(replyPacket.sessionKey));
    }
}

void barrackBuilderStartGameOk(
    uint32_t zoneServerId,
    uint32_t zoneServerIp,
    uint32_t zoneServerPort,
    uint16_t mapId,
    uint8_t commanderListId,
    uint64_t socialInfoId,
    uint8_t isSingleMap,
    zmsg_t *replyMsg)
{
    #pragma pack(push, 1)
    struct BcStartGameOkPacket {
        ServerPacketHeader header;
        uint32_t zoneServerId; // TODO : RouterID 16 -> 32
        uint32_t zoneServerIp;
        uint32_t zoneServerPort;
        uint32_t mapId;
        uint8_t commanderListId;
        uint64_t socialInfoId;
        uint8_t isSingleMap;
        uint8_t unk1;
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = BC_START_GAMEOK;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        serverPacketHeaderInit(&replyPacket.header, packetType);
        replyPacket.zoneServerId = zoneServerId;
        replyPacket.zoneServerIp = zoneServerIp;
        replyPacket.zoneServerPort = zoneServerPort;
        replyPacket.mapId = mapId;
        replyPacket.commanderListId = commanderListId;
        replyPacket.socialInfoId = socialInfoId;
        replyPacket.isSingleMap = isSingleMap;
        replyPacket.unk1 = 1;
    }
}

void barrackBuilderCommanderMoveOk(
    uint64_t accountId,
    uint16_t commanderListId,
    PositionXYZ *position,
    zmsg_t *replyMsg)
{
    // uncompressed structure
    #pragma pack(push, 1)
    struct {
        PacketNormalHeader normalHeader;
        uint64_t accountId;
        uint8_t unk1;
        PositionXYZ position;
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = BC_NORMAL_COMMANDER_MOVE_OK;

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        packetNormalHeaderInit(&replyPacket.normalHeader, packetType, sizeof(replyPacket));
        replyPacket.accountId = accountId;
        replyPacket.unk1 = 3; // 3, 2, 1, 3, 2, 1, ...
        memcpy(&replyPacket.position, position, sizeof(replyPacket.position));
    }
}

void barrackBuilderNormalUnk1(uint64_t accountId, zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        PacketNormalHeader normalHeader;
        uint64_t accountId;
        uint8_t unk1;
        uint64_t unk2;
    } replyPacket;
    #pragma pack(pop)

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        packetNormalHeaderInit(&replyPacket.normalHeader, BC_NORMAL_UNKNOWN_1, sizeof(replyPacket));
        replyPacket.accountId = accountId;
        replyPacket.unk1 = 0; // ICBT
        replyPacket.unk2 = 0;
    }
}

void barrackBuilderIesModifyList(zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        // not yet implemented
    } replyPacket;
    (void) replyPacket;
    #pragma pack(pop)

    // PacketType packetType = BC_IES_MODIFY_LIST;
    // CHECK_SERVER_PACKET_SIZE (replyPacket, packetType);
    // BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        size_t memSize;

        void *memory = dumpToMem(
            "[03:07:13][main.c:55 in CNetUsr__PacketHandler_0]  48 00 FF FF FF FF 35 01 01 00 0C 00 53 68 61 72 | H.....5.....Shar\n"
            "[03:07:13][main.c:55 in CNetUsr__PacketHandler_0]  65 64 43 6F 6E 73 74 00 02 00 F5 00 00 00 01 00 | edConst.........\n"
            "[03:07:13][main.c:55 in CNetUsr__PacketHandler_0]  06 00 56 61 6C 75 65 00 02 00 0C 00 00 00 02 00 | ..Value.........\n"
            "[03:07:13][main.c:55 in CNetUsr__PacketHandler_0]  31 00 05 00 31 2E 30 30 00 03 00 51 41 00 0A 00 | 1...1.00...QA...\n"
            "[03:07:13][main.c:55 in CNetUsr__PacketHandler_0]  32 30 31 35 2D 30 38 2D 30 00 0F 00 43 68 61 6E | 2015-08-0...Chan\n"
            "[03:07:13][main.c:55 in CNetUsr__PacketHandler_0]  67 65 20 42 79 20 54 6F 6F 6C 00 0B 00 00 00 02 | ge By Tool......\n"
            "[03:07:13][main.c:55 in CNetUsr__PacketHandler_0]  00 31 00 05 00 31 2E 30 30 00 03 00 51 41 00 0A | .1...1.00...QA..\n"
            "[03:07:13][main.c:55 in CNetUsr__PacketHandler_0]  00 32 30 31 35 2D 30 38 2D 30 00 0F 00 43 68 61 | .2015-08-0...Cha\n"
            "[03:07:13][main.c:55 in CNetUsr__PacketHandler_0]  6E 67 65 20 42 79 20 54 6F 6F 6C 00 FB 00 00 00 | nge By Tool.....\n"
            "[03:07:13][main.c:55 in CNetUsr__PacketHandler_0]  01 00 06 00 56 61 6C 75 65 00 03 00 05 00 00 00 | ....Value.......\n"
            "[03:07:13][main.c:55 in CNetUsr__PacketHandler_0]  02 00 31 00 05 00 31 2E 30 30 00 05 00 4C 69 6C | ..1...1.00...Lil\n"
            "[03:07:13][main.c:55 in CNetUsr__PacketHandler_0]  79 00 0A 00 32 30 31 35 2D 30 38 2D 30 00 0F 00 | y...2015-08-0...\n"
            "[03:07:13][main.c:55 in CNetUsr__PacketHandler_0]  43 68 61 6E 67 65 20 42 79 20 54 6F 6F 6C 00 04 | Change By Tool..\n"
            "[03:07:13][main.c:55 in CNetUsr__PacketHandler_0]  00 00 00 02 00 31 00 05 00 31 2E 30 30 00 05 00 | .....1...1.00...\n"
            "[03:07:13][main.c:55 in CNetUsr__PacketHandler_0]  4C 69 6C 79 00 0A 00 32 30 31 35 2D 30 38 2D 30 | Lily...2015-08-0\n"
            "[03:07:13][main.c:55 in CNetUsr__PacketHandler_0]  00 0F 00 43 68 61 6E 67 65 20 42 79 20 54 6F 6F | ...Change By Too\n"
            "[03:07:13][main.c:55 in CNetUsr__PacketHandler_0]  6C 00 03 00 00 00 02 00 31 00 05 00 31 2E 30 30 | l.......1...1.00\n"
            "[03:07:13][main.c:55 in CNetUsr__PacketHandler_0]  00 05 00 4C 69 6C 79 00 0A 00 32 30 31 35 2D 30 | ...Lily...2015-0\n"
            "[03:07:13][main.c:55 in CNetUsr__PacketHandler_0]  38 2D 30 00 0F 00 43 68 61 6E 67 65 20 42 79 20 | 8-0...Change By \n"
            "[03:07:13][main.c:55 in CNetUsr__PacketHandler_0]  54 6F 6F 6C 00                                  | Tool.\n"
            , NULL, &memSize
        );

        zmsg_add(replyMsg, zframe_new(memory, memSize));
    }
}

void barrackBuilderServerEntry(
    uint32_t ipClientNet,
    uint32_t ipVirtualClientNet,
    uint16_t channelPort1,
    uint16_t channelPort2,
    zmsg_t *replyMsg)
{
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint32_t ipClientNet;
        uint32_t ipVirtualClientNet;
        uint16_t channelPort1;
        uint16_t channelPort2;
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = BC_SERVER_ENTRY;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        serverPacketHeaderInit(&replyPacket.header, packetType);
        replyPacket.ipClientNet        = ipClientNet;
        replyPacket.ipVirtualClientNet = ipVirtualClientNet;
        replyPacket.channelPort1       = channelPort1;
        replyPacket.channelPort2       = channelPort2;
    }
}

void barrackBuilderCommanderList(
    uint64_t accountId,
    GameSession *gameSession,
    Commander *commanders,
    int commandersCount,
    zmsg_t *replyMsg)
{
    // Keep sizes in memory
    size_t commanderBarrackInfoPacketSize = 0;
    size_t attributesSizeAllCommanders[commandersCount];
    size_t attributeSizeAllCommanders[commandersCount][EQSLOT_Count];

    for (int commanderIndex = 0; commanderIndex < commandersCount; commanderIndex++) {

        // iterate through all commander
        Commander *curCommander = &commanders[commanderIndex];
        Inventory *inventory = &curCommander->inventory;

        // TESTIG PURPOSES, DELETE LATER
        /*
        Item newItem;
        newItem.itemId = 1234;
        newItem.itemType = 531101;
        newItem.amount = 1;
        newItem.equipSlot = EQSLOT_BODY_ARMOR;
        newItem.attributes = itemAttributesNew(4200, 0, NULL, NULL, NULL, 0, 0);
        inventoryAddItem(inventory, &newItem);
        inventoryEquipItem(inventory, newItem.itemId, EQSLOT_BODY_ARMOR);
        */

        // get attributes size
        size_t attributesSize = 0;
        for (int eqSlotIndex = 0; eqSlotIndex < EQSLOT_Count; eqSlotIndex++) {

            Item *item = inventory->equippedItems[eqSlotIndex];

            // get attribute size
            size_t attrSize = item ? itemAttributesGetPacketSize(item->attributes) : 0;

            // get total structure size
            #pragma pack(push, 1)
            typedef struct {
                uint16_t attrSize;
                uint8_t attrBuffer[attrSize];
            } AttributePacket;
            #pragma pack(pop)

            attributesSize += sizeof(AttributePacket);
            attributeSizeAllCommanders[commanderIndex][eqSlotIndex] = attrSize;
        }

        attributesSizeAllCommanders[commanderIndex] = attributesSize;

        // get CommanderBarrackInfoPacket size
        #pragma pack(push, 1)
        typedef struct {
            CommanderAppearance commander;
            uint64_t socialInfoId;
            uint16_t commanderPosition;
            uint16_t mapId;
            uint32_t unk4;
            uint32_t unk5;
            uint32_t maxXP;
            uint32_t unk6;
            PositionXYZ pos;
            PositionXZ dir;
            PositionXYZ pos2;
            PositionXZ dir2;
            uint32_t unk8;
            uint8_t attributesPacket[attributesSize];
            uint16_t unk9;
        } CommanderBarrackInfoPacket;
        #pragma pack(pop)

        commanderBarrackInfoPacketSize += sizeof(CommanderBarrackInfoPacket);
    }

    // We got the final packet size, allocate replyPacket
    #pragma pack(push, 1)
    struct BarrackBuilderCommanderListPacket {
        VariableSizePacketHeader variableSizeHeader;
        uint64_t accountId;
        uint8_t unk1;
        uint8_t commandersCount;
        uint8_t familyName [COMMANDER_FAMILY_NAME_SIZE];

        uint16_t accountInfoLength; // sizeof(accountInfo)
        //AccountInfo accountInfo[accountInfoCount];

        // AccountInfo
        uint16_t typeCredits;
        float creditsAmount;
        uint16_t typeCredits2;
        float creditsAmount2;
        uint16_t typeCredits3;
        float creditsAmount3;

        uint8_t commandersBarrackInfoPacket[commanderBarrackInfoPacketSize];
    } replyPacket;
    #pragma pack(pop)

    PacketStream packetStream;
    packetStreamInit(&packetStream, &replyPacket);

    // Now fill it
    PacketType packetType = BC_COMMANDER_LIST;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        // fill replyPacket
        variableSizePacketHeaderInit(&replyPacket.variableSizeHeader, packetType, sizeof(replyPacket));
        replyPacket.accountId = accountId;
        replyPacket.unk1 = 1; // ICBT - equal to 1 or 4
        replyPacket.commandersCount = commandersCount;
        strncpy(replyPacket.familyName, gameSession->accountSession.familyName, sizeof(replyPacket.familyName));
        replyPacket.accountInfoLength = 0x12; // 3 sets
        replyPacket.typeCredits = SWAP_UINT16(0x940e); // 94 0E = Medal (iCoin)
        replyPacket.creditsAmount = gameSession->accountSession.credits;
        replyPacket.typeCredits2 = SWAP_UINT16(0x970e); // 94 0E = Medal (iCoin)
        replyPacket.creditsAmount2 = 0;
        replyPacket.typeCredits3 = SWAP_UINT16(0x950e); // 94 0E = Medal (iCoin)
        replyPacket.creditsAmount3 = 0;

        // we want to start writing at the offset of commandersBarrackInfoPacket
        size_t offset = offsetof(struct BarrackBuilderCommanderListPacket, commandersBarrackInfoPacket);
        packetStreamAddOffset(&packetStream, offset);

        // fill commandersBarrackInfoPacket
        for (int commanderIndex = 0; commanderIndex < commandersCount; commanderIndex++) {

            // iterate through all commander
            Commander *curCommander = &commanders[commanderIndex];
            CommanderInfo *cInfo = &curCommander->info;
            Inventory *inventory = &curCommander->inventory;

            // Define CommanderBarrackInfoPacket current structure
            size_t attributesSize = attributesSizeAllCommanders[commanderIndex];
            #pragma pack(push, 1)
            struct CommanderBarrackInfoPacket {
                CommanderAppearance appearance;
                uint64_t socialInfoId;
                uint16_t commanderPosition;
                uint16_t mapId;
                uint32_t unk4;
                uint32_t unk5;
                uint32_t maxXP;
                uint32_t unk6;
                PositionXYZ pos;
                PositionXZ dir;
                PositionXYZ pos2;
                PositionXZ dir2;
                uint32_t unk8;
                uint8_t attributesPacket[attributesSize];
                uint16_t unk9;
            } *curCommandersBarrackInfoPacket = packetStreamGetCurrentBuffer(&packetStream);
            #pragma pack(pop)

            // fill it
            curCommandersBarrackInfoPacket->appearance = cInfo->appearance;

            curCommandersBarrackInfoPacket->socialInfoId = cInfo->socialInfoId; // CharUniqueId?
            curCommandersBarrackInfoPacket->commanderPosition = commanderIndex + 1;
            curCommandersBarrackInfoPacket->mapId = 1002; /// FIXME : No MapId in the current structure!
            curCommandersBarrackInfoPacket->unk4 = SWAP_UINT32(0x02000000);
            curCommandersBarrackInfoPacket->unk5 = 0;
            curCommandersBarrackInfoPacket->maxXP = cInfo->maxXP;
            curCommandersBarrackInfoPacket->unk6 = SWAP_UINT32(0xC01C761C);
            curCommandersBarrackInfoPacket->pos = PositionXYZ_decl(
                SWAP_UINT32(0x25e852c1), SWAP_UINT32(0x6519e541), SWAP_UINT32(0x39f4ef42)
            );
            curCommandersBarrackInfoPacket->dir = PositionXZ_decl(0, 0); // Set direction to face camera.
            curCommandersBarrackInfoPacket->pos2 = curCommandersBarrackInfoPacket->pos;
            curCommandersBarrackInfoPacket->dir2 = curCommandersBarrackInfoPacket->dir;
            curCommandersBarrackInfoPacket->unk8 = 0;
            curCommandersBarrackInfoPacket->unk9 = 0;

            // fill attributes
            size_t offset = offsetof(struct CommanderBarrackInfoPacket, attributesPacket);
            packetStreamAddOffset(&packetStream, offset);

            for (int eqSlotIndex = 0; eqSlotIndex < EQSLOT_Count; eqSlotIndex++) {

                Item *item = inventory->equippedItems[eqSlotIndex];

                // get attribute size
                size_t attrSize = attributeSizeAllCommanders[commanderIndex][eqSlotIndex];

                // Define AttributePacket current structure
                #pragma pack(push, 1)
                struct AttributePacket {
                    uint16_t attrSize;
                    uint8_t attrBuffer[attrSize];
                } *attributePacket = packetStreamGetCurrentBuffer(&packetStream);
                #pragma pack(pop)

                attributePacket->attrSize = attrSize;

                // fill attribute buffer
                size_t offset = offsetof(struct AttributePacket, attrBuffer);
                packetStreamAddOffset(&packetStream, offset);

                // write in the buffer
                if (item) {
                    itemAttributesGetPacket(item->attributes, packetStreamGetCurrentBuffer(&packetStream));
                    // relocate the stream position
                    packetStreamAddOffset(&packetStream, attrSize);
                }
            }

            packetStreamAddOffset(&packetStream, sizeof_struct_member(struct CommanderBarrackInfoPacket, unk9));
        }
    }
}

void barrackBuilderPetInformation(zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        PacketNormalHeader normalHeader;
        uint32_t petsCount;
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = BC_NORMAL_PET_INFORMATION;
    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        packetNormalHeaderInit(&replyPacket.normalHeader, packetType, sizeof(replyPacket));
        replyPacket.petsCount = 0;
    }
}

void barrackBuilderZoneTraffics(uint16_t mapId, zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    typedef struct {
        uint16_t zoneListId;
        uint16_t currentPlayersCount;
    } SingleZoneTraffic;
    #pragma pack(pop)

    #pragma pack(push, 1)
    typedef struct {
        uint16_t mapId;
        uint16_t zoneServerCount;
        // SingleZoneTraffic zones[]; // variable length array
    }   SingleMapTraffic;
    #pragma pack(pop)

	#pragma pack(push, 1)
    typedef struct {
        uint16_t zoneMaxPcCount;
        uint16_t mapAvailableCount;
        // SingleMapTraffic maps[]; // variable length array
    } ZoneTrafficsPacket;
    #pragma pack(pop)

    // data from the DB
    int zoneMaxPcCount = 100;

    // number of maps playable
    int mapAvailableCount = 1;

    // array of zone server availables for each map
	int *zoneServerCounts = alloca(sizeof(*zoneServerCounts) * mapAvailableCount);

    // array of mapId for each map
    int *mapsId = alloca(sizeof(*mapsId) * mapAvailableCount);

    // fill the arrays here
    for (int mapIndex = 0; mapIndex < mapAvailableCount; mapIndex++) {
        zoneServerCounts [mapIndex] = 1;
        mapsId [mapIndex] = mapId;
    }
    // number of players per zone
    int currentPlayersCount = 10;

    // count the total memory space needed for the reply packet
    size_t outPacketSize = sizeof(ZoneTrafficsPacket) + (sizeof(SingleMapTraffic) * mapAvailableCount);

    for (int mapIndex = 0; mapIndex < mapAvailableCount; mapIndex++) {
        outPacketSize += sizeof(SingleZoneTraffic) * zoneServerCounts[mapIndex];
    }

    // allocate on the stack the memory for the packet
    uint8_t *stackBuffer = alloca(sizeof(*stackBuffer) * outPacketSize);
    memset(stackBuffer, 0, outPacketSize);

    // construct the packet
    PacketStream stream;
    packetStreamInit(&stream, (uint8_t *)stackBuffer);

    packetStreamAppend(&stream, &zoneMaxPcCount, sizeof_struct_member(ZoneTrafficsPacket, zoneMaxPcCount));
    packetStreamAppend(&stream, &mapAvailableCount, sizeof_struct_member(ZoneTrafficsPacket, mapAvailableCount));

    for (int mapIndex = 0; mapIndex < mapAvailableCount; mapIndex++) {
        packetStreamAppend(&stream, &mapsId [mapIndex], sizeof_struct_member(SingleMapTraffic, mapId));
        packetStreamAppend(&stream, &zoneServerCounts[mapIndex], sizeof_struct_member(SingleMapTraffic, zoneServerCount));

        for (int zoneServerIndex = 0; zoneServerIndex < zoneServerCounts[mapIndex]; zoneServerIndex++) {
            packetStreamAppend(&stream, &zoneServerIndex, sizeof_struct_member(SingleZoneTraffic, zoneListId));
            packetStreamAppend(&stream, &currentPlayersCount, sizeof_struct_member(SingleZoneTraffic, currentPlayersCount));
        }
    }

    // compress the packet
	#pragma pack(push, 1)
    struct {
        PacketNormalHeader normalHeader;
        Zlib zlibData;
    } compressedPacket;
    #pragma pack(pop)

    zlibCompress(&compressedPacket.zlibData, stackBuffer, outPacketSize);
    outPacketSize = ZLIB_GET_COMPRESSED_PACKET_SIZE(&compressedPacket.zlibData, sizeof(compressedPacket));
    packetNormalHeaderInit(&compressedPacket.normalHeader, BC_NORMAL_ZONE_TRAFFIC, outPacketSize);

    zmsg_add(replyMsg, zframe_new (&compressedPacket, outPacketSize));
}

void barrackBuilderBarrackNameChange(BarrackNameResultType resultType, uint8_t *barrackName, zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint8_t changed;
        uint32_t resultType;
        uint8_t barrackName[64];
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = BC_BARRACKNAME_CHANGE;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        serverPacketHeaderInit(&replyPacket.header, packetType);
        replyPacket.changed = (resultType == BC_BARRACKNAME_CHANGE_OK);
        replyPacket.resultType = resultType;
        strncpy(replyPacket.barrackName, barrackName, sizeof(replyPacket.barrackName));
    }
}

void barrackBuilderCommanderDestroy(uint8_t commanderDestroyMask, zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint8_t commanderDestroyMask;
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = BC_COMMANDER_DESTROY;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        serverPacketHeaderInit(&replyPacket.header, packetType);
        replyPacket.commanderDestroyMask = commanderDestroyMask;
    }
}

void barrackBuilderCommanderCreate(CommanderCreateInfo *commanderCreate, zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        CommanderCreateInfo commanderCreate;
    } replyPacket;
    #pragma pack(pop)

    // ICBT : those values are zeroes for some reason
    memset(commanderCreate->appearance.familyName, 0, sizeof(commanderCreate->appearance.familyName));
    commanderCreate->appearance.accountId = 0;

    PacketType packetType = BC_COMMANDER_CREATE;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        serverPacketHeaderInit(&replyPacket.header, packetType);
        memcpy(&replyPacket.commanderCreate, commanderCreate, sizeof(replyPacket.commanderCreate));
    }
}

void barrackBuilderLogoutOk(zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = BC_LOGOUTOK;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        serverPacketHeaderInit(&replyPacket.header, packetType);
    }
}
