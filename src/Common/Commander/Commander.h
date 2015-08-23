/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file Commander.h
 * @brief Commander contains all character related functions
 *
 * @license <license placeholder>
 */

#pragma once

// ---------- Includes ------------
#include "R1EMU.h"

// ---------- Defines -------------
#define COMMANDER_NAME_SIZE 64
#define COMMANDER_FAMILY_NAME_SIZE 64
#define COMMANDER_HEIGHT_JUMP 350.0f

// ------ Structure declaration -------

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
    uint32_t itemUnk2;
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
 * @brief CommanderInfo is the struct of a commander.
 */

typedef struct Commander
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
    uint16_t hairType;
    uint16_t pose;
}   Commander;

typedef struct CommanderInfo
{
    Commander base;
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
}   CommanderInfo;
#pragma pack(pop)

typedef struct CommanderCreateInfo {
    Commander commander;
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

typedef enum CommanderJobId
{
    COMMANDER_JOB_WARRIOR = 0x3E9,
    COMMANDER_JOB_ARCHER  = 0xBB9,
    COMMANDER_JOB_MAGE    = 0x7D1,
    COMMANDER_JOB_CLERIC  = 0xFA1
}   CommanderJobId;

typedef enum CommanderClassId
{
    COMMANDER_CLASS_WARRIOR = 10001,
    COMMANDER_CLASS_ARCHER  = 10003,
    COMMANDER_CLASS_CLERIC  = 10005,
    COMMANDER_CLASS_MAGE    = 10006,

}   CommanderClassId;


typedef enum CommanderGender
{
    COMMANDER_GENDER_MALE   = 1,
    COMMANDER_GENDER_FEMALE = 2,
    COMMANDER_GENDER_BOTH   = 3,

}   CommanderGender;


typedef enum CommanderHair
{
    COMMANDER_HAIR_COUNT
}   CommanderHair;

// ----------- Functions ------------

/**
 * @brief Initialize a commander with basic information
 */

void
CommanderInfo_init (
    CommanderInfo *commander
);

/**
 * @brief Dump a CommanderInfo in the console
 */
void
CommanderInfo_print (
    CommanderInfo *commander
);
