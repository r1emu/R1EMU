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

#include "zone_builder.h"
#include "common/utils/time.h"
#include "common/server/worker.h"
#include "common/packet/packet.h"
#include "common/packet/packet_type.h"
#include "common/packet/packet_stream.h"

void zoneBuilderRestSit(PcId_t targetPcId, zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        PcId_t pcId;
        uint8_t isSit;
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = ZC_REST_SIT;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        replyPacket.header.type = packetType;
        replyPacket.pcId = targetPcId;
        replyPacket.isSit = false;
    }
}

void zoneBuilderItemAdd(Item *item, ItemInventoryIndex_t inventoryIndex, InventoryAddType addType, zmsg_t *replyMsg) {

    size_t propertiesSize = itemGetPropertiesCPacketSize(item);

    #pragma pack(push, 1)
    struct {
        VariableSizePacketHeader variableSizeHeader;
        ItemCPacket item;
        uint16_t propertiesSize;
        InventoryAddType_t addType;
        float notificationDelay;
        uint8_t inventoryType;
        uint8_t properties[propertiesSize];
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = ZC_ITEM_ADD;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        variableSizePacketHeaderInit(&replyPacket.variableSizeHeader, packetType, sizeof(replyPacket));
        replyPacket.item.uid = actorGetUId(item);
        replyPacket.item.id = itemGetId(item);
        replyPacket.item.amount = itemGetAmount(item);
        replyPacket.item.inventoryIndex = inventoryIndex;
        replyPacket.propertiesSize = propertiesSize;
        replyPacket.addType = addType;
        replyPacket.notificationDelay = 0.0f;
        replyPacket.inventoryType = 0;

        PacketStream packetStream;
        packetStreamInit(&packetStream, replyPacket.properties);
        itemPropertiesGetCPacket(item, &packetStream);
    }

    buffer_print(&replyPacket, sizeof(replyPacket), NULL);
}

void zoneBuilderSkillReady(
    PcId_t targetPcId,
    SkillId_t skillId,
    PositionXYZ *pos1,
    PositionXYZ *pos2,
    zmsg_t *replyMsg)
{
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        PcId_t pcId;
        SkillId_t skillId;
        float unk3;
        uint16_t unk4;
        uint16_t unk5;
        PositionXYZ pos1;
        PositionXYZ pos2;
    } replyPacket;
    #pragma pack(pop)

    /*  pcId     skillId  u3       u4        x        y        z        x2       y2       z2
        5A730100 419C0000 0000803F 011C ECC7 DAFD24C4 74768243 1B77F1C3 DAFD24C4 74768243 1B77F1C3
        5A730100 419C0000 0000803F 011C ECC7 720344C4 74768243 2178F6C3 720344C4 74768243 2178F6C3
        5A730100 419C0000 0000803F 011C ECC7 C9EC91C4 74768243 17060AC4 C9EC91C4 74768243 17060AC4
    */
    PacketType packetType = ZC_SKILL_READY;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        serverPacketHeaderInit(&replyPacket.header, packetType);
        replyPacket.pcId = targetPcId;
        replyPacket.skillId = skillId;
        replyPacket.unk3 = 1.0;
        replyPacket.unk4 = SWAP_UINT16(0x011C);
        replyPacket.unk5 = SWAP_UINT16(0xECC7);
        memcpy(&replyPacket.pos1, pos1, sizeof(replyPacket.pos1));
        memcpy(&replyPacket.pos2, pos1, sizeof(replyPacket.pos2));
    }
}

void zoneBuilderPlayAni(zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint32_t unkId1;
        uint32_t unkId2;
        uint8_t isUnk1;
        uint8_t isUnk2;
        float timeDelay;
        float unk5;
    } replyPacket;
    #pragma pack(pop)

    /*
        4B010000 25182700 01 01 00000000 0000803F
    */
    PacketType packetType = ZC_PLAY_ANI;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        serverPacketHeaderInit(&replyPacket.header, packetType);
        replyPacket.unkId1 = SWAP_UINT32(0x48010000);
        replyPacket.unkId2 = SWAP_UINT32(0x271826);
        replyPacket.isUnk1 = 1;
        replyPacket.isUnk2 = 1;
        replyPacket.timeDelay = 0;
        replyPacket.unk5 = SWAP_UINT32(0x0000803F);
    }
}

void zoneBuilderSkillCast(
    PcId_t targetPcId,
    SkillId_t skillId,
    PositionXYZ *position1,
    PositionXYZ *position2,
    zmsg_t *replyMsg)
{
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        PcId_t pcId;
        uint16_t unk2;
        uint16_t unk3;
        PositionXYZ position1;
        PositionXYZ position2;
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = ZC_SKILL_CAST;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        serverPacketHeaderInit(&replyPacket.header, packetType);
        replyPacket.pcId = targetPcId;
        replyPacket.unk2 = 0xECC7;
        replyPacket.unk3 = 0;
        memcpy(&replyPacket.position1, position1, sizeof(PositionXYZ));
        memcpy(&replyPacket.position2, position2, sizeof(PositionXYZ));
    }
}

void zoneBuilderPlaySkillCastAni(PcId_t targetPcId, PositionXYZ *position, zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        PcId_t pcId;
        PositionXYZ position;
        uint16_t unk1;
        uint16_t unk2;
        uint32_t unk3;
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = ZC_PLAY_SKILL_CAST_ANI;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        serverPacketHeaderInit(&replyPacket.header, packetType);
        replyPacket.pcId = targetPcId;
        memcpy(&replyPacket.position, position, sizeof(PositionXYZ));
        replyPacket.unk1 = 0xECC7;
        replyPacket.unk2 = 0;
        replyPacket.unk3 = 0;
    }
}

void zoneBuilderNormalUnk8(PcId_t targetPcId, zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        PacketNormalHeader normalHeader;
        PcId_t pcId;
        uint32_t unk1;
        SkillId_t skillId;
        SkillLevel_t skillLevel; // ? not sure
        uint8_t unk2[61];
    } replyPacket;
    #pragma pack(pop)

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        size_t memSize = sizeof(replyPacket);
        dumpToMem(
            "[11:37:20][           ToSClient:                     dbgBuffer]  30 0D FF FF FF FF 59 00 54 00 00 00 36 F8 01 00 | 0.....Y.T...6...\n"
            "[11:37:20][           ToSClient:                     dbgBuffer]  11 18 27 00 41 9C 00 00 02 00 00 00 BC BD A9 C4 | ..'.A...........\n"
            "[11:37:20][           ToSClient:                     dbgBuffer]  6A 1F 02 43 57 94 34 C3 4C EF 2E B4 00 00 80 3F | j..CW.4.L......?\n"
            "[11:37:20][           ToSClient:                     dbgBuffer]  6A 21 22 B4 00 00 A0 41 1A 74 00 00 01 00 00 00 | j!.....A.t......\n"
            "[11:37:20][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:37:20][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00                      | .........\n"
          , &replyPacket, &memSize
        );

        replyPacket.pcId = targetPcId;
    }
}

void zoneBuilderNormalUnk9(PcId_t targetPcId, zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        PacketNormalHeader normalHeader;
        PcId_t pcId;
        uint8_t unk1;
        uint32_t unk2;
        uint32_t unk3;
        uint32_t unk4;
    } replyPacket;
    #pragma pack(pop)

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        size_t memSize = sizeof(replyPacket);
        dumpToMem(
            "[11:10:23][           ToSClient:                     dbgBuffer]  30 0D 2E D3 5E 00 1D 00 24 01 00 00 5A 73 01 00 | 0...^...$...Zs..\n"
            "[11:10:23][           ToSClient:                     dbgBuffer]  00 6E 00 00 00 FF FF FF FF 57 04 00 00          | .n.......W...\n"
          , &replyPacket, &memSize
        );

        replyPacket.pcId = targetPcId;
    }
}

void zoneBuilderPartyList(zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        // not yet implemented
    } replyPacket;
   (void) replyPacket;
    #pragma pack(pop)

    // BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        size_t memSize;
        void *memory = dumpToMem(
            "[11:10:22][           ToSClient:                     dbgBuffer]  82 0C FF FF FF FF 44 01 00 00 34 54 A3 00 00 00 | ......D...4T....\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 16 00 00 00 13 9D 01 00 34 35 34 31 37 39 | ..........454179\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  01 30 36 36 35 30 31 33 D1 A8 01 44 00 00 00 00 | .0665013...D....\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  EA B5 AC EC 9B 90 EC 9E 90 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  F3 D4 56 00 30 35 E3 13 0B 05 04 00 46 CF 01 00 | ..V.05......F...\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  EA B5 AC EC 9B 90 EC 9E 90 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  43 42 54 EC 9A A9 00 00 00 00 00 00 00 00 00 00 | CBT.............\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 DB 1F 00 0C 00 00 00 01 2E 55 00 45 95 09 00 | ..........U.E...\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  4A 00 00 00 00 00 B4 C2 00 40 A2 44 00 D0 0A C5 | J........@.D....\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  EC 01 00 00 24 04 00 00 EC 01 00 00 24 04 00 00 | ....$.......$...\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  12 00 84 1B 00 80 9D 46 76 1B 00 68 97 46 85 1B | .......Fv..h.F..\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  18 72 BE 49                                     | .r.I\n"
            , NULL, &memSize
        );

        zmsg_add(replyMsg, zframe_new(memory, memSize));
    }
}

void zoneBuilderPartyInfo(zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        // not yet implemented
    } replyPacket;
   (void) replyPacket;
    #pragma pack(pop)

    // BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        size_t memSize;
        void *memory = dumpToMem(
            "[11:10:22][           ToSClient:                     dbgBuffer]  81 0C FF FF FF FF A4 00 00 00 34 54 A3 00 00 00 | ..........4T....\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 16 00 00 00 13 9D 01 00 34 35 34 31 37 39 | ..........454179\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  39 30 36 36 35 30 31 33 34 00 36 00 36 00 35 00 | 906650134.6.6.5.\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  30 00 31 00 33 00 34 00 00 00 00 00 00 00 00 00 | 0.1.3.4.........\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 D1 A8 01 44 00 00 00 00 EA B5 AC EC 9B 90 | .....D..........\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  EC 9E 90 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 02 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00                                     | ....\n"
            , NULL, &memSize
        );

        zmsg_add(replyMsg, zframe_new(memory, memSize));
    }
}

void zoneBuilderJobPts(zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        // not yet implemented
    } replyPacket;
   (void) replyPacket;
    #pragma pack(pop)

    // BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        size_t memSize;
        void *memory = dumpToMem(
            "[11:10:22][           ToSClient:                     dbgBuffer]  18 0D FF FF FF FF 04 00 01 00                   | .........."
            , NULL, &memSize
        );

        zmsg_add(replyMsg, zframe_new(memory, memSize));
    }
}

void zoneBuilderNormalUnk7(
    AccountId_t accountId,
    PcId_t targetPcId,
    char *familyName,
    char *commanderName,
    zmsg_t *replyMsg)
{
    #pragma pack(push, 1)
    struct {
        // not yet implemented
    } replyPacket;
    #pragma pack(pop)

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        size_t memSize = sizeof(replyPacket);
        dumpToMem(
            "[11:10:22][           ToSClient:                     dbgBuffer]  30 0D FF FF FF FF 25 01 E1 00 00 00 00 16 00 00 | 0.....%.........\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 13 9D 01 00 D1 A8 01 44 00 00 00 00 D1 A8 01 | ........D.......\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  44 00 00 00 00 EA B5 AC EC 9B 90 EC 9E 90 00 00 | D...............\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 F3 D4 56 00 30 35 E3 13 FD 03 09 | .......V.05.....\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  B8 5A 73 01 00 EA B5 AC EC 9B 90 EC 9E 90 00 00 | .Zs.............\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 43 42 54 32 00 00 00 00 00 00 00 | .....CBT2.......\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 04 00 01 00 00 00 02 33 56 | ..............3V\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 01 00 00 00 00 00 1D C4 00 00 82 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  43 00 20 80 C4 69 00 00 00 77 00 00 00 69 00 00 | C. ..i...w...i..\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 77 00 00 00                                  | .w...\n"
            , &replyPacket, &memSize
        );
    }
}

void zoneBuilderNormalUnk6(char *commanderName, zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        PacketNormalHeader normalHeader;
        uint32_t unk1;
        uint32_t unk2;
        uint32_t unk3;
        uint32_t unk4;
        uint32_t unk5;
        uint8_t  unk6[16*3+8];
        uint8_t  unk7;
        char commanderName[COMMANDER_NAME_SIZE];
        uint8_t unk8[80];
    } replyPacket;
    #pragma pack(pop)

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        size_t memSize = sizeof(replyPacket);
        dumpToMem(
            "[11:10:22][           ToSClient:                     dbgBuffer]  30 0D FF FF FF FF E9 00 C9 00 00 00 85 07 00 00 | 0...............\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  19 51 00 00 43 22 00 00 1C 08 00 00 FF FF FF FF | .Q..C...........\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 01 43 42 54 32 00 00 00 | .........CBT2...\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 20 04 00 00 00 00 | .......... .....\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 02 33 00 00 00 00 00 00                      | ..3......\n"
            , &replyPacket, &memSize
        );

        strncpy(replyPacket.commanderName, commanderName, sizeof(replyPacket.commanderName));
    }
}

void zoneBuilderFaction(zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        // not yet implemented
        NpcId_t npcId;
        uint32_t unk1;
    } replyPacket;

   (void) replyPacket;
    #pragma pack(pop)

    // PacketType packetType = ZC_FACTION;
    // CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);
    // BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        size_t memSize;
        void *memory = dumpToMem(
            "[11:10:22][           ToSClient:                     dbgBuffer]  6B 0D FF FF FF FF 48 01 00 00 CE 18 27 00       | k.....H.....'."
            , NULL, &memSize
        );

        zmsg_add(replyMsg, zframe_new(memory, memSize));
    }
}

void zoneBuilderEnterMonster(zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        // not yet implemented
    } replyPacket;
   (void) replyPacket;
    #pragma pack(pop)

    // PacketType packetType = ZC_ENTER_MONSTER;
    // CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);
    // BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        size_t memSize;
        void *memory = dumpToMem(
            "[03:07:36][main.c:56 in CNetUsr__PacketHandler_1]  BF 0B FF FF FF FF 84 00 48 01 00 00 00 40 23 C4 | ........H....@#.\n"
            "[03:07:36][main.c:56 in CNetUsr__PacketHandler_1]  A7 69 82 43 00 40 6E C4 46 0B 51 32 00 00 80 BF | .i.C.@n.F.Q2....\n"
            "[03:07:36][main.c:56 in CNetUsr__PacketHandler_1]  02 00 21 00 00 00 21 00 00 00 00 00 00 00 00 00 | ..!...!.........\n"
            "[03:07:36][main.c:56 in CNetUsr__PacketHandler_1]  30 4E 00 00 58 02 00 00 21 00 00 00 00 00 69 6F | 0N..X...!.....io\n"
            "[03:07:36][main.c:56 in CNetUsr__PacketHandler_1]  01 00 00 00 00 00 80 3F 00 00 00 00 08 00 00 00 | .......?........\n"
            "[03:07:36][main.c:56 in CNetUsr__PacketHandler_1]  28 00 00 00 06 00 0A 00 EA B2 BD EA B3 84 EB B3 | ................\n"
            "[03:07:36][main.c:56 in CNetUsr__PacketHandler_1]  91 00 01 00 00 11 00 53 49 41 55 5F 46 52 4F 4E | .......SIAU_FRON\n"
            "[03:07:36][main.c:56 in CNetUsr__PacketHandler_1]  5F 4E 50 43 5F 30 31 00 01 00 00 01 00 00 07 00 | _NPC_01.........\n"
            "[03:07:36][main.c:56 in CNetUsr__PacketHandler_1]  00 00 A0 42                                     | ...B\n"
            , NULL, &memSize
        );

        zmsg_add(replyMsg, zframe_new(memory, memSize));
    }
}

void zoneBuilderSkillAdd(zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        // not yet implemented
    } replyPacket;
   (void) replyPacket;
    #pragma pack(pop)

    // PacketType packetType = ZC_SKILL_ADD;
    // CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);
    // BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        size_t memSize;
        void *memory = dumpToMem(
            "[11:10:22][           ToSClient:                     dbgBuffer]  32 0C FF FF FF FF A2 00 00 00 00 00 00 00 00 00 | 2...............\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 03 00 00 00 78 00 | ..............x.\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  6C F5 00 00 00 00 00 00 1C F5 55 01 00 00 80 3F | l.........U....?\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  48 01 00 00 96 43 46 01 00 00 00 00 4C 01 00 00 | H....CF.....L...\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  C8 42 4E 01 00 00 0C 42 4F 01 00 00 60 41 76 01 | .BN....BO...`Av.\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 62 01 00 00 0C 42 75 01 00 00 00 00 | ....b....Bu.....\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  71 01 00 00 00 00 70 01 00 00 80 3F 9B 01 00 00 | q.....p....?....\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 9D 01 00 00 00 00 A8 01 00 00 40 40 C6 01 | ............@@..\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 C7 01 00 00 80 3F C8 01 00 00 00 00 | .........?......\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  DF 18 00 00 00 00 D4 01 00 00 A0 42 D2 01 00 00 | ...........B....\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00                                           | ..\n"
            , NULL, &memSize
        );

        zmsg_add(replyMsg, zframe_new(memory, memSize));
    }
}

void zoneBuilderLoginTime(zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint64_t now;
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = ZC_LOGIN_TIME;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        serverPacketHeaderInit(&replyPacket.header, packetType);
        replyPacket.now = getFileTime();
    }
}

void zoneBuilderStamina(Stamina_t stamina, zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        Stamina_t stamina;
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = ZC_STAMINA;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        replyPacket.header.type = ZC_STAMINA;
        replyPacket.stamina = stamina;
    }
}

void zoneBuilderAddStamina(Stamina_t stamina, zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        Stamina_t stamina;
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = ZC_ADD_STAMINA;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        replyPacket.header.type = ZC_ADD_STAMINA;
        replyPacket.stamina = stamina;
    }
}

void zoneBuilderUpdateSP(PcId_t targetPcId, Sp_t sp, zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        PcId_t pcId;
        Sp_t sp;
        uint8_t unk1;
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = ZC_UPDATE_SP;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        replyPacket.header.type = ZC_UPDATE_SP;
        replyPacket.pcId = targetPcId;
        replyPacket.sp = sp;
        replyPacket.unk1 = 0;
    }
}

void zoneBuilderUpdateAllStatus(
    PcId_t targetPcId,
    Hp_t currentHp, Hp_t maxHp,
    Sp_t currentSp, Sp_t maxSp,
    zmsg_t *replyMsg)
{
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        PcId_t pcId;
        Hp_t currentHp;
        Hp_t maxHp;
        Sp_t currentSp;
        Sp_t maxSp;
        uint32_t unk1; // 01 is most common, but other values (0x09, 0x0A, 0x0B) seem to work
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = ZC_UPDATE_ALL_STATUS;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        replyPacket.header.type = ZC_UPDATE_ALL_STATUS;
        replyPacket.pcId = targetPcId;
        replyPacket.currentHp = currentHp;
        replyPacket.maxHp = maxHp;
        replyPacket.currentSp = currentSp;
        replyPacket.maxSp = maxSp;
        replyPacket.unk1 = 0x01;
    }
}

void zoneBuilderPCLevelUp(PcId_t targetPcId, CommanderLevel_t level, zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        PcId_t pcId;
        CommanderLevel_t level;
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = ZC_PC_LEVELUP;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        replyPacket.header.type = ZC_PC_LEVELUP;
        replyPacket.pcId = targetPcId;
        replyPacket.level = level;
    }
}

void zoneBuilderAddHp(PcId_t targetPcId, Hp_t currentHp, Hp_t maxHp, zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        PcId_t pcId;
        Hp_t currentHp;
        Hp_t maxHp; //? not sure
        uint32_t unk1; // This might be some encoding for the source of the hp?
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = ZC_ADD_HP;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        replyPacket.header.type = ZC_ADD_HP;
        replyPacket.pcId = targetPcId;
        replyPacket.currentHp = currentHp;
        replyPacket.maxHp = maxHp;
        replyPacket.unk1 = 1;
    }
}

void zoneBuilderBuffList(PcId_t targetPcId, zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint16_t unk1;
        PcId_t pcId;
        uint8_t  unk2;
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = ZC_BUFF_LIST;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        size_t memSize = sizeof(replyPacket);
        dumpToMem(
            "[11:10:22][           ToSClient:                     dbgBuffer]  E2 0B FF FF FF FF 0D 00 5A 73 01 00 00          | ........Zs...\n"
            , &replyPacket, &memSize
        );

        replyPacket.pcId = targetPcId;
    }
}

void zoneBuilderLeave(PcId_t targetPcId, zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        PcId_t pcId;
        uint16_t unk1;
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = ZC_LEAVE;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        replyPacket.header.type = ZC_LEAVE;
        replyPacket.pcId = targetPcId;
        replyPacket.unk1 = 1;
    }
}

void zoneBuilderCampInfo(uint64_t accountId, zmsg_t *replyMsg) {}

void zoneBuilderEnterPc(Commander *commander, zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        PcId_t pcId; // A65F0B00
        PositionXYZ position; // 00001DC4, 00008243, 002080C4
        float unk1; // 0000803F
        uint32_t unk2; // 00000000
        uint16_t unk3; // 0000
        SocialInfoId_t socialInfoId; // EE2500003C010000
        uint8_t pose; // 00
        float moveSpeed; // 0000F841
        uint32_t unk5; // 00000000
        uint32_t currentHP; // EE00
        uint32_t maxHP; // EE00
        uint16_t currentSP; // 6900
        uint16_t maxSP; // 6900
        uint32_t currentStamina; // A8610000
        uint32_t maxStamina; // A8610000
        uint8_t unk6; // 00
        uint16_t unk7; // 0000
        uint32_t titleAchievmentId; // 24B42B1B
        uint32_t unk9; // FFFFFFFF
        uint8_t unk10; // 00
        CommanderAppearanceCPacket appearance;
        uint8_t partyName[48+1]; // "None"
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = ZC_ENTER_PC;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        serverPacketHeaderInit(&replyPacket.header, packetType);
        replyPacket.pcId = commander->pcId;
        replyPacket.position = commander->pos;
        replyPacket.unk1 = 1.0f;
        replyPacket.unk2 = 0;
        replyPacket.unk3 = 0;
        replyPacket.socialInfoId = commander->socialInfoId;
        replyPacket.pose = 0;
        replyPacket.moveSpeed = 31.0f; // TODO : store this in CommanderSession
        replyPacket.currentHP = commander->currentHP;
        replyPacket.maxHP = commander->maxHP;
        replyPacket.currentSP = commander->currentSP;
        replyPacket.maxSP = commander->maxSP;
        replyPacket.currentStamina = commander->currentStamina;
        replyPacket.maxStamina = commander->maxStamina;
        replyPacket.unk6 = 0;
        replyPacket.unk7 = 0;
        replyPacket.titleAchievmentId = SWAP_UINT32(0xA1860100); // ICBT, "Hunter"
        replyPacket.unk9 = -1;
        replyPacket.unk10 = 0;
        commanderAppearanceCPacketInit(&replyPacket.appearance,
            commander->familyName, commander->commanderName,
            commander->accountId, commander->classId, commander->jobId,
            commander->gender, commander->level, commander->hairId, commander->pose);
        strncpy(replyPacket.partyName, "None", sizeof(replyPacket.partyName));
    }
}

void zoneBuilderObjectProperty(ActorId_t actorId, zmsg_t *replyMsg) {

    size_t propsSize = 10;

    #pragma pack(push, 1)
    typedef struct TempProp {
        uint16_t propId;
        float value;
    } tempProp;
    #pragma pack(pop)

    tempProp props[propsSize];

    props[0].propId = SWAP_UINT16(0x8E10); // Lv
    props[0].value = 10;

    props[1].propId = SWAP_UINT16(0xA910); // MSPD (movement speed)
    props[1].value = 31;

    props[2].propId = SWAP_UINT16(0xAD10); // DEX
    props[2].value = 0;

    props[3].propId = SWAP_UINT16(0x9410); // MNA
    props[3].value = 3;

    props[4].propId = SWAP_UINT16(0xAB10); // INT
    props[4].value = 4;

    props[5].propId = SWAP_UINT16(0x9210); // CON
    props[5].value = 7;

    props[6].propId = SWAP_UINT16(0xAA10); // STR
    props[6].value = 6;

    props[7].propId = SWAP_UINT16(0x8311); // LUCK
    props[7].value = 1;

    props[8].propId = SWAP_UINT16(0x3311); // MSP
    props[8].value = 121;

    props[9].propId = SWAP_UINT16(0x2911); // MHP
    props[9].value = 238;

    #pragma pack(push, 1)
    struct {
        // not yet implemented
        VariableSizePacketHeader variableSizeHeader;
        ActorId_t actorId;
        tempProp properties[propsSize];
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = ZC_OBJECT_PROPERTY;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        variableSizePacketHeaderInit(&replyPacket.variableSizeHeader, packetType, sizeof(replyPacket));
        replyPacket.actorId = actorId;
        for (size_t i = 0; i < propsSize; i++) {
            replyPacket.properties[i] = props[i];
        }
    }
}

void zoneBuilderAddonMsg(zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        // not yet implemented
    } replyPacket;
   (void) replyPacket;
    #pragma pack(pop)

    // PacketType packetType = ZC_ADDON_MSG;
    // CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);
    // BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        size_t memSize;
        void *memory = dumpToMem(
            "[11:10:23][           ToSClient:                     dbgBuffer]  4F 0C FF FF FF FF 1F 00 11 00 00 00 00 00 4B 45 | O.............KE\n"
            "[11:10:23][           ToSClient:                     dbgBuffer]  59 42 4F 41 52 44 5F 54 55 54 4F 52 49 41 4C    | YBOARD_TUTORIAL"
            , NULL, &memSize
        );

        zmsg_add(replyMsg, zframe_new(memory, memSize));
    }
}

void zoneBuilderNormalUnk5(zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        // not yet implemented
    } replyPacket;

   (void) replyPacket;
    #pragma pack(pop)

    // BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        size_t memSize;
        void *memory = dumpToMem(
            "[03:07:36][main.c:56 in CNetUsr__PacketHandler_1]  33 0D FF FF FF FF 1D 00 21 01 00 00 8D FA 0D 00 | 3.......!.......\n"
            "[03:07:36][main.c:56 in CNetUsr__PacketHandler_1]  7B A7 CA C0 60 C3 C8 C0 00 44 20 00 00          | {...`....D ..\n"
          , NULL, &memSize
        );

        zmsg_add(replyMsg, zframe_new(memory, memSize));
    }
}

void zoneBuilderNormalUnk4(SocialInfoId_t socialInfoId, zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        // not yet implemented
        PacketNormalHeader normalHeader;
        SocialInfoId_t socialInfoId;
    } replyPacket, *memory;

   (void) replyPacket;
    #pragma pack(pop)

    // BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        size_t memSize;
        memory = dumpToMem(
            "[03:07:36][main.c:56 in CNetUsr__PacketHandler_1]  33 0D FF FF FF FF 2C 00 E6 00 00 00 EE 25 00 00 | 3.....,......%..\n"
            "[03:07:36][main.c:56 in CNetUsr__PacketHandler_1]  3C 01 00 00 01 00 00 00 01 00 00 00 A1 0F 01 00 | <...............\n"
            "[03:07:36][main.c:56 in CNetUsr__PacketHandler_1]  00 00 00 00 80 1D 61 60 E7 D2 D0 01             | ......a`....\n"
          , NULL, &memSize
        );

        memory->socialInfoId = socialInfoId;
        zmsg_add(replyMsg, zframe_new(memory, memSize));
    }
}

void zoneBuilderNormalUnk3(zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        // not yet implemented
    } replyPacket;
   (void) replyPacket;
    #pragma pack(pop)

    // BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        size_t memSize;
        void *memory = dumpToMem(
            "[03:07:36][main.c:56 in CNetUsr__PacketHandler_1]  33 0D FF FF FF FF 10 00 D9 00 00 00 00 00 00 00 | 3...............\n"
          , NULL, &memSize
        );

        zmsg_add(replyMsg, zframe_new(memory, memSize));
    }
}

void zoneBuilderNormalUnk2(zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        // not yet implemented
    } replyPacket;

   (void) replyPacket;
    #pragma pack(pop)

    // BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        size_t memSize;
        void *memory = dumpToMem(
            "[03:07:36][main.c:56 in CNetUsr__PacketHandler_1]  33 0D FF FF FF FF 18 00 CA 00 00 00 EE 25 00 00 | 3............%..\n"
            "[03:07:36][main.c:56 in CNetUsr__PacketHandler_1]  3C 01 00 00 00 00 00 00                         | <.......\n"
          , NULL, &memSize
        );

        zmsg_add(replyMsg, zframe_new(memory, memSize));
    }
}

void zoneBuilderNormalUnk1(zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        // not yet implemented
    } replyPacket;

   (void) replyPacket;
    #pragma pack(pop)

    // BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        size_t memSize;
        void *memory = dumpToMem(
            "[03:07:36][main.c:56 in CNetUsr__PacketHandler_1]  33 0D FF FF FF FF 11 00 97 00 00 00 00 00 00 00 | 3...............\n"
            "[03:07:36][main.c:56 in CNetUsr__PacketHandler_1]  00                                              | .\n"
          , NULL, &memSize
        );

        zmsg_add(replyMsg, zframe_new(memory, memSize));
    }
}

void zoneBuilderStartGame(
    float timeMultiplier,
    float serverAppTimeOffset,
    float globalAppTimeOffset,
    double serverDateTime,
    zmsg_t *replyMsg)
{
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        float timeMultiplier;
        float serverAppTimeOffset;
        float globalAppTimeOffset;
        double serverDateTime;
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = ZC_START_GAME;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        serverPacketHeaderInit(&replyPacket.header, packetType);
        replyPacket.timeMultiplier = timeMultiplier;
        replyPacket.serverAppTimeOffset = serverAppTimeOffset;
        replyPacket.globalAppTimeOffset = globalAppTimeOffset;
        replyPacket.serverDateTime = serverDateTime;
    }
}

void zoneBuilderQuickSlotList(zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint32_t unk1;
        uint16_t unk2;
    } replyPacket;
   (void) replyPacket;
    #pragma pack(pop)

    PacketType packetType = ZC_QUICK_SLOT_LIST;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        serverPacketHeaderInit(&replyPacket.header, packetType);
        replyPacket.unk1 = SWAP_UINT32(0x0C000000); // ICBT
        replyPacket.unk2 = 0; // ICBT
    }
}

void zoneBuilderCooldownList(SocialInfoId_t socialInfoId, zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint16_t unk1;
        SocialInfoId_t socialInfoId;
        uint32_t unk4;
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = ZC_COOLDOWN_LIST;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        serverPacketHeaderInit(&replyPacket.header, packetType);
        replyPacket.unk1 = 0x14; // ICBT
        replyPacket.socialInfoId = socialInfoId;
        replyPacket.unk4 = 0;
    }
}

void zoneBuilderAbilityList(PcId_t targetPcId, zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        VariableSizePacketHeader variableSizeHeader;
        PcId_t pcId;
        uint16_t entriesCount;
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = ZC_ABILITY_LIST;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        size_t memSize = sizeof(replyPacket);
        dumpToMem(
            "[03:07:36][main.c:56 in CNetUsr__PacketHandler_1]  35 0C FF FF FF FF 0E 00 A6 5F 0B 00 00 00       | 5........_....\n"
            , &replyPacket, &memSize
        );

        variableSizePacketHeaderInit(&replyPacket.variableSizeHeader, packetType, sizeof(replyPacket));
        replyPacket.pcId = targetPcId;
        replyPacket.entriesCount = 0;
    }
}

void zoneBuilderSkillList(PcId_t targetPcId, zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        VariableSizePacketHeader variableSizeHeader;
        PcId_t pcId;
        uint16_t skillCount;
        Zlib zlibData;
    } replyPacket, *memory;
    #pragma pack(pop)

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        size_t memSize;
        memory = dumpToMem(
			"[03:10:49][main.c:30 in writePacketToFile]  33 0C FF FF FF FF AE 01 FE 89 0E 00 08 00 8D FA | \n"
			"[03:10:49][main.c:30 in writePacketToFile]  9C 01 AD D2 3B 48 03 41 14 05 D0 9B 8F 1A 41 C8 | \n"
			"[03:10:49][main.c:30 in writePacketToFile]  2A 1A B4 10 62 21 96 06 2C 2C 44 F6 47 8C 85 D6 | \n"
			"[03:10:49][main.c:30 in writePacketToFile]  7E AA D8 5A 58 4B 0A 59 24 22 B6 22 56 29 B6 8A | \n"
			"[03:10:49][main.c:30 in writePacketToFile]  BF D6 10 41 88 62 B5 60 A3 29 2D C4 42 04 AD D2 | \n"
			"[03:10:49][main.c:30 in writePacketToFile]  28 08 0A BE 79 A3 EE 92 C9 4A 0A A7 78 CC 0E 24 | \n"
			"[03:10:49][main.c:30 in writePacketToFile]  1C EE 7D 9B A3 31 CC 45 80 E3 12 B0 8E 28 E4 79 | \n"
			"[03:10:49][main.c:30 in writePacketToFile]  8E 5D 24 01 47 3F 4D 22 BF 9A 3B A3 3B 50 A5 E9 | \n"
			"[03:10:49][main.c:30 in writePacketToFile]  59 15 BE D7 68 E6 CD 6E 4D BC 4F F0 9C A6 E9 18 | \n"
			"[03:10:49][main.c:30 in writePacketToFile]  8F F4 5E B0 EF 69 3A BA C9 EF 29 9E 9D 3C 17 79 | \n"
			"[03:10:49][main.c:30 in writePacketToFile]  5E F3 3F 2C D0 DD D1 EF F8 BE C4 EF CB FC B2 B7 | \n"
			"[03:10:49][main.c:30 in writePacketToFile]  15 41 19 C0 11 79 36 B0 46 37 71 A4 C7 35 C8 E3 | \n"
			"[03:10:49][main.c:30 in writePacketToFile]  F6 E4 AE E8 57 AE 21 54 8E 2E 54 29 5B A8 3C 4B | \n"
			"[03:10:49][main.c:30 in writePacketToFile]  A8 1A AC 1A B4 7C 95 CB 2A A0 B5 6A D6 6A 56 C1 | \n"
			"[03:10:49][main.c:30 in writePacketToFile]  50 55 EF 2B C0 3E 39 CA AC 62 12 86 C6 00 91 92 | \n"
			"[03:10:49][main.c:30 in writePacketToFile]  66 92 CA 28 CD 08 95 66 FE 95 55 31 A0 FA 8F AC | \n"
			"[03:10:49][main.c:30 in writePacketToFile]  5E FB 3A B0 43 98 43 56 FD 74 F7 C9 DD 65 84 2A | \n"
			"[03:10:49][main.c:30 in writePacketToFile]  FF C2 AA 0C AB C2 B2 DA 55 54 61 59 F5 DB CD 59 | \n"
			"[03:10:49][main.c:30 in writePacketToFile]  C9 06 2B B6 68 B0 90 95 0D 56 E7 E3 AC B2 03 59 | \n"
			"[03:10:49][main.c:30 in writePacketToFile]  D5 BB 64 56 DC A0 33 15 68 50 DD AB 34 37 A8 29 | \n"
			"[03:10:49][main.c:30 in writePacketToFile]  AA B0 BD 4A 9B AD 55 40 70 AF 6E 0B 51 6C 53 56 | \n"
			"[03:10:49][main.c:30 in writePacketToFile]  07 AC 1A 91 15 22 11 17 0D 4A D5 79 D6 DF 2B 55 | \n"
			"[03:10:49][main.c:30 in writePacketToFile]  25 B7 3D A1 A8 C2 B2 72 8D 76 54 93 C3 51 14 C9 | \n"
			"[03:10:49][main.c:30 in writePacketToFile]  62 06 B2 F2 E8 FB 57 F5 70 C2 0D CA 6D 0F 53 5D | \n"
			"[03:10:49][main.c:30 in writePacketToFile]  9A FE B6 FF BD 57 9E B2 ED B2 C1 01 D1 A0 51 FA | \n"
			"[03:10:49][main.c:30 in writePacketToFile]  6E F0 ED 29 C2 2A 8B 55 E3 B4 E7 E2 F4 FA 59 35 | \n"
			"[03:10:49][main.c:30 in writePacketToFile]  EA 6D A8 6E DA 56 7D 84 A8 6A 56 B0 C1 2F       | \n"
          , NULL, &memSize
        );
        memory->pcId = targetPcId;

        /* Uncompressed :
            00000000 00000000 14000000 780022EA 00000000 B2B94B37 55010000 803F4801 00000000 46010000 00004C01 0000C842 4E010000 E0414F01 00006041 76010000 00006201 0000F041 75010000 00007101 00000000 70010000 803F9B01 00000000 9D010000 0000A801 00004040 C6010000 0000C701 0000803F C8010000 0000DF18 00000000 D4010000 C842D201 00000000
            00000000 00000000 03000000 780002A8 2C010000 6E2F348F 55010000 803F4801 00009643 46010000 00004C01 0000C842 4E010000 0C424F01 00006041 76010000 00006201 00000C42 75010000 00007101 00000000 70010000 803F9B01 00000000 9D010000 0000A801 00004040 C6010000 0000C701 0000803F C8010000 0000DF18 00000000 D4010000 A042D201 00000000
            00000000 00000000 64000000 78000060 00000000 267FFE42 55010000 803F4801 00000000 46010000 00004C01 0000C842 4E010000 20424F01 0000C041 76010000 00006201 0000C841 75010000 00007101 00000000 70010000 803F9B01 00000000 9D010000 0000A801 00004040 C6010000 0000C701 0000803F C8010000 0000DF18 00000000 D4010000 C842D201 00000000
            71D90000 A39F0100 449C0000 7800A751 00000000 325FBA3E 55010000 803F4801 00F05246 46010000 00004C01 0000C842 4E010000 00004F01 00006041 76010000 803F6201 00000000 75010000 00007101 00000000 70010000 803F9B01 00002041 9D010000 0000A801 00008040 C6010000 7A43C701 0000803F C8010000 0000DF18 00000000 D4010000 0000D201 00000000
        */
        replyPacket.pcId = targetPcId;
    }
}

void zoneBuilderItemEquipList(Inventory *inventory, zmsg_t *replyMsg) {

    /**
     * @brief ItemCPacket is the client packet structure of an equiped item
     */
    #define DEFINE_EquippedItemCPacket(attrSize) \
    typedef struct EquippedItemCPacket {         \
        ItemId_t id;                             \
        uint16_t sizeOfAttributes;               \
        uint16_t unk1;                           \
        ActorId_t uid;                           \
        ItemEquipmentSlot_t eqSlot;              \
        uint8_t unk2;                            \
        uint16_t unk3;                           \
        uint32_t unk4;                           \
        uint8_t properties[attrSize];            \
    }   EquippedItemCPacket;

    size_t itemPropertiesSize[EQSLOT_COUNT];
    size_t itemsPacketSize = 0;

    for (int eqSlot = 0; eqSlot < EQSLOT_COUNT; eqSlot++) {

        ItemEquipable *eqItem = inventory->equippedItems[eqSlot];
        // get property size
        size_t attrSize = eqItem ? itemGetPropertiesCPacketSize(&eqItem->item) : 0;

        #pragma pack(push, 1)
        DEFINE_EquippedItemCPacket(attrSize);
        #pragma pack(pop)

        itemPropertiesSize[eqSlot] = attrSize;
        itemsPacketSize += sizeof(EquippedItemCPacket);
    }

    #pragma pack(push, 1)
    struct EquipmentListPacket {
        VariableSizePacketHeader variableSizeHeader;
        uint8_t items[itemsPacketSize];
    } replyPacket;
    #pragma pack(pop)

    PacketStream packetStream;
    packetStreamInit(&packetStream, &replyPacket);

    PacketType packetType = ZC_ITEM_EQUIP_LIST;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        variableSizePacketHeaderInit(&replyPacket.variableSizeHeader, packetType, sizeof(replyPacket));

        // we want to start writing at the offset of items
        packetStreamAddOffset(&packetStream, offsetof(struct EquipmentListPacket, items));

        for (int eqSlot = 0; eqSlot < EQSLOT_COUNT; eqSlot++) {

            ItemEquipable *eqItem = inventory->equippedItems[eqSlot];

            size_t attrSize = itemPropertiesSize[eqSlot];

            #pragma pack(push, 1)
            DEFINE_EquippedItemCPacket(attrSize);
            #pragma pack(pop)

            EquippedItemCPacket *eqItemPkt = packetStreamGetCurrentBuffer(&packetStream);

            eqItemPkt->id = eqItem ? itemGetId((Item *) eqItem) : inventoryGetEquipmentEmptySlot(eqSlot);
            eqItemPkt->sizeOfAttributes = attrSize;
            eqItemPkt->unk1 = 0;
            eqItemPkt->uid = eqItem ? actorGetUId((Actor *) eqItem) : 0;
            eqItemPkt->eqSlot = eqSlot;
            eqItemPkt->unk2 = 0;
            eqItemPkt->unk3 = 0;
            eqItemPkt->unk4 = 0;

            // fill property buffer
            packetStreamAddOffset(&packetStream, offsetof(struct EquippedItemCPacket, properties));

            // write in the buffer
            if (attrSize > 0) {
                dbg("Get item property UID : %llx", actorGetUId((Actor *) eqItem));
                itemPropertiesGetCPacket(&eqItem->item, &packetStream);
            }
        }
    }

}

void zoneBuilderStartInfo(zmsg_t *replyMsg) {

    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint16_t unk1;
        uint32_t unk2;
        uint16_t unk3;
        uint32_t unk4;
        uint32_t unk5;
        uint16_t unk6;
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = ZC_START_INFO;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        serverPacketHeaderInit(&replyPacket.header, packetType);
        replyPacket.unk1 = 0x18;
        replyPacket.unk2 = 1;
        replyPacket.unk3 = 0xFA1;
        replyPacket.unk4 = 0;
        replyPacket.unk5 = 0;
        replyPacket.unk6 = 1;
    }
}

void zoneBuilderSetPos(PcId_t targetPcId, PositionXYZ *position, zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        PcId_t pcId;
        PositionXYZ position;
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = ZC_SET_POS;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        serverPacketHeaderInit(&replyPacket.header, packetType);
        replyPacket.pcId = targetPcId;
        memcpy(&replyPacket.position, position, sizeof(PositionXYZ));
    }
}

void zoneBuilderGuestPageMap(zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint16_t unk1;
        uint32_t unk2;
        uint8_t  unk3;
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = ZC_GUESTPAGE_MAP;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        serverPacketHeaderInit(&replyPacket.header, packetType);
        replyPacket.unk1 = 0xD;
        replyPacket.unk2 = 1;
        replyPacket.unk3 = 0;
    }
}

void zoneBuilderMyPageMap(zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint16_t unk1;
        uint32_t unk2;
        uint8_t  unk3;
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = ZC_MYPAGE_MAP;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        serverPacketHeaderInit(&replyPacket.header, packetType);
        replyPacket.unk1 = 0xD;
        replyPacket.unk2 = 1;
        replyPacket.unk3 = 0;
    }
}

void zoneBuilderHelpList(zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        // not yet implemented
    } replyPacket;

   (void) replyPacket;
    #pragma pack(pop)

    // BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        size_t memSize;
        void *memory = dumpToMem(
            "[03:07:36][main.c:56 in CNetUsr__PacketHandler_1]  47 0D FF FF FF FF 48 00 0C 00 00 00 01 00 00 00 | G.....H.........\n"
            "[03:07:36][main.c:56 in CNetUsr__PacketHandler_1]  00 02 00 00 00 00 03 00 00 00 00 04 00 00 00 00 | ................\n"
            "[03:07:36][main.c:56 in CNetUsr__PacketHandler_1]  09 00 00 00 00 0A 00 00 00 00 0E 00 00 00 00 0F | ................\n"
            "[03:07:36][main.c:56 in CNetUsr__PacketHandler_1]  00 00 00 00 1A 00 00 00 00 1C 00 00 00 00 27 00 | ..............'.\n"
            "[03:07:36][main.c:56 in CNetUsr__PacketHandler_1]  00 00 00 28 00 00 00 00                         | ...(....\n"
          , NULL, &memSize
        );

        zmsg_add(replyMsg, zframe_new(memory, memSize));
    }
}

void zoneBuilderNpcStateList(zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint16_t unk1;
        uint32_t unk2;
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = ZC_NPC_STATE_LIST;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        serverPacketHeaderInit(&replyPacket.header, packetType);
        replyPacket.unk1 = 0xC;
        replyPacket.unk2 = 0;
    }
}

void zoneBuilderUiInfoList(zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint16_t unk1;
        uint32_t unk2;
        uint32_t unk3;
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = ZC_UI_INFO_LIST;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        serverPacketHeaderInit(&replyPacket.header, packetType);
        replyPacket.unk1 = 0x10;
        replyPacket.unk2 = 0;
        replyPacket.unk3 = 0;
    }
}

void zoneBuilderChatMacroList(zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint16_t unk1;
        uint32_t unk2;
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = ZC_CHAT_MACRO_LIST;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        serverPacketHeaderInit(&replyPacket.header, packetType);
        replyPacket.unk1 = 0xC;
        replyPacket.unk2 = 0;
    }
}

void zoneBuilderAchievePointList(zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint16_t unk1;
        uint32_t unk2;
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = ZC_ACHIEVE_POINT_LIST;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        serverPacketHeaderInit(&replyPacket.header, packetType);
        replyPacket.unk1 = 0xC;
        replyPacket.unk2 = 0;
    }
}

void zoneBuilderSkillmapList(zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint16_t unk1;
        uint32_t unk2;
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = ZC_SKILLMAP_LIST;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        serverPacketHeaderInit(&replyPacket.header, packetType);
        replyPacket.unk1 = 0xC;
        replyPacket.unk2 = 0;
    }
}

void zoneBuilderOptionList(zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint16_t unk1;
        uint8_t unk2;
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = ZC_OPTION_LIST;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        serverPacketHeaderInit(&replyPacket.header, packetType);
        replyPacket.unk1 = 9;
        replyPacket.unk2 = 0;
    }
}

void zoneBuilderMyPCEnter(PositionXYZ *position, zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        PositionXYZ position;
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = ZC_MYPC_ENTER;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        serverPacketHeaderInit(&replyPacket.header, packetType);
        memcpy(&replyPacket.position, position, sizeof(PositionXYZ));
    }
}

void zoneBuilderMoveSpeed(PcId_t targetPcId, float movementSpeed, zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        PcId_t pcId;
        float movementSpeed;
        uint32_t unk1;
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = ZC_MOVE_SPEED;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        serverPacketHeaderInit(&replyPacket.header, packetType);

        replyPacket.pcId = targetPcId;
        replyPacket.movementSpeed = movementSpeed;
        replyPacket.unk1 = 0;
    }
}

void zoneBuilderChat(Commander *commander, uint8_t *chatText, zmsg_t *replyMsg) {

    size_t chatTextLen = strlen(chatText) + 1;

    #pragma pack(push, 1)
    struct {
        VariableSizePacketHeader variableSizeHeader;
        PcId_t pcId;
        uint8_t familyName[COMMANDER_FAMILY_NAME_SIZE];
        uint8_t commanderName[COMMANDER_NAME_SIZE+1];
        uint8_t unk1;
        CommanderJobId_t jobId;
        uint32_t unk2;
        CommanderGender_t gender;
        CommanderHairId_t hairId;
        uint16_t unk3;
        ItemId_t headTop;
        float displayTime;
        uint8_t chatText[chatTextLen];
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = ZC_CHAT;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        variableSizePacketHeaderInit(&replyPacket.variableSizeHeader, packetType, sizeof(replyPacket));
        replyPacket.pcId = commander->pcId;
        memcpy(replyPacket.familyName, commander->familyName, sizeof(replyPacket.familyName));
        memcpy(replyPacket.commanderName, commander->commanderName, sizeof(replyPacket.commanderName));
        replyPacket.unk1 = 0x4F;
        replyPacket.jobId = commander->jobId;
        replyPacket.unk2 = 1;
        replyPacket.gender = commander->gender;
        replyPacket.hairId = commander->hairId;
        replyPacket.unk3 = 0;
        replyPacket.headTop = itemGetId((Item *) commander->inventory.equippedItems[EQSLOT_HAT]);
        replyPacket.displayTime = 0;
        memcpy(replyPacket.chatText, chatText, sizeof(replyPacket.chatText));
    }
}

void zoneBuilderJump(PcId_t targetPcId, float height, zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        PcId_t pcId;
        float height;
        uint32_t unk1;
        uint8_t unk2;
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = ZC_JUMP;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        serverPacketHeaderInit(&replyPacket.header, packetType);
        replyPacket.pcId = targetPcId;
        replyPacket.height = height;
        replyPacket.unk1 = 0;
        replyPacket.unk2 = 1;
    }
}

void zoneBuilderConnectOk(
    GameMode_t gameMode,
    AccountSessionPrivileges_t accountPrivileges,
    Commander *commander,
    zmsg_t *replyMsg)
{
    #pragma pack(push, 1)
    struct {
        VariableSizePacketHeader variableSizeHeader;
        GameMode_t gameMode; // 0 = NormalMode, 1 = SingleMode
        uint32_t unk1;
        AccountSessionPrivileges_t accountPrivileges;
        uint16_t unk2;
        uint64_t unk3;
        uint8_t markers[3];
        uint8_t passport[41];
        PcId_t pcId;
        uint32_t unk5;

        CommanderAppearanceCPacket appearance;
        PositionXYZ pos;
        Xp_t currentXP;
        Xp_t maxXP;
        PcId_t _pcId;
        SocialInfoId_t socialInfoId;
        CommanderId_t commanderId;
        Hp_t currentHP;
        Hp_t maxHP;
        Sp_t currentSP;
        Sp_t maxSP;
        Stamina_t currentStamina;
        Stamina_t maxStamina;
        uint16_t unk6;
        uint16_t unk7;
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = ZC_CONNECT_OK;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        variableSizePacketHeaderInit(&replyPacket.variableSizeHeader, packetType, sizeof(replyPacket));

        replyPacket.gameMode = gameMode;
        replyPacket.unk1 = SWAP_UINT32(0xCED3B04C); // ICBT
        replyPacket.accountPrivileges = accountPrivileges;
        replyPacket.unk2 = 0xC2; // ICBT
        replyPacket.unk3 = 0; // ICBT
        memcpy(replyPacket.markers, "*\x00*", sizeof(replyPacket.markers));
        memcpy(replyPacket.passport, "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA", sizeof(replyPacket.passport));
        replyPacket.pcId = commander->pcId;
        replyPacket.unk5 = 0; // ICBT
        commanderAppearanceCPacketInit(&replyPacket.appearance,
            commander->familyName, commander->commanderName,
            commander->accountId, commander->classId, commander->jobId, commander->gender,
            commander->level, commander->hairId, commander->pose);
        replyPacket.pos = commander->pos;
        replyPacket.currentXP = commander->currentXP;
        replyPacket.maxXP = commander->maxXP;
        replyPacket._pcId = commander->pcId;
        replyPacket.socialInfoId = commander->socialInfoId;
        replyPacket.commanderId = commander->commanderId;
        replyPacket.currentHP = commander->currentHP;
        replyPacket.maxHP = commander->maxHP;
        replyPacket.currentSP = commander->currentSP;
        replyPacket.maxSP = commander->maxSP;
        replyPacket.currentStamina = commander->currentStamina;
        replyPacket.maxStamina = commander->maxStamina;
    }
}

void zoneBuilderSessionObjects(zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        VariableSizePacketHeader variableSizeHeader;
    } replyPacket;
   (void) replyPacket;
    #pragma pack(pop)

    // PacketType packetType = ZC_SESSION_OBJECTS;
    // CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);
    // BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        size_t memSize;
        void *memory = dumpToMem(
            "[03:07:36][main.c:56 in CNetUsr__PacketHandler_1]  2B 0C FF FF FF FF 12 00 00 00 00 00 8D FA 02 00 | +...............\n"
            "[03:07:36][main.c:56 in CNetUsr__PacketHandler_1]  03 00                                           | ..\n"
            , NULL, &memSize
        );

        zmsg_add(replyMsg, zframe_new(memory, memSize));
    }
}

void zoneBuilderItemInventoryList(Inventory *inventory, zmsg_t *replyMsg) {

    #define DEFINE_InventoryItemCPacket(attrSize)             \
    typedef struct {                                          \
        ItemId_t itemId;                                      \
        uint16_t sizeOfAttributes;                            \
        uint16_t unknown1;                                     \
        ActorId_t itemUId;                                    \
        ItemAmount_t amount;                                  \
        ItemPrice_t price;                                    \
        ItemInventoryIndex_t inventoryIndex;                  \
        uint32_t unknown2;                                     \
        uint8_t properties[attrSize];                         \
    } InventoryItemCPacket;

    // Get inventory count
    size_t inventoryCount = inventoryGetItemsCount(inventory);

    // Calculate size in packet to store all items
    size_t itemPropertiesSize[ITEM_CAT_COUNT][inventoryCount];
    size_t totalSize = 0;

    // Itarate all inventory bags
    for (ItemCategory category = 0; category < ITEM_CAT_COUNT; category++) {

        // Iterate through all items in this bag
        Item *item = inventoryGetFirstItem(inventory, category);
        size_t inventoryIndex = 0;

        while (item) {
            size_t attrSize = item ? itemGetPropertiesCPacketSize(item) : 0;
            itemPropertiesSize[category][inventoryIndex] = attrSize;

            #pragma pack(push, 1)
            DEFINE_InventoryItemCPacket(attrSize);
            #pragma pack(pop)

            totalSize += sizeof(InventoryItemCPacket);

            item = inventoryGetNextItem(inventory, category);
            inventoryIndex++;
        }
    }

    // Populate packet for Items
    uint8_t itemsPacket[totalSize];

    PacketStream packetStream;
    packetStreamInit(&packetStream, itemsPacket);

    // Itarate all inventory bags
    for (ItemCategory category = 0; category < ITEM_CAT_COUNT; category++) {

        // Iterate through all items in this bag
        size_t inventoryIndex = 0;

        for (Item *item = inventoryGetFirstItem(inventory, category);
             item != NULL;
             item = inventoryGetNextItem(inventory, category))
        {
            size_t attrSize = itemPropertiesSize[category][inventoryIndex];

            #pragma pack(push, 1)
            DEFINE_InventoryItemCPacket(attrSize);
            #pragma pack(pop)

            InventoryItemCPacket *inventoryItemPacket = packetStreamGetCurrentBuffer(&packetStream);

            inventoryItemPacket->itemId = itemGetId(item);
            inventoryItemPacket->sizeOfAttributes = attrSize;
            inventoryItemPacket->unknown1 = 0;
            inventoryItemPacket->itemUId = actorGetUId(item);
            inventoryItemPacket->amount = itemGetAmount(item);
            inventoryItemPacket->price = 0;
            inventoryItemPacket->inventoryIndex = inventoryGetBagIndexByActorId(inventory, itemGetCategory(item), actorGetUId(item));
            inventoryItemPacket->unknown2 = 0;

            // fill attribute buffer
            size_t offset = offsetof(InventoryItemCPacket, properties);
            packetStreamAddOffset(&packetStream, offset);

            // write in the buffer
            if (attrSize > 0) {
                itemPropertiesGetCPacket(item, &packetStream);
            }

            inventoryIndex++;
        }
    }

    #pragma pack(push, 1)
    struct InventoryListPacket {
        VariableSizePacketHeader variableSizeHeader;
        uint32_t inventoryCount;
        Zlib zlibData;
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = ZC_ITEM_INVENTORY_LIST;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    replyPacket.inventoryCount = inventoryCount;

    // compress content of packek (items)
    zlibCompress(&replyPacket.zlibData, itemsPacket, sizeof(itemsPacket));
    size_t outPacketSize = ZLIB_GET_COMPRESSED_PACKET_SIZE(&replyPacket.zlibData, sizeof(replyPacket));
    variableSizePacketHeaderInit(&replyPacket.variableSizeHeader, packetType, outPacketSize);

    zmsg_add(replyMsg, zframe_new(&replyPacket, outPacketSize));
}

void zoneBuilderMoveDir(
    PcId_t targetPcId,
    PositionXYZ *position,
    PositionXZ *direction,
    float timestamp,
    zmsg_t *replyMsg)
{
    /*
         pcId       posX     posY     posZ     dirX     dirZ     u1 u2 unk3     time
         4E6E0100   4E1D36C4 E656A143 9DD40444 F304353F F304353F 01 00 00F84101 5341B543
         4E6E0100   402434C4 E656A143 ABCD0644 F304353F F304353F 01 00 00F84101 8D54B543
         4E6E0100   7AE81AC4 4108A543 92AB5C44 1D566733 FFFF7FBF 01 00 00F84101 8EED5A44
         4E6E0100   7AE81AC4 89C9A343 FA915944 1D566733 FFFF7FBF 01 00 00F84101 38F85A44
    */

    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        PcId_t pcId;
        PositionXYZ position;
        PositionXZ direction;
        uint8_t unk1;
        uint8_t unk2;
        uint32_t unk3;
        float timestamp;
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = ZC_MOVE_DIR;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        serverPacketHeaderInit(&replyPacket.header, packetType);
        replyPacket.pcId = targetPcId;
        memcpy(&replyPacket.position, position, sizeof(PositionXYZ));
        memcpy(&replyPacket.direction, direction, sizeof(PositionXZ));
        replyPacket.unk1 = 1;
        replyPacket.unk2 = 0;
        replyPacket.unk3 = SWAP_UINT32(0x00F04101);
        replyPacket.timestamp = timestamp;
    }
}

void zoneBuilderPcMoveStop(
    PcId_t targetPcId,
    PositionXYZ *position,
    PositionXZ *direction,
    float timestamp,
    zmsg_t *replyMsg)
{
    /*
        header        pcId     posX     posY     posZ     u1 dirX     dirZ     time
        590D 3C395F00 32750100 00001DC4 A7698243 002080C4 01 0000803F 00000000 498AC742
        590D 6B3A5F00 5A730100 5FBA8BC4 74768243 9B4C03C4 01 F30435BF F304353F 91698943
        590D 3F435F00 5A730100 67408EC4 74768243 3FE20BC4 01 FEFF7FBF 1C56E7B3 F8439043
        590D E4435F00 5A730100 67408EC4 74768243 17060AC4 01 00000000 FEFF7F3F 56B59043
        590D 66445F00 5A730100 0BF18DC4 74768243 17060AC4 01 FEFF7F3F 1C56E733 BD179143
    */

    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        PcId_t pcId;
        PositionXYZ position;
        uint8_t unk1;
        PositionXZ direction;
        float timestamp;
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = ZC_PC_MOVE_STOP;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        serverPacketHeaderInit(&replyPacket.header, packetType);
        replyPacket.pcId = targetPcId;
        memcpy(&replyPacket.position, position, sizeof(PositionXYZ));
        memcpy(&replyPacket.direction, direction, sizeof(PositionXZ));
        replyPacket.unk1 = 1;
        replyPacket.timestamp = timestamp;
    }
}

void zoneBuilderRotateHead(PcId_t pcId, PositionXZ *pos, zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        PcId_t pcId;
        float unk1; //x?
        float unk2; //z?
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = ZC_HEAD_ROTATE;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        serverPacketHeaderInit(&replyPacket.header, packetType);
        replyPacket.pcId = pcId;
        replyPacket.unk1 = pos->x;
        replyPacket.unk2 = pos->z;
    }
}

void zoneBuilderRotate(PcId_t pcId, PositionXZ *bodyDir, zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        PcId_t pcId;
        PositionXZ bodyDir;
        uint8_t unk3;
        uint8_t unk4;
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = ZC_ROTATE;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        serverPacketHeaderInit(&replyPacket.header, packetType);
        replyPacket.pcId = pcId;
        replyPacket.bodyDir = *bodyDir;
        replyPacket.unk3 = false;
        replyPacket.unk4 = false;
    }
}

void zoneBuilderLogout(zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = ZC_LOGOUT_OK;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        serverPacketHeaderInit(&replyPacket.header, packetType);
    }
}

void zoneBuilderPose(PcId_t pcId, CommanderPose_t poseId, PositionXYZ *pos, PositionXZ *dir, zmsg_t *replyMsg) {

    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        PcId_t pcId;
        CommanderPose_t poseId;
        PositionXYZ pos;
        PositionXZ dir;
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = ZC_POSE;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        serverPacketHeaderInit(&replyPacket.header, packetType);
        replyPacket.pcId = pcId;
        replyPacket.poseId = poseId;
        memcpy(&replyPacket.pos, pos, sizeof(replyPacket.pos));
        memcpy(&replyPacket.dir, dir, sizeof(replyPacket.dir));
    }
}

void zoneBuilderChangeCamera(uint8_t mode, PositionXYZ *pos, float fspd, float ispd, zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint8_t mode; // option 0, 1 or 2 | 0 = reset, 1, 2 = set then update ?
        uint32_t unk;
        PositionXYZ pos;
        float fspd;
        float ispd;
    } replyPacket;
    #pragma pack(pop)

    info("Launched changeCamera packet size: %d", sizeof(replyPacket));
    PacketType packetType = ZC_CHANGE_CAMERA;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        serverPacketHeaderInit(&replyPacket.header, packetType);
        replyPacket.mode = mode;
        replyPacket.unk = 0;
        memcpy(&replyPacket.pos, pos, sizeof(replyPacket.pos));
        replyPacket.fspd = fspd;
        replyPacket.ispd = ispd;
    }
}

void zoneBuilderItemRemove(Item *item, InventoryRemoval removalType, InventoryType inventoryType, zmsg_t *replyMsg) {

    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        ActorId_t uid;
        ItemAmount_t amount;
        InventoryRemoval_t removalType;
        InventoryType_t inventoryType;
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = ZC_ITEM_REMOVE;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        serverPacketHeaderInit(&replyPacket.header, packetType);
        replyPacket.uid = actorGetUId(item);
        replyPacket.amount = item->amount;
        replyPacket.removalType = removalType;
        replyPacket.inventoryType = inventoryType;
    }
}
