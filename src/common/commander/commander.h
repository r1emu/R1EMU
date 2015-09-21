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

#define COMMANDER_NAME_SIZE 64
#define COMMANDER_FAMILY_NAME_SIZE 64
#define COMMANDER_HEIGHT_JUMP 350.0f

#pragma pack(push, 1)
/**
 * @brief CommanderInfo is the struct of a commander.
 */
typedef struct CommanderEquipment {
    uint32_t head_top;
    uint32_t head_middle;
    uint32_t itemUnk1;
    uint32_t body_armor;
    uint32_t gloves;
    uint32_t boots;
    uint32_t helmet;
    uint32_t bracelet;
    uint32_t weapon;
    uint32_t shield;
    uint32_t costume;
    uint32_t itemUnk3;
    uint32_t itemUnk4;
    uint32_t itemUnk5;
    uint32_t leg_armor;
    uint32_t itemUnk6;
    uint32_t itemUnk7;
    uint32_t ring_left;
    uint32_t ring_right;
    uint32_t necklace;
} CommanderEquipment;
#pragma pack(pop)

#pragma pack(push, 1)
/**
 * @brief CommanderAppearance is the struct of a commander sent to the client
 */
typedef struct CommanderAppearance
{
    uint8_t commanderName [COMMANDER_NAME_SIZE+1];
    uint8_t familyName [COMMANDER_FAMILY_NAME_SIZE];
    uint8_t unk2[7];
    uint64_t accountId;
    uint16_t classId;
    uint16_t unk4;
    uint16_t jobId;
    uint8_t gender;
    uint8_t unk5;
    uint32_t level;
    CommanderEquipment equipment;
    uint8_t hairId;
    uint8_t unk6;
    uint16_t pose;
} CommanderAppearance;
#pragma pack(pop)

#pragma pack(push, 1)
/**
 * Structure of variables needed for BC_COMMANDER_CREATE
 */
typedef struct CommanderCreateInfo {
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
} CommanderCreateInfo;
#pragma pack(pop)

/**
 * Contains information about a commander exchanged between the client and server
 */
#pragma pack(push, 1)
typedef struct
{
    CommanderAppearance appearance;
    PositionXYZ pos;
    uint32_t currentXP;
    uint32_t maxXP;
    uint32_t pcId;
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
} CommanderInfo;
#pragma pack(pop)

/**
 * Contains all information about a commander
 */
#pragma pack(push, 1)
typedef struct
{
    CommanderInfo info;
    Inventory inventory;

    uint16_t mapId; // Is it the right place?

} Commander;
#pragma pack(pop)

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
 * @brief Initialize a commander with basic information
 */
void commanderInfoInit(CommanderInfo *commander);
void commanderApparenceInit(CommanderAppearance *appearance);
void commanderInit(Commander *commander);

/**
 * @brief Dump a CommanderInfo in the console
 */
void commanderInfoPrint(CommanderInfo *commander);
void commanderAppearancePrint(CommanderAppearance *appearance);
