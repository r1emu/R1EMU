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
