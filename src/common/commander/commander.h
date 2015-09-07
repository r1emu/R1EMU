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
 * @brief Commander contains all character related functions
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
 * @brief Commander is the struct of a commander.
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
 * @brief CommanderPkt is the struct of a commander sent to the client
 */
typedef struct CommanderPkt
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
} CommanderPkt;

typedef struct Commander
{
    CommanderPkt base;
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
    Inventory inventory;
} Commander;
#pragma pack(pop)

typedef enum CommanderJobId
{
    COMMANDER_JOB_WARRIOR = 1001,
    COMMANDER_JOB_MAGE    = 2001,
    COMMANDER_JOB_ARCHER  = 3001,
    COMMANDER_JOB_CLERIC  = 4001
}   CommanderJobId;

typedef enum CommanderClassId
{
    COMMANDER_CLASS_WARRIOR = 10001,
    COMMANDER_CLASS_ARCHER  = 10003,
    COMMANDER_CLASS_CLERIC  = 10005,
    COMMANDER_CLASS_MAGE    = 10006,

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
void commanderInit(Commander *commander);
void commanderPktInit(CommanderPkt *commander);

/**
 * @brief Dump a Commander in the console
 */
void commanderPrint(Commander *commander);
void commanderPktPrint(CommanderPkt *commanderPkt);
