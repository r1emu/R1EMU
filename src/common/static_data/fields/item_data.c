/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @brief
 *
 *
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

// ---------- Includes ------------
#include "item_data.h"
#include "common/mysql/fields/mysql_item_data.h"

ItemData *itemDataNew(MYSQL_ROW row) {

    ItemData *self;

    if ((self = malloc(sizeof(ItemData))) == NULL) {
        return NULL;
    }

    if (!itemDataInit(self, row)) {
        itemDataDestroy(&self);
        error("ItemData failed to initialize.");
        return NULL;
    }

    return self;
}

bool itemDataInit(ItemData *self, MYSQL_ROW row) {

    memset(self, 0, sizeof(ItemData));

    int index = 0;

    self->ClassID = strtol(row[index++], NULL, 10);
    self->Weight = strtol(row[index++], NULL, 10);
    self->PR = strtol(row[index++], NULL, 10);
    self->DamageRange = strtol(row[index++], NULL, 10);
    self->ItemLv = strtol(row[index++], NULL, 10);
    self->PATK = strtol(row[index++], NULL, 10);
    self->ADD_MATK = strtol(row[index++], NULL, 10);
    self->ItemGrade = strtol(row[index++], NULL, 10);
    self->ItemStar = strtol(row[index++], NULL, 10);
    self->UseLv = strtol(row[index++], NULL, 10);
    self->MaterialPrice = strtol(row[index++], NULL, 10);
    self->Price = strtol(row[index++], NULL, 10);
    self->PriceRatio = strtol(row[index++], NULL, 10);
    self->SellPrice = strtol(row[index++], NULL, 10);
    self->RepairPriceRatio = strtol(row[index++], NULL, 10);
    self->MaxStack = strtol(row[index++], NULL, 10);
    self->Scale = strtol(row[index++], NULL, 10);
    self->Dur = strtol(row[index++], NULL, 10);
    self->MaxDur = strtol(row[index++], NULL, 10);
    self->repairPrice = strtol(row[index++], NULL, 10);
    self->UseCount = strtol(row[index++], NULL, 10);
    self->Hardness = strtol(row[index++], NULL, 10);
    self->ADD_MINATK = strtol(row[index++], NULL, 10);
    self->ADD_MAXATK = strtol(row[index++], NULL, 10);
    self->ADD_DEF = strtol(row[index++], NULL, 10);
    self->CRTHR = strtol(row[index++], NULL, 10);
    self->CRTATK = strtol(row[index++], NULL, 10);
    self->CRTDR = strtol(row[index++], NULL, 10);
    self->ADD_HR = strtol(row[index++], NULL, 10);
    self->ADD_DR = strtol(row[index++], NULL, 10);
    self->STR = strtol(row[index++], NULL, 10);
    self->DEX = strtol(row[index++], NULL, 10);
    self->CON = strtol(row[index++], NULL, 10);
    self->INT = strtol(row[index++], NULL, 10);
    self->MNA = strtol(row[index++], NULL, 10);
    self->SR = strtol(row[index++], NULL, 10);
    self->SDR = strtol(row[index++], NULL, 10);
    self->MHR = strtol(row[index++], NULL, 10);
    self->ADD_MDEF = strtol(row[index++], NULL, 10);
    self->MGP = strtol(row[index++], NULL, 10);
    self->AddSkillMaxR = strtol(row[index++], NULL, 10);
    self->SkillRange = strtol(row[index++], NULL, 10);
    self->SkillAngle = strtol(row[index++], NULL, 10);
    self->Luck = strtol(row[index++], NULL, 10);
    self->BlockRate = strtol(row[index++], NULL, 10);
    self->BLK = strtol(row[index++], NULL, 10);
    self->BLK_BREAK = strtol(row[index++], NULL, 10);
    self->Revive = strtol(row[index++], NULL, 10);
    self->HitCount = strtol(row[index++], NULL, 10);
    self->BackHit = strtol(row[index++], NULL, 10);
    self->SkillPower = strtol(row[index++], NULL, 10);
    self->ASPD = strtol(row[index++], NULL, 10);
    self->MSPD = strtol(row[index++], NULL, 10);
    self->KDPow = strtol(row[index++], NULL, 10);
    self->MHP = strtol(row[index++], NULL, 10);
    self->MSP = strtol(row[index++], NULL, 10);
    self->MSTA = strtol(row[index++], NULL, 10);
    self->RHP = strtol(row[index++], NULL, 10);
    self->RSP = strtol(row[index++], NULL, 10);
    self->RSPTIME = strtol(row[index++], NULL, 10);
    self->RSTA = strtol(row[index++], NULL, 10);
    self->ADD_CLOTH = strtol(row[index++], NULL, 10);
    self->ADD_LEATHER = strtol(row[index++], NULL, 10);
    self->ADD_CHAIN = strtol(row[index++], NULL, 10);
    self->ADD_IRON = strtol(row[index++], NULL, 10);
    self->ADD_GHOST = strtol(row[index++], NULL, 10);
    self->ADD_SMALLSIZE = strtol(row[index++], NULL, 10);
    self->ADD_MIDDLESIZE = strtol(row[index++], NULL, 10);
    self->ADD_LARGESIZE = strtol(row[index++], NULL, 10);
    self->ADD_FORESTER = strtol(row[index++], NULL, 10);
    self->ADD_WIDLING = strtol(row[index++], NULL, 10);
    self->ADD_VELIAS = strtol(row[index++], NULL, 10);
    self->ADD_PARAMUNE = strtol(row[index++], NULL, 10);
    self->ADD_KLAIDA = strtol(row[index++], NULL, 10);
    self->ADD_FIRE = strtol(row[index++], NULL, 10);
    self->ADD_ICE = strtol(row[index++], NULL, 10);
    self->ADD_POISON = strtol(row[index++], NULL, 10);
    self->ADD_LIGHTNING = strtol(row[index++], NULL, 10);
    self->ADD_EARTH = strtol(row[index++], NULL, 10);
    self->ADD_HOLY = strtol(row[index++], NULL, 10);
    self->ADD_DARK = strtol(row[index++], NULL, 10);
    self->BaseSocket = strtol(row[index++], NULL, 10);
    self->MaxSocket = strtol(row[index++], NULL, 10);
    self->MinOption = strtol(row[index++], NULL, 10);
    self->MaxOption = strtol(row[index++], NULL, 10);
    self->Aries = strtol(row[index++], NULL, 10);
    self->AriesDEF = strtol(row[index++], NULL, 10);
    self->Slash = strtol(row[index++], NULL, 10);
    self->SlashDEF = strtol(row[index++], NULL, 10);
    self->Strike = strtol(row[index++], NULL, 10);
    self->StrikeDEF = strtol(row[index++], NULL, 10);
    self->Aries_Range = strtol(row[index++], NULL, 10);
    self->Slash_Range = strtol(row[index++], NULL, 10);
    self->Strike_Range = strtol(row[index++], NULL, 10);
    self->MinRDmg = strtol(row[index++], NULL, 10);
    self->MaxRDmg = strtol(row[index++], NULL, 10);
    self->FDMinR = strtol(row[index++], NULL, 10);
    self->FDMaxR = strtol(row[index++], NULL, 10);
    self->TotalArrowCount = strtol(row[index++], NULL, 10);
    self->ArrowCount = strtol(row[index++], NULL, 10);
    self->RES_FIRE = strtol(row[index++], NULL, 10);
    self->RES_ICE = strtol(row[index++], NULL, 10);
    self->RES_POISON = strtol(row[index++], NULL, 10);
    self->RES_LIGHTNING = strtol(row[index++], NULL, 10);
    self->RES_EARTH = strtol(row[index++], NULL, 10);
    self->RES_HOLY = strtol(row[index++], NULL, 10);
    self->RES_DARK = strtol(row[index++], NULL, 10);
    self->BaseSocket_MA = strtol(row[index++], NULL, 10);
    self->MaxSocket_MA = strtol(row[index++], NULL, 10);

    self->ClassName = strdup(row[index++]);
    self->Name = strdup(row[index++]);
    self->ItemType = strdup(row[index++]);
    self->Journal = strdup(row[index++]);
    self->GroupName = strdup(row[index++]);
    self->EquipGroup = strdup(row[index++]);
    self->EquipXpGroup = strdup(row[index++]);
    self->ToolTipScp = strdup(row[index++]);
    self->BasicTooltipProp = strdup(row[index++]);
    self->TooltipImage = strdup(row[index++]);
    self->Icon = strdup(row[index++]);
    self->ReqToolTip = strdup(row[index++]);
    self->Desc = strdup(row[index++]);
    self->Destroyable = strdup(row[index++]);
    self->ShopTrade = strdup(row[index++]);
    self->UserTrade = strdup(row[index++]);
    self->DropStyle = strdup(row[index++]);
    self->Usable = strdup(row[index++]);
    self->Consumable = strdup(row[index++]);
    self->UseTx = strdup(row[index++]);
    self->ClassType = strdup(row[index++]);
    self->ClassType2 = strdup(row[index++]);
    self->Attribute = strdup(row[index++]);
    self->AttackType = strdup(row[index++]);
    self->Material = strdup(row[index++]);
    self->UseJob = strdup(row[index++]);
    self->UseGender = strdup(row[index++]);
    self->DBLHand = strdup(row[index++]);
    self->EqpType = strdup(row[index++]);
    self->DefaultEqpSlot = strdup(row[index++]);

    return true;
}

void itemDataPrint(ItemData *self) {
    dbg("==== ItemData %p ====", self);
    dbg("ClassID = %d", self->ClassID);
    dbg("Weight = %d", self->Weight);
    dbg("PR = %d", self->PR);
    dbg("DamageRange = %d", self->DamageRange);
    dbg("ItemLv = %d", self->ItemLv);
    dbg("PATK = %d", self->PATK);
    dbg("ADD_MATK = %d", self->ADD_MATK);
    dbg("ItemGrade = %d", self->ItemGrade);
    dbg("ItemStar = %d", self->ItemStar);
    dbg("UseLv = %d", self->UseLv);
    dbg("MaterialPrice = %d", self->MaterialPrice);
    dbg("Price = %d", self->Price);
    dbg("PriceRatio = %d", self->PriceRatio);
    dbg("SellPrice = %d", self->SellPrice);
    dbg("RepairPriceRatio = %d", self->RepairPriceRatio);
    dbg("MaxStack = %d", self->MaxStack);
    dbg("Scale = %d", self->Scale);
    dbg("Dur = %d", self->Dur);
    dbg("MaxDur = %d", self->MaxDur);
    dbg("repairPrice = %d", self->repairPrice);
    dbg("UseCount = %d", self->UseCount);
    dbg("Hardness = %d", self->Hardness);
    dbg("ADD_MINATK = %d", self->ADD_MINATK);
    dbg("ADD_MAXATK = %d", self->ADD_MAXATK);
    dbg("ADD_DEF = %d", self->ADD_DEF);
    dbg("CRTHR = %d", self->CRTHR);
    dbg("CRTATK = %d", self->CRTATK);
    dbg("CRTDR = %d", self->CRTDR);
    dbg("ADD_HR = %d", self->ADD_HR);
    dbg("ADD_DR = %d", self->ADD_DR);
    dbg("STR = %d", self->STR);
    dbg("DEX = %d", self->DEX);
    dbg("CON = %d", self->CON);
    dbg("INT = %d", self->INT);
    dbg("MNA = %d", self->MNA);
    dbg("SR = %d", self->SR);
    dbg("SDR = %d", self->SDR);
    dbg("MHR = %d", self->MHR);
    dbg("ADD_MDEF = %d", self->ADD_MDEF);
    dbg("MGP = %d", self->MGP);
    dbg("AddSkillMaxR = %d", self->AddSkillMaxR);
    dbg("SkillRange = %d", self->SkillRange);
    dbg("SkillAngle = %d", self->SkillAngle);
    dbg("Luck = %d", self->Luck);
    dbg("BlockRate = %d", self->BlockRate);
    dbg("BLK = %d", self->BLK);
    dbg("BLK_BREAK = %d", self->BLK_BREAK);
    dbg("Revive = %d", self->Revive);
    dbg("HitCount = %d", self->HitCount);
    dbg("BackHit = %d", self->BackHit);
    dbg("SkillPower = %d", self->SkillPower);
    dbg("ASPD = %d", self->ASPD);
    dbg("MSPD = %d", self->MSPD);
    dbg("KDPow = %d", self->KDPow);
    dbg("MHP = %d", self->MHP);
    dbg("MSP = %d", self->MSP);
    dbg("MSTA = %d", self->MSTA);
    dbg("RHP = %d", self->RHP);
    dbg("RSP = %d", self->RSP);
    dbg("RSPTIME = %d", self->RSPTIME);
    dbg("RSTA = %d", self->RSTA);
    dbg("ADD_CLOTH = %d", self->ADD_CLOTH);
    dbg("ADD_LEATHER = %d", self->ADD_LEATHER);
    dbg("ADD_CHAIN = %d", self->ADD_CHAIN);
    dbg("ADD_IRON = %d", self->ADD_IRON);
    dbg("ADD_GHOST = %d", self->ADD_GHOST);
    dbg("ADD_SMALLSIZE = %d", self->ADD_SMALLSIZE);
    dbg("ADD_MIDDLESIZE = %d", self->ADD_MIDDLESIZE);
    dbg("ADD_LARGESIZE = %d", self->ADD_LARGESIZE);
    dbg("ADD_FORESTER = %d", self->ADD_FORESTER);
    dbg("ADD_WIDLING = %d", self->ADD_WIDLING);
    dbg("ADD_VELIAS = %d", self->ADD_VELIAS);
    dbg("ADD_PARAMUNE = %d", self->ADD_PARAMUNE);
    dbg("ADD_KLAIDA = %d", self->ADD_KLAIDA);
    dbg("ADD_FIRE = %d", self->ADD_FIRE);
    dbg("ADD_ICE = %d", self->ADD_ICE);
    dbg("ADD_POISON = %d", self->ADD_POISON);
    dbg("ADD_LIGHTNING = %d", self->ADD_LIGHTNING);
    dbg("ADD_EARTH = %d", self->ADD_EARTH);
    dbg("ADD_HOLY = %d", self->ADD_HOLY);
    dbg("ADD_DARK = %d", self->ADD_DARK);
    dbg("BaseSocket = %d", self->BaseSocket);
    dbg("MaxSocket = %d", self->MaxSocket);
    dbg("MinOption = %d", self->MinOption);
    dbg("MaxOption = %d", self->MaxOption);
    dbg("Aries = %d", self->Aries);
    dbg("AriesDEF = %d", self->AriesDEF);
    dbg("Slash = %d", self->Slash);
    dbg("SlashDEF = %d", self->SlashDEF);
    dbg("Strike = %d", self->Strike);
    dbg("StrikeDEF = %d", self->StrikeDEF);
    dbg("Aries_Range = %d", self->Aries_Range);
    dbg("Slash_Range = %d", self->Slash_Range);
    dbg("Strike_Range = %d", self->Strike_Range);
    dbg("MinRDmg = %d", self->MinRDmg);
    dbg("MaxRDmg = %d", self->MaxRDmg);
    dbg("FDMinR = %d", self->FDMinR);
    dbg("FDMaxR = %d", self->FDMaxR);
    dbg("TotalArrowCount = %d", self->TotalArrowCount);
    dbg("ArrowCount = %d", self->ArrowCount);
    dbg("RES_FIRE = %d", self->RES_FIRE);
    dbg("RES_ICE = %d", self->RES_ICE);
    dbg("RES_POISON = %d", self->RES_POISON);
    dbg("RES_LIGHTNING = %d", self->RES_LIGHTNING);
    dbg("RES_EARTH = %d", self->RES_EARTH);
    dbg("RES_HOLY = %d", self->RES_HOLY);
    dbg("RES_DARK = %d", self->RES_DARK);
    dbg("BaseSocket_MA = %d", self->BaseSocket_MA);
    dbg("MaxSocket_MA = %d", self->MaxSocket_MA);
    dbg("ClassName = %s", self->ClassName);
    dbg("Name = %s", self->Name);
    dbg("ItemType = %s", self->ItemType);
    dbg("Journal = %s", self->Journal);
    dbg("GroupName = %s", self->GroupName);
    dbg("EquipGroup = %s", self->EquipGroup);
    dbg("EquipXpGroup = %s", self->EquipXpGroup);
    dbg("ToolTipScp = %s", self->ToolTipScp);
    dbg("BasicTooltipProp = %s", self->BasicTooltipProp);
    dbg("TooltipImage = %s", self->TooltipImage);
    dbg("Icon = %s", self->Icon);
    dbg("ReqToolTip = %s", self->ReqToolTip);
    dbg("Desc = %s", self->Desc);
    dbg("Destroyable = %s", self->Destroyable);
    dbg("ShopTrade = %s", self->ShopTrade);
    dbg("UserTrade = %s", self->UserTrade);
    dbg("DropStyle = %s", self->DropStyle);
    dbg("Usable = %s", self->Usable);
    dbg("Consumable = %s", self->Consumable);
    dbg("UseTx = %s", self->UseTx);
    dbg("ClassType = %s", self->ClassType);
    dbg("ClassType2 = %s", self->ClassType2);
    dbg("Attribute = %s", self->Attribute);
    dbg("AttackType = %s", self->AttackType);
    dbg("Material = %s", self->Material);
    dbg("UseJob = %s", self->UseJob);
    dbg("UseGender = %s", self->UseGender);
    dbg("DBLHand = %s", self->DBLHand);
    dbg("EqpType = %s", self->EqpType);
    dbg("DefaultEqpSlot = %s", self->DefaultEqpSlot);
}

void itemDataFree(ItemData *self) {
}

void itemDataDestroy(ItemData **_self) {
    ItemData *self = *_self;

    if (_self && self) {
        itemDataFree(self);
        free(self);
        *_self = NULL;
    }
}
