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
#include "common/commander/commander.h"
#include "common/commander/inventory.h"
#include "common/actor/item/item.h"

void zoneBuilderRestSit(uint32_t targetPcId, zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint32_t pcId;
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
        uint8_t addType;
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
}

void zoneBuilderSkillReady(
    uint32_t targetPcId,
    uint32_t skillId,
    PositionXYZ *pos1,
    PositionXYZ *pos2,
    zmsg_t *replyMsg)
{
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint32_t pcId;
        uint32_t skillId;
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
        uint32_t unkSchrageId1;
        uint32_t unkSchrageId2;
        uint8_t isUnk1;
        uint8_t isUnk2;
        float timeDelay;
        float unk5;
    } replyPacket;
    #pragma pack(pop)

    /*
        4B010000 25182700 0101 00000000 0000803F
    */
    PacketType packetType = ZC_PLAY_ANI;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        serverPacketHeaderInit(&replyPacket.header, packetType);
        replyPacket.unkSchrageId1 = SWAP_UINT32(0x48010000);
        replyPacket.unkSchrageId2 = SWAP_UINT32(0x271826);
        replyPacket.isUnk1 = 1;
        replyPacket.isUnk2 = 1;
        replyPacket.timeDelay = 0;
        replyPacket.unk5 = SWAP_UINT32(0x0000803F);
    }
}

void zoneBuilderSkillCast(
    uint32_t targetPcId,
    uint32_t skillId,
    PositionXYZ *position1,
    PositionXYZ *position2,
    zmsg_t *replyMsg)
{
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint32_t pcId;
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

void zoneBuilderPlaySkillCastAni(uint32_t targetPcId, PositionXYZ *position, zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint32_t pcId;
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

void zoneBuilderNormalUnk8(uint32_t targetPcId, zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        PacketNormalHeader normalHeader;
        uint32_t pcId;
        uint32_t unk1;
        uint32_t skillId;
        uint32_t skillLevel; // ? not sure
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

void zoneBuilderNormalUnk9(uint32_t targetPcId, zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        PacketNormalHeader normalHeader;
        uint32_t pcId;
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
    uint64_t accountId,
    uint32_t targetPcId,
    char *familyName,
    char *commanderName,
    zmsg_t *replyMsg)
{
    #pragma pack(push, 1)
    struct {
        PacketNormalHeader normalHeader;
        uint32_t unk1;
        uint32_t socialId;
        uint8_t unk2;
        uint64_t accountId;
        uint64_t accountId_2;
        char familyName[COMMANDER_FAMILY_NAME_SIZE];
        uint32_t unk3[3];
        uint32_t pcId;
        char familyName_2[COMMANDER_FAMILY_NAME_SIZE];
        char commanderName[COMMANDER_NAME_SIZE];
        uint8_t unk4[48];
    } replyPacket;
    #pragma pack(pop)

    /*
        300D     FFFFFFFF 2501     E1000000 0016     00 0000  139D0100 D1A80144 00000000 D1A80144 00000000
        EAB5ACEC9B90EC9E9000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
        F3D45600 3035E313 FD0309B8 5A730100
        EAB5ACEC9B90EC9E9000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
        CommanderName
        43425432000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
        00000000 00040001
                                          x        y        z
        00000002 33560000 000000 01000000 00001DC4 00008243 002080C4
        69000000 77000000 69000000 77000000
    */

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

        replyPacket.accountId = accountId;
        replyPacket.accountId_2 = accountId;
        replyPacket.pcId = targetPcId;

        strncpy(replyPacket.familyName, familyName, sizeof(replyPacket.familyName));
        strncpy(replyPacket.familyName_2, familyName, sizeof(replyPacket.familyName_2));
        strncpy(replyPacket.commanderName, commanderName, sizeof(replyPacket.commanderName));
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
            "[11:10:22][           ToSClient:                     dbgBuffer]  BF 0B FF FF FF FF 84 00 48 01 00 00 00 40 23 C4 | ........H....@#.\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  A7 69 82 43 00 40 6E C4 46 0B 51 32 00 00 80 BF | .i.C.@n.F.Q2....\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  02 00 21 00 00 00 21 00 00 00 00 00 00 00 00 00 | ..!...!.........\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  30 4E 00 00 58 02 00 00 21 00 00 00 00 00 69 6F | 0N..X...!.....io\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  01 00 00 00 00 00 80 3F 00 00 00 00 08 00 00 00 | .......?........\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  28 00 00 00 06 00 0A 00 EA B2 BD EA B3 84 EB B3 |(...............\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  91 00 01 00 00 11 00 53 49 41 55 5F 46 52 4F 4E | .......SIAU_FRON\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  5F 4E 50 43 5F 30 31 00 01 00 00 01 00 00 07 00 | _NPC_01.........\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 A0 42                                     | ...B\n"
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

void zoneBuilderStamina(uint32_t stamina, zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint32_t stamina;
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

void zoneBuilderAddStamina(uint32_t stamina, zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint32_t stamina;
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

void zoneBuilderUpdateSP(uint32_t targetPcID, uint32_t sp, zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint32_t pcId;
        uint32_t sp;
        uint8_t  unk1;
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = ZC_UPDATE_SP;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        replyPacket.header.type = ZC_UPDATE_SP;
        replyPacket.pcId = targetPcID;
        replyPacket.sp = sp;
        replyPacket.unk1 = 0;
    }
}

void zoneBuilderUpdateAllStatus(uint32_t targetPcID, uint32_t hp, uint32_t maxHp, uint16_t sp,
                                uint16_t maxSp, zmsg_t *replyMsg)
{
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint32_t pcId;
        uint32_t hp;
        uint32_t maxHp;
        uint16_t sp;
        uint16_t maxSp;
        uint32_t unk1; // 01 is most common, but other values (0x09, 0x0A, 0x0B) seem to work
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = ZC_UPDATE_ALL_STATUS;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        replyPacket.header.type = ZC_UPDATE_ALL_STATUS;
        replyPacket.pcId = targetPcID;
        replyPacket.hp = hp;
        replyPacket.maxHp = maxHp;
        replyPacket.sp = sp;
        replyPacket.maxSp = maxSp;
        replyPacket.unk1 = 0x01;
    }
}

void zoneBuilderPCLevelUp(uint32_t targetPcID, uint32_t level, zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint32_t pcId;
        uint32_t level;
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = ZC_PC_LEVELUP;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        replyPacket.header.type = ZC_PC_LEVELUP;
        replyPacket.pcId = targetPcID;
        replyPacket.level = level;
    }
}

void zoneBuilderAddHp(uint32_t targetPcID, uint32_t hp, uint32_t maxHp, zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint32_t pcId;
        uint32_t hp;
        uint32_t maxHp; //? not sure
        uint32_t unk1; // This might be some encoding for the source of the hp?
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = ZC_ADD_HP;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        replyPacket.header.type = ZC_ADD_HP;
        replyPacket.pcId = targetPcID;
        replyPacket.hp = hp;
        replyPacket.maxHp = maxHp;
        replyPacket.unk1 = 1;
    }
}

void zoneBuilderBuffList(uint32_t targetPcId, zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint16_t unk1;
        uint32_t pcId;
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

void zoneBuilderLeave(uint32_t targetPcId, zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint32_t pcId;
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
        uint32_t pcId; // A65F0B00
        PositionXYZ position; // 00001DC4, 00008243, 002080C4
        float unk1; // 0000803F
        uint32_t unk2; // 00000000
        uint16_t unk3; // 0000
        uint64_t socialInfoId; // EE2500003C010000
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

void zoneBuilderObjectProperty(zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        // not yet implemented
    } replyPacket;
   (void) replyPacket;
    #pragma pack(pop)

    // PacketType packetType = ZC_OBJECT_PROPERTY;
    // CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);
    // BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        size_t memSize;
        void *memory = dumpToMem(
            "[11:10:22][           ToSClient:                     dbgBuffer]  63 0C FF FF FF FF 7A 02 04 06 00 00 74 9F 01 00 | c.....z.....t...\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  6C 03 00 00 80 3F AB 17 00 58 9D 45 AC 17 00 00 | l....?...X.E....\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  96 43 20 04 00 50 C3 46 62 04 00 40 9C 46 71 03 | .C ..P.Fb..@.Fq.\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 F8 41 48 17 00 00 40 40 49 17 00 00 00 41 | ...AH...@@I....A\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  99 04 00 00 80 40 9A 04 00 00 E0 40 9C 04 00 00 | .....@.....@....\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  C0 40 E8 03 00 00 80 3F 84 03 00 00 00 00 95 04 | .@.....?........\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 80 3F 9D 04 00 00 E0 40 4B 11 00 00 80 40 | ...?.....@K....@\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  4A 11 00 00 80 40 96 04 00 00 40 40 9F 04 00 00 | J....@....@@....\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  40 40 AA 04 00 00 00 00 90 04 00 00 00 00 A7 04 | @@..............\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 D2 42 A8 04 00 00 EE 42 BF 03 00 00 00 00 | ...B.....B......\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  BE 03 00 00 00 00 B9 03 00 00 00 00 B8 03 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 B6 03 00 00 00 00 A5 04 00 00 00 41 98 04 | .............A..\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 80 40 79 04 00 00 00 00 9E 04 00 00 C0 40 | ...@y..........@\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  83 03 00 00 00 00 92 04 00 00 80 40 93 04 00 00 | ...........@....\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 A6 04 00 00 E0 40 72 17 00 00 80 40 69 11 | .......@r....@i.\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 B3 04 00 00 00 00 47 15 00 00 00 00 | ..........G.....\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  AC 04 00 00 00 00 AD 04 00 00 00 00 AE 04 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 56 04 00 00 00 00 53 04 00 00 00 00 A1 03 | ..V.....S.......\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 D5 1E 00 00 00 00 8D 17 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  DD 1E 00 00 00 00 97 03 00 00 00 00 C3 03 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 A6 18 00 00 00 00 C1 03 00 00 00 00 C4 03 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 9B 03 00 00 00 00 98 03 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  96 03 00 00 00 00 A0 03 00 00 00 00 9F 03 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 9D 03 00 00 00 00 A1 18 00 00 00 00 A0 18 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 B2 04 00 00 00 00 7D 03 00 00 00 00 | ..........}.....\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  7E 03 00 00 80 3F 7F 03 08 00 43 68 61 72 34 5F | ~....?....Char4_\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  31 00 94 04 00 00 FA 43 CB 03 00 00 00 00 9A 03 | 1......C........\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 09 04 00 00 00 00 52 17 00 00 00 00 | ..........R.....\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  29 04 00 00 00 00 08 04 00 00 00 00 51 17 00 00 | )...........Q...\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 26 04 00 00 00 00 FC 03 00 00 00 00 F3 03 | ..&.............\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 F1 03 00 40 1C 45 EF 03 00 00 00 00 | .......@.E......\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  EE 03 00 00 7A 43 ED 03 00 40 9C 45 EC 03 00 00 | ....zC...@.E....\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  C8 42 E9 03 00 00 7A 43 EA 03 00 00 00 00 E2 03 | .B....zC........\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 04 00 00 80 3F FE 03 00 00 00 00 | .........?......\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  E5 03 00 00 AF 43 C7 03 00 00 00 00 C9 03 00 00 | .....C..........\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 40 C6 03 45 76 F4 3F C5 03 00 00 80 3F 73 17 | .@..Ev.?.....?s.\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 74 17 00 00 C0 40 70 17 00 00 C0 40 | ....t....@p....@\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  71 17 00 00 40 41 6E 17 00 00 30 41 6D 04 00 00 | q...@An...0Am...\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 68 04 00 00 20 42 66 04 00 00 02 43 65 04 | ..h... Bf....Ce.\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 92 17 00 00 7A 44                   | ........zD\n"
            , NULL, &memSize
        );

        zmsg_add(replyMsg, zframe_new(memory, memSize));
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

void zoneBuilderNormalUnk4(uint64_t socialInfoId, zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        // not yet implemented
        PacketNormalHeader normalHeader;
        uint64_t socialInfoId;
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

void zoneBuilderCooldownList(uint64_t socialInfoId, zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint16_t unk1;
        uint64_t socialInfoId;
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

void zoneBuilderAbilityList(uint32_t targetPcId, zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        VariableSizePacketHeader variableSizeHeader;
        uint32_t pcId;
        uint16_t numberEntries;
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
        replyPacket.numberEntries = 0;
    }
}

void zoneBuilderSkillList(uint32_t targetPcId, zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        VariableSizePacketHeader variableSizeHeader;
        uint32_t pcId;
        uint16_t skillCount;
        Zlib zlibData;
    } replyPacket, *memory;
    #pragma pack(pop)

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        size_t memSize;
        memory = dumpToMem(
            "[03:07:36][main.c:56 in CNetUsr__PacketHandler_1]  33 0C FF FF FF FF 14 00 A6 5F 0B 00 00 00 8D FA | 3........_......\n"
            "[03:07:36][main.c:56 in CNetUsr__PacketHandler_1]  02 00 03 00                                     | ....\n"
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
        uint8_t eqSlotIndex;                     \
        uint8_t unk2;                            \
        uint16_t unk3;                           \
        uint32_t unk4;                           \
        uint8_t properties[attrSize];            \
    }   EquippedItemCPacket;

    size_t itemPropertiesSize[EQSLOT_COUNT];
    size_t itemsPacketSize = 0;

    for (int eqSlotIndex = 0; eqSlotIndex < EQSLOT_COUNT; eqSlotIndex++) {

        ItemEquipable *eqItem = inventory->equippedItems[eqSlotIndex];
        // get property size
        size_t attrSize = eqItem ? itemGetPropertiesCPacketSize(&eqItem->item) : 0;

        #pragma pack(push, 1)
        DEFINE_EquippedItemCPacket(attrSize);
        #pragma pack(pop)

        itemPropertiesSize[eqSlotIndex] = attrSize;
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

        for (int eqSlotIndex = 0; eqSlotIndex < EQSLOT_COUNT; eqSlotIndex++) {

            ItemEquipable *eqItem = inventory->equippedItems[eqSlotIndex];

            size_t attrSize = itemPropertiesSize[eqSlotIndex];

            #pragma pack(push, 1)
            DEFINE_EquippedItemCPacket(attrSize);
            #pragma pack(pop)

            EquippedItemCPacket *eqItemPkt = packetStreamGetCurrentBuffer(&packetStream);

            eqItemPkt->id = eqItem ? itemGetId((Item *) eqItem) : inventoryGetEquipmentEmptySlot(eqSlotIndex);
            eqItemPkt->sizeOfAttributes = attrSize;
            eqItemPkt->unk1 = 0;
            eqItemPkt->uid = eqItem ? actorGetUId((Actor *) eqItem) : 0;
            eqItemPkt->eqSlotIndex = eqSlotIndex;
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

void zoneBuilderSetPos(uint32_t targetPcId, PositionXYZ *position, zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint32_t pcId;
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

void zoneBuilderMoveSpeed(uint32_t targetPcId, float movementSpeed, zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint32_t pcId;
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
        uint32_t pcId;
        uint8_t familyName[COMMANDER_FAMILY_NAME_SIZE];
        uint8_t commanderName[COMMANDER_NAME_SIZE+1];
        uint8_t unk1;
        uint16_t jobId;
        uint32_t unk2;
        uint8_t gender;
        uint8_t hairId;
        uint16_t unk3;
        uint32_t headTop;
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

void zoneBuilderJump(uint32_t targetPcId, float height, zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint32_t pcId;
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
    uint8_t gameMode,
    uint8_t accountPrivileges,
    Commander *commander,
    zmsg_t *replyMsg)
{
    #pragma pack(push, 1)
    struct {
        VariableSizePacketHeader variableSizeHeader;
        uint8_t gameMode; // 0 = NormalMode, 1 = SingleMode
        uint32_t unk1;
        uint8_t accountPrivileges;
        uint16_t unk2;
        uint64_t unk3;
        uint8_t markers[3];
        uint8_t passport[41];
        uint32_t pcId;
        uint32_t unk5;

        CommanderAppearanceCPacket appearance;
        PositionXYZ pos;
        uint32_t currentXP;
        uint32_t maxXP;
        uint32_t _pcId;
        uint64_t socialInfoId;
        uint64_t commanderId;
        uint32_t currentHP;
        uint32_t maxHP;
        uint16_t currentSP;
        uint16_t maxSP;
        uint32_t currentStamina;
        uint32_t maxStamina;
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
        uint16_t unkown1;                                     \
        ActorId_t itemUId;                                    \
        ItemAmount_t amount;                                  \
        ItemPrice_t price;                                    \
        ItemInventoryIndex_t inventoryIndex;                  \
        uint32_t unkown2;                                     \
        uint8_t properties[attrSize];                         \
    } InventoryItemCPacket;

    // Get inventory count
    size_t inventoryCount = inventoryGetItemsCount(inventory);

    // Calculate size in packet to store all items
    size_t itemPropertiesSize[ITEM_CAT_COUNT][inventoryCount];
    size_t totalSize = 0;

    // Itarate all inventory bags
    for (int category = 0; category < ITEM_CAT_COUNT; category++) {

        // Iterate through all items in this bag
        Item *item = inventoryGetFirstItem(inventory, category);
        uint8_t inventoryIndex = 0;

        while (item) {

        size_t attrSize = item ? itemGetPropertiesCPacketSize(item) : 0;

            #pragma pack(push, 1)
            DEFINE_InventoryItemCPacket(attrSize);
            #pragma pack(pop)

            itemPropertiesSize[category][inventoryIndex] = attrSize;
            totalSize += sizeof(InventoryItemCPacket);

            item = inventoryGetNextItem(inventory, category);
            inventoryIndex++;
        }
    }



    /// VERSION OF THE PACKET, NO COMPRESSED
    /*
    #pragma pack(push, 1)
    struct InventoryListPacket {
        VariableSizePacketHeader variableSizeHeader;
        uint32_t inventoryCount;
        uint16_t compression;
        uint8_t itemsPacket[totalSize];
    } replyPacket;
    #pragma pack(pop)

    PacketType packetType = ZC_ITEM_INVENTORY_LIST;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    PacketStream packetStream;
    packetStreamInit(&packetStream, &replyPacket);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {
        variableSizePacketHeaderInit(&replyPacket.variableSizeHeader, packetType, sizeof(replyPacket));

        replyPacket.inventoryCount = inventoryCount;
        replyPacket.compression = 0;

        // we want to start writing at the offset of commandersBarrackInfoPacket
        size_t offset = offsetof(struct InventoryListPacket, itemsPacket);
        packetStreamAddOffset(&packetStream, offset);

        item = inventoryGetFirstItem(inventory);
        inventoryIndex = -1;

        while (item) {

            inventoryIndex++;
            size_t attrSize = itemPropertiesSize[inventoryIndex];

            #pragma pack(push, 1)
            DEFINE_InventoryItemCPacket(attrSize);
            #pragma pack(pop)

            InventoryItemCPacket *inventoryItemCPacket  = packetStreamGetCurrentBuffer(&packetStream);

            inventoryItemCPacket->itemType = item->itemType;
            inventoryItemCPacket->sizeOfAttributes = attrSize;
            inventoryItemCPacket->unkown1 = 0;
            inventoryItemCPacket->itemId = item ? item->itemId : 0;
            inventoryItemCPacket->amount = item->amount;
            inventoryItemCPacket->price = 0;
            inventoryItemCPacket->inventoryIndex = ITEM_CAT_SIZE * item->itemCategory + item->inventoryIndex;
            inventoryItemCPacket->unkown2 = 0;

            // fill property buffer
            size_t offset = offsetof(struct InventoryItemCPacket, properties);
            packetStreamAddOffset(&packetStream, offset);

            // write in the buffer
            if (attrSize > 0) {
                itemPropertiesGetCPacket(item->properties, packetStreamGetCurrentBuffer(&packetStream));
                // relocate the stream position
                packetStreamAddOffset(&packetStream, attrSize);
            }


            item = inventoryGetNextItem(inventory);
        }

    }

    // Debug purposes
    buffer_print(&replyPacket, sizeof(replyPacket), NULL);

    */

    /// VERSION OF THE PACKET, USING zLIB compression

    // Populate packet for Items
    uint8_t itemsPacket[totalSize];

    PacketStream packetStream;
    packetStreamInit(&packetStream, &itemsPacket);

    // Itarate all inventory bags
    for (int category = 0; category < ITEM_CAT_COUNT; category++) {

        // Iterate through all items in this bag
        Item *item = inventoryGetFirstItem(inventory, category);
        uint8_t inventoryIndex = 0;

        while (item) {
            size_t attrSize = itemPropertiesSize[category][inventoryIndex];

            // write in the buffer
            if (attrSize > 0) {
                itemPropertiesGetCPacket(item, &packetStream);
            }

            item = inventoryGetNextItem(inventory, category);
            inventoryIndex++;
        }
    }


    // Debug purposes
    buffer_print(&itemsPacket, sizeof(itemsPacket), NULL);


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
    zlibCompress(&replyPacket.zlibData, &itemsPacket, sizeof(itemsPacket));
    size_t outPacketSize = ZLIB_GET_COMPRESSED_PACKET_SIZE(&replyPacket.zlibData, sizeof(replyPacket));
    variableSizePacketHeaderInit(&replyPacket.variableSizeHeader, packetType, outPacketSize);

    zmsg_add(replyMsg, zframe_new(&replyPacket, outPacketSize));

    // Debug purposes
    buffer_print(&replyPacket, outPacketSize, NULL);
}

void zoneBuilderMoveDir(
    uint32_t targetPcId,
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
        uint32_t pcId;
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
    uint32_t targetPcId,
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
        uint32_t pcId;
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

void zoneBuilderRotateHead(uint32_t pcId, PositionXZ *pos, zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint32_t pcId;
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

void zoneBuilderRotate(uint32_t pcId, PositionXZ *bodyDir, zmsg_t *replyMsg) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint32_t pcId;
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

void zoneBuilderPose(uint32_t pcId, uint32_t poseId, PositionXYZ *pos, PositionXZ *dir, zmsg_t *replyMsg) {

    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint32_t pcId;
        uint32_t poseId;
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

void zoneBuilderItemRemove(Item *item, uint8_t removalType, uint8_t inventoryType, zmsg_t *replyMsg) {

    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint64_t uid;
        uint32_t amount;
        uint8_t removalType;
        uint8_t inventoryType;
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
