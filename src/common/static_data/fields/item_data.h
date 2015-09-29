/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file item_data.h
 * @brief
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#pragma once

// Includes
#include "R1EMU.h"

// Type definition
typedef struct ItemData {
    int ClassID;
    int Weight;
    int PR;
    int DamageRange;
    int ItemLv;
    int PATK;
    int ADD_MATK;
    int ItemGrade;
    int ItemStar;
    int UseLv;
    int MaterialPrice;
    int Price;
    int PriceRatio;
    int SellPrice;
    int RepairPriceRatio;
    int MaxStack;
    int Scale;
    int Dur;
    int MaxDur;
    int repairPrice;
    int UseCount;
    int Hardness;
    int ADD_MINATK;
    int ADD_MAXATK;
    int ADD_DEF;
    int CRTHR;
    int CRTATK;
    int CRTDR;
    int ADD_HR;
    int ADD_DR;
    int STR;
    int DEX;
    int CON;
    int INT;
    int MNA;
    int SR;
    int SDR;
    int MHR;
    int ADD_MDEF;
    int MGP;
    int AddSkillMaxR;
    int SkillRange;
    int SkillAngle;
    int Luck;
    int BlockRate;
    int BLK;
    int BLK_BREAK;
    int Revive;
    int HitCount;
    int BackHit;
    int SkillPower;
    int ASPD;
    int MSPD;
    int KDPow;
    int MHP;
    int MSP;
    int MSTA;
    int RHP;
    int RSP;
    int RSPTIME;
    int RSTA;
    int ADD_CLOTH;
    int ADD_LEATHER;
    int ADD_CHAIN;
    int ADD_IRON;
    int ADD_GHOST;
    int ADD_SMALLSIZE;
    int ADD_MIDDLESIZE;
    int ADD_LARGESIZE;
    int ADD_FORESTER;
    int ADD_WIDLING;
    int ADD_VELIAS;
    int ADD_PARAMUNE;
    int ADD_KLAIDA;
    int ADD_FIRE;
    int ADD_ICE;
    int ADD_POISON;
    int ADD_LIGHTNING;
    int ADD_EARTH;
    int ADD_HOLY;
    int ADD_DARK;
    int BaseSocket;
    int MaxSocket;
    int MinOption;
    int MaxOption;
    int Aries;
    int AriesDEF;
    int Slash;
    int SlashDEF;
    int Strike;
    int StrikeDEF;
    int Aries_Range;
    int Slash_Range;
    int Strike_Range;
    int MinRDmg;
    int MaxRDmg;
    int FDMinR;
    int FDMaxR;
    int TotalArrowCount;
    int ArrowCount;
    int RES_FIRE;
    int RES_ICE;
    int RES_POISON;
    int RES_LIGHTNING;
    int RES_EARTH;
    int RES_HOLY;
    int RES_DARK;
    int BaseSocket_MA;
    int MaxSocket_MA;
    char *ClassName;
    char *Name;
    char *ItemType;
    char *Journal;
    char *GroupName;
    char *EquipGroup;
    char *EquipXpGroup;
    char *ToolTipScp;
    char *BasicTooltipProp;
    char *TooltipImage;
    char *Icon;
    char *ReqToolTip;
    char *Desc;
    char *Destroyable;
    char *ShopTrade;
    char *UserTrade;
    char *DropStyle;
    char *Usable;
    char *Consumable;
    char *UseTx;
    char *ClassType;
    char *ClassType2;
    char *Attribute;
    char *AttackType;
    char *Material;
    char *UseJob;
    char *UseGender;
    char *DBLHand;
    char *EqpType;
    char *DefaultEqpSlot;
}   ItemData;

// Functions
/**
 * @brief Allocate a new ItemData structure.
 * @param row The row of the item in the SQL query
 * @return A pointer to an allocated ItemData.
 */
ItemData *itemDataNew(MYSQL_ROW row);

/**
 * @brief Initialize an allocated ItemData structure.
 * @param self An allocated ItemData to initialize.
 * @param row The row of the item in the SQL query
 * @return true on success, false otherwise.
 */
bool itemDataInit(ItemData *self, MYSQL_ROW row);

/**
 * @brief Prints a ItemData structure.
 * @param self An allocated ItemData
 */
void itemDataPrint(ItemData *self);

/**
 * @brief Free an allocated ItemData structure and nullify the content of the pointer.
 * @param self A pointer to an allocated ItemData.
 */
void itemDataDestroy(ItemData **self);

/**
 * @brief Free an allocated ItemData structure
 * @param self A pointer to an allocated ItemData.
 */
void itemDataFree(ItemData *self);
