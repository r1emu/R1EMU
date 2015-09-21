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

void barrackBuilderCommanderList(uint64_t accountId, GameSession *gameSession, int commandersCount, Commander *commanders ,zmsg_t *replyMsg) {


    size_t commandersSize = 0;
    char *commandersPacket;
    char *tempCommandersPacket;
    char *emptyCommanderBuffer;

    // Alloc memory for initial commandersPacket pointer
    if ((commandersPacket = malloc(sizeof(commandersPacket))) == NULL) {
        return;
    }
    // Commanders list
    for (int commanderIndex = 0; commanderIndex < commandersCount; commanderIndex++) {

        Commander commanderData = commanders[commanderIndex];
        CommanderAppearance *commanderAppearance = &commanderData.info.appearance;

        CommanderEquipment *dataEquipment = &commanderAppearance->equipment;

        Inventory *inventory = &commanderData.inventory;

        ///TODO
        inventoryInit(inventory); /// TESTING PURPOSES, INVENTORY SHOULD BE INITIALIZED IN OTHER PLACE.

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

        size_t attributeSizes[EQSLOT_Count]; // Size of each item attributes
        char *attributes[EQSLOT_Count]; // each item attributes
        size_t equipmentAttrSize = 0; // size of all item attributes + item attribute size headers

        // Iterate through equipment
        for (int i = 0; i < EQSLOT_Count; i++) {

            // Get item from inventory.
            Item* item = inventory->equippedItems[i];

            if (item != NULL) {
                // Get attributes packet for this item

                // Attributes size
                attributeSizes[i] = itemAttributesGetPacketSize(item->attributes);

                // Attributes packet
                char packet[attributeSizes[i]];
                itemAttributesGetPacket(item->attributes, packet);
                attributes[i] = packet;

            } else {
                // No item in this slot.
                attributeSizes[i] = 0;
            }

            // Keep track of size of all attributes, add space for "length" of each packet.
            equipmentAttrSize += sizeof(uint16_t) + attributeSizes[i];
        }

        /**
         * Structure of variables needed for BC_COMMANDER_CREATE
         * 314 bytes + attributes (variable length)
         */
        #pragma pack(push, 1)
        typedef struct CommanderBarrackInfo2 {
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
            char equipmentAttributes[equipmentAttrSize];
            uint16_t unk9;
        } CommanderBarrackInfo2;
        #pragma pack(pop)

        // IS ALLOC'ING HERE.. REALLY NEEDED? Or CommanderBarrackInfo2 can be used to memcpy into Packet structure?
        // realloc memory to add space for this commander
        tempCommandersPacket = realloc(commandersPacket, commandersSize + sizeof(CommanderBarrackInfo2));
        if (tempCommandersPacket != NULL) {
            commandersPacket = tempCommandersPacket;
        } else {
            return; /// TODO, how to handle errors in this part of the code?
        }

        // Get a pointer to the empty space in memry to copy the new commander
        emptyCommanderBuffer = commandersPacket+commandersSize;

        // Update commandersSize size, with the total size.
        commandersSize = commandersSize + sizeof(CommanderBarrackInfo2);

        // Creates a new structure to hold new commanders data
        CommanderBarrackInfo2 currentCommanderBarrackInfo;

        // Set strcture to 00
        memset(&currentCommanderBarrackInfo, 0, sizeof(currentCommanderBarrackInfo));

        // Set some pointers to make code easier to read.
        CommanderAppearance *currentCommander = &currentCommanderBarrackInfo.commander;
        CommanderEquipment *cEquipment = &currentCommander->equipment;


        // Set commanders Info
        strncpy(currentCommander->commanderName, commanderAppearance->commanderName, sizeof(currentCommander->commanderName));

        //
        currentCommander->accountId = 0x0; // Not needed
        currentCommander->classId = commanderAppearance->classId;
        currentCommander->unk4 = 0x0;
        currentCommander->jobId = commanderAppearance->jobId;
        currentCommander->level = commanderAppearance->level;
        currentCommander->gender = commanderAppearance->gender;
        currentCommander->hairId = commanderAppearance->hairId;

        // Equipment
        cEquipment->head_top = dataEquipment->head_top;
        cEquipment->head_middle = dataEquipment->head_middle;
        cEquipment->itemUnk1 = dataEquipment->itemUnk1;
        cEquipment->body_armor = dataEquipment->body_armor;
        cEquipment->gloves = dataEquipment->gloves;
        cEquipment->boots = dataEquipment->boots;
        cEquipment->helmet = dataEquipment->helmet;
        cEquipment->bracelet = dataEquipment->bracelet;
        cEquipment->weapon = dataEquipment->weapon;
        cEquipment->shield = dataEquipment->shield;
        cEquipment->costume = dataEquipment->costume;
        cEquipment->itemUnk3 = dataEquipment->itemUnk3;
        cEquipment->itemUnk4 = dataEquipment->itemUnk4;
        cEquipment->itemUnk5 = dataEquipment->itemUnk5;
        cEquipment->leg_armor = dataEquipment->leg_armor;
        cEquipment->itemUnk6 = dataEquipment->itemUnk6;
        cEquipment->itemUnk7 = dataEquipment->itemUnk7;
        cEquipment->ring_left = dataEquipment->ring_left;
        cEquipment->ring_right = dataEquipment->ring_right;
        cEquipment->necklace = dataEquipment->necklace;

        currentCommanderBarrackInfo.socialInfoId = commanderData.info.socialInfoId; // CharUniqueId?
        currentCommanderBarrackInfo.commanderPosition = commanderIndex+1;
        currentCommanderBarrackInfo.mapId = 1002; /// TODO FIX Not MapId in currecnt structure!
        currentCommanderBarrackInfo.unk4 = SWAP_UINT32(0x02000000); //
        currentCommanderBarrackInfo.unk5 = 0;
        currentCommanderBarrackInfo.maxXP = commanderData.info.maxXP; // ?? Or current XP?
        currentCommanderBarrackInfo.unk6 = SWAP_UINT32(0xC01C761C); //

        currentCommanderBarrackInfo.pos.x = SWAP_UINT32(0x25e852c1);
        currentCommanderBarrackInfo.pos.y = SWAP_UINT32(0x6519e541);
        currentCommanderBarrackInfo.pos.z = SWAP_UINT32(0x39f4ef42);

        currentCommanderBarrackInfo.dir.x = 0; // Set direction to face camera.
        currentCommanderBarrackInfo.dir.z = 0; // Set direction to face camera.

        currentCommanderBarrackInfo.pos2.x = SWAP_UINT32(0x25e852c1);
        currentCommanderBarrackInfo.pos2.y = SWAP_UINT32(0x6519e541);
        currentCommanderBarrackInfo.pos2.z = SWAP_UINT32(0x39f4ef42);
        currentCommanderBarrackInfo.dir2.x = 0;
        currentCommanderBarrackInfo.dir2.z = 0;
        currentCommanderBarrackInfo.unk8 = 0;

        // Concatenate all "item attributes packets" in one packet.
        size_t offset = 0; // Offset inside "equipmentAttributes" packet.
        for (int i = 0; i < EQSLOT_Count; i++) {

            uint16_t attrSize = (uint16_t) attributeSizes[i];
            // Size of item's attributes
            memcpy(currentCommanderBarrackInfo.equipmentAttributes+offset, &attrSize, sizeof(uint16_t));
            // Actual item's attributes (only if any)
            if (attributeSizes[i] > 0) {
                memcpy(currentCommanderBarrackInfo.equipmentAttributes+sizeof(uint16_t)+offset, attributes[i], attributeSizes[i]);
            }
            // Increase offset
            offset += sizeof(uint16_t) + attributeSizes[i];
        }

        currentCommanderBarrackInfo.unk9 = 0;

        // copy struct to the space
        memcpy(emptyCommanderBuffer, &currentCommanderBarrackInfo, sizeof(currentCommanderBarrackInfo));

        // Print buffers for debugging purposes
        //buffer_print (emptyCommanderBuffer, sizeof(currentCommanderBarrackInfo), NULL);
        //buffer_print (commandersPacket, commandersSize, NULL);

    }
    /// FOR EACH ACCOUNT INFO (yet hard to know which they are)
    // Account Info struct
    /*
    #pragma pack(push, 1)
    struct AccountInfo {
        uint16_t accountInfoType;
        uint8_t *AccountInfoContent;
    } AccountInfo;
    #pragma pack(pop)
    */

    //int accountInfoCount = 3;

    #pragma pack(push, 1)
    struct {
        VariableSizePacketHeader variableSizeHeader;
        uint64_t accountId;
        uint8_t unk1;
        uint8_t commandersCount;
        uint8_t familyName [COMMANDER_FAMILY_NAME_SIZE];
        uint16_t accountInfoLength; // sizeof(accountInfo)
        //AccountInfo accountInfo[accountInfoCount];

        uint16_t typeCredits;
        float creditsAmount;
        uint16_t typeCredits2;
        float creditsAmount2;
        uint16_t typeCredits3;
        float creditsAmount3;
        char commandersPacket[commandersSize];
    } replyPacket;
    #pragma pack(pop)


    PacketType packetType = BC_COMMANDER_LIST;
    CHECK_SERVER_PACKET_SIZE(replyPacket, packetType);

    BUILD_REPLY_PACKET(replyPacket, replyMsg)
    {

        variableSizePacketHeaderInit(&replyPacket.variableSizeHeader, packetType, sizeof(replyPacket));
        replyPacket.accountId = accountId;
        replyPacket.unk1 = 1; // ICBT - equal to 1 or 4
        replyPacket.commandersCount = commandersCount;

        // Family name
        strncpy(replyPacket.familyName, gameSession->accountSession.familyName, sizeof(replyPacket.familyName));
        /*
        // Account Info
        //replyPacket.accountInfoLength = sizeof(replyPacket.accountInfo) / sizeof(uint8_t); // Is this right? (total bytes / 8)
        replyPacket.accountInfo[0].accountInfoType = SWAP_UINT16(0x940e); // 94 0E = Medal (iCoin)
        replyPacket.accountInfo[0].AccountInfoContent = gameSession->accountSession.credits;
        replyPacket.accountInfo[1].accountInfoType = SWAP_UINT16(0x970e); // 97 0E = GiftMedal
        replyPacket.accountInfo[2].AccountInfoContent = 0;
        replyPacket.accountInfo[2].accountInfoType = SWAP_UINT16(0x950e); // 95 0E = ReceiveGiftMedal
        replyPacket.accountInfo[2].AccountInfoContent = 0;
        */
        replyPacket.accountInfoLength = 0x12; // 3 sets
        replyPacket.typeCredits = SWAP_UINT16(0x940e); // 94 0E = Medal (iCoin)
        replyPacket.creditsAmount = gameSession->accountSession.credits;
        replyPacket.typeCredits2 = SWAP_UINT16(0x970e); // 94 0E = Medal (iCoin)
        replyPacket.creditsAmount2 = 0;
        replyPacket.typeCredits3 = SWAP_UINT16(0x950e); // 94 0E = Medal (iCoin)
        replyPacket.creditsAmount3 = 0;

        memcpy(replyPacket.commandersPacket, commandersPacket, commandersSize);
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
