/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file commander.h
 * @brief CommanderInfo contains all character related functions
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#pragma once

#include "R1EMU.h"
#include "inventory.h"
#include "skills_manager.h"

#define COMMANDER_NAME_SIZE 64
#define COMMANDER_FAMILY_NAME_SIZE 64
#define COMMANDER_HEIGHT_JUMP 350.0f

typedef uint16_t CommanderClassId_t;
typedef uint16_t CommanderJobId_t;
typedef uint16_t CommanderJobPoints_t;
typedef uint8_t CommanderGender_t;
typedef uint8_t CommanderHairId_t;
typedef uint32_t CommanderLevel_t;
typedef uint16_t CommanderPose_t;
typedef ActorId_t CommanderId_t;
typedef ActorId_t AccountId_t;
typedef ActorId_t SocialInfoId_t;
typedef uint32_t PcId_t;
typedef uint32_t Xp_t;
typedef uint32_t Hp_t;
typedef uint16_t Sp_t;
typedef uint32_t Stamina_t;

/**
 * @brief CommanderEquipmentCPacket is the structure of a commander equipment client packet.
 */
#pragma pack(push, 1)
typedef struct CommanderEquipmentCPacket {
    ItemId_t head_top;
    ItemId_t head_middle;
    ItemId_t itemUnk1;
    ItemId_t body_armor;
    ItemId_t gloves;
    ItemId_t boots;
    ItemId_t helmet;
    ItemId_t bracelet;
    ItemId_t weapon;
    ItemId_t shield;
    ItemId_t costume;
    ItemId_t itemUnk3;
    ItemId_t itemUnk4;
    ItemId_t itemUnk5;
    ItemId_t leg_armor;
    ItemId_t itemUnk6;
    ItemId_t itemUnk7;
    ItemId_t ring_left;
    ItemId_t ring_right;
    ItemId_t necklace;
} CommanderEquipmentCPacket;
#pragma pack(pop)

/**
 * @brief CommanderAppearanceCPacket is the struct of a commander sent to the client
 */
#pragma pack(push, 1)
typedef struct CommanderAppearanceCPacket
{
    uint8_t commanderName[COMMANDER_NAME_SIZE+1];
    uint8_t familyName[COMMANDER_FAMILY_NAME_SIZE];
    uint8_t unk2[7];
    AccountId_t accountId;
    CommanderClassId_t classId;
    uint16_t unk4;
    CommanderJobId_t jobId;
    CommanderGender_t gender;
    uint8_t unk5;
    CommanderLevel_t level;
    CommanderEquipmentCPacket equipment;
    CommanderHairId_t hairId;
    uint8_t unk6;
    CommanderPose_t pose;
} CommanderAppearanceCPacket;
#pragma pack(pop)

/**
 * Contains all information about a commander
 */
typedef struct
{
    /** Appearance */
    uint8_t commanderName[COMMANDER_NAME_SIZE+1];
    uint8_t familyName[COMMANDER_FAMILY_NAME_SIZE];
    CommanderGender_t gender;
    CommanderHairId_t hairId;
    CommanderPose_t pose;
    PositionXYZ pos;
    PositionXYZ barrackPos;
    PositionXZ dir;
    CommanderClassId_t classId;
    CommanderJobId_t jobId;

    /** Identificators */
    ActorId_t accountId;
    PcId_t pcId;
    SocialInfoId_t socialInfoId;
    CommanderId_t commanderId;
    MapId_t mapId;

    /** Stats */
    CommanderLevel_t level;
    Xp_t currentXP;
    Xp_t maxXP;
    Hp_t currentHP;
    Hp_t maxHP;
    Sp_t currentSP;
    Sp_t maxSP;
    Stamina_t currentStamina;
    Stamina_t maxStamina;

    /** Commander inventory */
    Inventory inventory;
    /** Commander skills */
    SkillsManager skillsManager;

} Commander;

typedef struct
{
    /** Appearance */
    uint8_t commanderName[COMMANDER_NAME_SIZE+1];
    uint8_t familyName[COMMANDER_FAMILY_NAME_SIZE];
    CommanderGender_t gender;
    CommanderHairId_t hairId;
    CommanderPose_t pose;
    PositionXYZ pos;
    PositionXZ dir;
    CommanderClassId_t classId;
    CommanderJobId_t jobId;

    /** Identificators */
    ActorId_t accountId;
    PcId_t pcId;
    SocialInfoId_t socialInfoId;
    CommanderId_t commanderId;
    MapId_t mapId;

    /** Stats */
    CommanderLevel_t level;
    Xp_t currentXP;
    Xp_t maxXP;
    Hp_t currentHP;
    Hp_t maxHP;
    Sp_t currentSP;
    Sp_t maxSP;
    Stamina_t currentStamina;
    Stamina_t maxStamina;

    /** Commander inventory */
    InventorySPacket inventory[0];

} CommanderSPacket;

typedef enum CommanderJobId
{
    COMMANDER_JOB_WARRIOR = 1001,
    COMMANDER_JOB_HIGHLANDER = 1002,
    COMMANDER_JOB_PELTASTA = 1003,
    COMMANDER_JOB_HOPLITE = 1004,
    COMMANDER_JOB_CENTURION = 1005,
    COMMANDER_JOB_BARBARIAN = 1006,
    COMMANDER_JOB_CATAPHRACT = 1007,
    COMMANDER_JOB_CORSAIR = 1008,
    COMMANDER_JOB_DOPPELSOLDNER = 1009,
    COMMANDER_JOB_RODELERO = 1010,
    COMMANDER_JOB_SQUIRE = 1011,
    COMMANDER_JOB_MURMILLO = 1012,
    COMMANDER_JOB_SHINOBI = 1013,
    COMMANDER_JOB_WIZARD = 2001,
    COMMANDER_JOB_PYROMANCER = 2002,
    COMMANDER_JOB_CRYOMANCER = 2003,
    COMMANDER_JOB_PSYCHOKINO = 2004,
    COMMANDER_JOB_ALCHEMIST = 2005,
    COMMANDER_JOB_SORCERER = 2006,
    COMMANDER_JOB_LINKER = 2007,
    COMMANDER_JOB_CHRONOMANCER = 2008,
    COMMANDER_JOB_NECROMANCER = 2009,
    COMMANDER_JOB_THAUMATURGE = 2010,
    COMMANDER_JOB_WARLOCK = 2011,
    COMMANDER_JOB_MIMIC = 2012,
    COMMANDER_JOB_DAOSHI = 2013,
    COMMANDER_JOB_ARCHER  = 3001,
    COMMANDER_JOB_HUNTER = 3002,
    COMMANDER_JOB_QUARRELSHOOTER = 3003,
    COMMANDER_JOB_RANGER = 3004,
    COMMANDER_JOB_SAPPER = 3005,
    COMMANDER_JOB_WUGUSHI = 3006,
    COMMANDER_JOB_HACKAPELL = 3007,
    COMMANDER_JOB_SCOUT = 3008,
    COMMANDER_JOB_ROGUE = 3009,
    COMMANDER_JOB_SCHWARZEREITER = 3010,
    COMMANDER_JOB_FLETCHER = 3011,
    COMMANDER_JOB_PIEDPIPER = 3012,
    COMMANDER_JOB_APPRAISER = 3013,
    COMMANDER_JOB_FALCONER = 3014,
    COMMANDER_JOB_CLERIC  = 4001,
    COMMANDER_JOB_PRIEST = 4002,
    COMMANDER_JOB_KRIVIS = 4003,
    COMMANDER_JOB_BOKOR = 4004,
    COMMANDER_JOB_DRUID = 4005,
    COMMANDER_JOB_SADHU = 4006,
    COMMANDER_JOB_DIEVDIRBYS = 4007,
    COMMANDER_JOB_ORACLE = 4008,
    COMMANDER_JOB_MONK = 4009,
    COMMANDER_JOB_PARDONER = 4010,
    COMMANDER_JOB_PALADIN = 4011,
    COMMANDER_JOB_CHAPLAIN = 4012,
    COMMANDER_JOB_SHEPHERD = 4013,
    COMMANDER_JOB_GM = 9001
}   CommanderJobId;

typedef enum CommanderClassId
{
    COMMANDER_CLASS_WARRIOR = 10001,
    COMMANDER_CLASS_ARCHER  = 10003,
    COMMANDER_CLASS_CLERIC  = 10005,
    COMMANDER_CLASS_WIZARD  = 10006,

} CommanderClassId;

typedef enum CommanderGender
{
    COMMANDER_GENDER_MALE   = 1,
    COMMANDER_GENDER_FEMALE = 2,
    COMMANDER_GENDER_BOTH   = 3,

} CommanderGender;

typedef enum CommanderHair
{
    COMMANDER_HAIR_COUNT
} CommanderHair;


/**
 * Allocate a new Commander structure.
 * @return A pointer to an allocated Commander, or NULL if an error occured.
 */
Commander *commanderNew(void);

/**
 * Initialize an allocated Commander structure.
 * @param self An allocated Commander to initialize.
 * @return true on success, false otherwise.
 */
bool commanderInit(Commander *commander);

/**
 * @brief Print commander basic information
 */
bool commanderEquipmentCPacketInit(CommanderEquipmentCPacket *equipment);
bool commanderAppearanceCPacketInit (
    CommanderAppearanceCPacket *appearance,
    char *familyName,
    char *commanderName,
    ActorId_t accountId,
    CommanderClassId_t classId,
    CommanderJobId_t jobId,
    CommanderGender_t gender,
    CommanderLevel_t level,
    CommanderHairId_t hairId,
    CommanderPose_t pose);

void commanderPrint(Commander *commander);

/**
 * @brief Duplicate a commander in memory
 */
Commander *commanderDup(Commander *src);

/**
 * @brief Free a commander in memory
 */
void commanderFree(Commander *self);
void commanderDestroy(Commander **_self);
