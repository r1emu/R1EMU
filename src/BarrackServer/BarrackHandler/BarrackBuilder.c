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
#include "BarrackBuilder.h"
#include "Common/Server/Worker.h"
#include "Common/Packet/Packet.h"
#include "Common/Packet/PacketStream.h"
#include "Common/Packet/PacketType.h"


// ------ Structure declaration -------


// ------ Static declaration -------


// ------ Extern function implementation -------

void
BarrackBuilder_loginOk (
    uint64_t accountId,
    uint8_t *accountLogin,
    uint8_t *sessionKey,
    AccountSessionPrivileges accountPrivileges,
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint16_t unk1;
        uint64_t accountId;
        uint8_t accountLogin [ACCOUNT_SESSION_LOGIN_MAXSIZE];
        uint32_t accountPrivileges;
        uint8_t sessionKey [GAME_SESSION_KEY_MAXSIZE];
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = BC_LOGINOK;
    CHECK_SERVER_PACKET_SIZE (replyPacket, packetType);
    BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        ServerPacketHeader_init (&replyPacket.header, packetType);
        replyPacket.unk1 = 0x3E9; // From iCBT1
        replyPacket.accountId = accountId;
        replyPacket.accountPrivileges = accountPrivileges;
        strncpy (replyPacket.accountLogin, accountLogin, sizeof (replyPacket.accountLogin));
        strncpy (replyPacket.sessionKey, sessionKey, sizeof (replyPacket.sessionKey));
    }
}

void
BarrackBuilder_startGameOk (
    uint32_t zoneServerId,
    uint8_t *zoneServerIp,
    uint32_t zoneServerPort,
    uint16_t mapId,
    uint8_t commanderListId,
    uint64_t socialInfoId,
    uint8_t isSingleMap,
    zmsg_t *replyMsg
) {
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
    CHECK_SERVER_PACKET_SIZE (replyPacket, packetType);
    BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        ServerPacketHeader_init (&replyPacket.header, packetType);
        replyPacket.zoneServerId = zoneServerId;
        replyPacket.zoneServerIp = *(uint32_t *) ((char []) {127, 0, 0, 1});
        replyPacket.zoneServerPort = zoneServerPort;
        replyPacket.mapId = mapId;
        replyPacket.commanderListId = commanderListId;
        replyPacket.socialInfoId = socialInfoId;
        replyPacket.isSingleMap = isSingleMap;
        replyPacket.unk1 = 1;
    }
}

void
BarrackBuilder_commanderMoveOk (
    uint64_t accountId,
    uint16_t commanderListId,
    PositionXYZ *position,
    zmsg_t *replyMsg
) {
    // Uncompressed structure
    #pragma pack(push, 1)
    struct {
        PacketNormalHeader normalHeader;
        uint64_t accountId;
        uint8_t unk1;
        PositionXYZ position;
    } replyPacket;
    #pragma pack(pop)

    BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        PacketNormalHeader_init (&replyPacket.normalHeader, BC_NORMAL_COMMANDER_MOVE_OK, sizeof (replyPacket));
        replyPacket.accountId = accountId;
        replyPacket.unk1 = 3; // 3, 2, 1, 3, 2, 1, ...
        memcpy (&replyPacket.position, position, sizeof (replyPacket.position));
    }
}

void
BarrackBuilder_normalUnk1 (
    uint64_t accountId,
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        PacketNormalHeader normalHeader;
        uint64_t accountId;
        uint8_t unk1;
        uint64_t unk2;
    } replyPacket;
    #pragma pack(pop)

    BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        PacketNormalHeader_init (&replyPacket.normalHeader, BC_NORMAL_UNKNOWN_1, sizeof (replyPacket));
        replyPacket.accountId = accountId;
        replyPacket.unk1 = 0; // ICBT
        replyPacket.unk2 = 0;
    }
}

void
BarrackBuilder_iesModifyList (
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        // Not yet implemented
    } replyPacket;
    (void) replyPacket;
    #pragma pack(pop)

    // PacketType packetType = BC_IES_MODIFY_LIST;
    // CHECK_SERVER_PACKET_SIZE (replyPacket, packetType);
    // BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        size_t memSize;
        void *memory = dumpToMem (
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

        zmsg_add (replyMsg, zframe_new (memory, memSize));
    }
}

void
BarrackBuilder_serverEntry (
    uint32_t ipClientNet,
    uint32_t ipVirtualClientNet,
    uint16_t channelPort1,
    uint16_t channelPort2,
    zmsg_t *replyMsg
) {
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
    CHECK_SERVER_PACKET_SIZE (replyPacket, packetType);
    BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        ServerPacketHeader_init (&replyPacket.header, packetType);
        replyPacket.ipClientNet        = ipClientNet;
        replyPacket.ipVirtualClientNet = ipVirtualClientNet;
        replyPacket.channelPort1       = channelPort1;
        replyPacket.channelPort2       = channelPort2;
    }
}

void
BarrackBuilder_commanderList (
    uint64_t accountId,
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        VariableSizePacketHeader variableSizeHeader;
        uint64_t accountId;
        uint8_t unk1;
        uint8_t commandersCount;
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = BC_COMMANDER_LIST;
    CHECK_SERVER_PACKET_SIZE (replyPacket, packetType);
    BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        VariableSizePacketHeader_init (&replyPacket.variableSizeHeader, packetType, sizeof (replyPacket));
        replyPacket.accountId = accountId;
        replyPacket.unk1 = 1; // ICBT - equal to 1 or 4
        replyPacket.commandersCount = 0;
    }
}

void
BarrackBuilder_petInformation (
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        PacketNormalHeader normalHeader;
        uint32_t petsCount;
    } replyPacket;
    #pragma pack(pop)

    BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        PacketNormalHeader_init (&replyPacket.normalHeader, BC_NORMAL_PET_INFORMATION, sizeof (replyPacket));
        replyPacket.petsCount = 0;
    }
}

void
BarrackBuilder_zoneTraffics (
    zmsg_t *replyMsg
) {
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

    // === Data from the DB ===
    int zoneMaxPcCount = 100;
    // Number of maps playable
    int mapAvailableCount = 1;
    // Array of zone server availables for each map
	int *zoneServerCounts = alloca (sizeof (*zoneServerCounts) * mapAvailableCount);
    // Array of mapId for each map
    int *mapsId = alloca (sizeof (*mapsId) * mapAvailableCount);
    // Fill the arrays here
    for (int mapIndex = 0; mapIndex < mapAvailableCount; mapIndex++) {
        zoneServerCounts [mapIndex] = 5;
        mapsId [mapIndex] = 0x3fd; // West Siauliai Woods
    }
    // Number of players per zone
    int currentPlayersCount = 10;

    // Count the total memory space needed for the reply packet
    size_t outPacketSize = sizeof (ZoneTrafficsPacket) + (sizeof (SingleMapTraffic) * mapAvailableCount);
    for (int mapIndex = 0; mapIndex < mapAvailableCount; mapIndex++) {
        outPacketSize += sizeof (SingleZoneTraffic) * zoneServerCounts[mapIndex];
    }

    // Allocate on the stack the memory for the packet
    uint8_t *stackBuffer = alloca (sizeof (*stackBuffer) * outPacketSize);
    memset (stackBuffer, 0, outPacketSize);

    // Construct the packet
    PacketStream stream;
    PacketStream_init (&stream, (uint8_t *) stackBuffer);

    PacketStream_append (&stream, &zoneMaxPcCount, sizeof_struct_member (ZoneTrafficsPacket, zoneMaxPcCount));
    PacketStream_append (&stream, &mapAvailableCount, sizeof_struct_member (ZoneTrafficsPacket, mapAvailableCount));

    for (int mapIndex = 0; mapIndex < mapAvailableCount; mapIndex++)
    {
        PacketStream_append (&stream, &mapsId [mapIndex], sizeof_struct_member (SingleMapTraffic, mapId));
        PacketStream_append (&stream, &zoneServerCounts[mapIndex], sizeof_struct_member (SingleMapTraffic, zoneServerCount));

        for (int zoneServerIndex = 0; zoneServerIndex < zoneServerCounts[mapIndex]; zoneServerIndex++) {
            PacketStream_append (&stream, &zoneServerIndex, sizeof_struct_member (SingleZoneTraffic, zoneListId));
            PacketStream_append (&stream, &currentPlayersCount, sizeof_struct_member (SingleZoneTraffic, currentPlayersCount));
        }
    }

    // Compress the packet
	#pragma pack(push, 1)
    struct {
        PacketNormalHeader normalHeader;
        Zlib zlibData;
    } compressedPacket;
    #pragma pack(pop)

    Zlib_compress (&compressedPacket.zlibData, stackBuffer, outPacketSize);
    outPacketSize = ZLIB_GET_COMPRESSED_PACKET_SIZE (&compressedPacket.zlibData, sizeof (compressedPacket));
    PacketNormalHeader_init (&compressedPacket.normalHeader, BC_NORMAL_ZONE_TRAFFIC, outPacketSize);

    zmsg_add (replyMsg, zframe_new (&compressedPacket, outPacketSize));
}

void
BarrackBuilder_barrackNameChange (
    uint8_t *barrackName,
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint8_t unk1[5];
        uint8_t barrackName[64];
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = BC_BARRACKNAME_CHANGE;
    CHECK_SERVER_PACKET_SIZE (replyPacket, packetType);
    BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        ServerPacketHeader_init (&replyPacket.header, packetType);
        memcpy (replyPacket.unk1, "\x01\x01\x01\x01\x01", sizeof (replyPacket.unk1));
        strncpy (replyPacket.barrackName, barrackName, sizeof (replyPacket.barrackName));
    }
}

void
BarrackBuilder_commanderDestroy (
    uint8_t commanderDestroyMask,
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint8_t commanderDestroyMask;
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = BC_COMMANDER_DESTROY;
    CHECK_SERVER_PACKET_SIZE (replyPacket, packetType);
    BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        ServerPacketHeader_init (&replyPacket.header, packetType);
        replyPacket.commanderDestroyMask = commanderDestroyMask;
    }
}

void
BarrackBuilder_commanderCreate (
    CommanderCreateInfo *commanderCreate,
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        CommanderCreateInfo commanderCreate;
    } replyPacket;
    #pragma pack(pop)

    // ICBT : Those values are zeroes for some reason
    memset (commanderCreate->commander.familyName, 0, sizeof (commanderCreate->commander.familyName));
    commanderCreate->commander.accountId = 0;

    PacketType packetType = BC_COMMANDER_CREATE;
    CHECK_SERVER_PACKET_SIZE (replyPacket, packetType);
    BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        ServerPacketHeader_init (&replyPacket.header, packetType);
        memcpy (&replyPacket.commanderCreate, commanderCreate, sizeof (replyPacket.commanderCreate));
    }
}
