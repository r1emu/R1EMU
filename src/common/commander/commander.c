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

#include "commander.h"

bool commanderEquipmentCPacketInit(CommanderEquipmentCPacket *equipment) {
    equipment->head_top = 2;
    equipment->head_middle = 2;
    equipment->itemUnk1 = 4;
    equipment->body_armor = 0x81a9d;
    equipment->gloves = 6;
    equipment->boots = 7;
    equipment->helmet = 0x2710;
    equipment->bracelet = 0x2af8;
    equipment->weapon = 0x3118d;
    equipment->shield = 0x98967c;
    equipment->costume = 4;
    equipment->itemUnk3 = 9;
    equipment->itemUnk4 = 9;
    equipment->itemUnk5 = 4;
    equipment->leg_armor = 0x7f38d;
    equipment->itemUnk6 = 9;
    equipment->itemUnk7 = 9;
    equipment->ring_left = 9;
    equipment->ring_right = 9;
    equipment->necklace = 0xa;

    return true;
}

Commander *commanderNew(void) {
    Commander *self;

    if ((self = calloc(1, sizeof(Commander))) == NULL) {
        error("Cannot allocate a new commander.");
        return NULL;
    }

    if (!commanderInit (self)) {
        commanderDestroy (&self);
        error("Router failed to initialize.");
        return NULL;
    }

    return self;
}

Commander *commanderDup(Commander *src) {
    Commander *dest = NULL;

    if (!(dest = commanderNew())) {
        error("Cannot allocate a new commander.");
        return NULL;
    }

    memcpy(dest, src, sizeof(*src));

    return dest;
}

bool commanderInit(Commander *commander) {
    memset(commander, 0, sizeof(*commander));

    commander->accountId = -1;
    commander->socialInfoId = -1;
    commander->commanderId = -1;

    commander->classId = COMMANDER_CLASS_CLERIC;
    commander->jobId = COMMANDER_JOB_CLERIC; // Cleric
    commander->gender = COMMANDER_GENDER_FEMALE; // Female

    commander->level = 1;
    commander->hairId = 0x10;
    commander->pose = SWAP_UINT16(0x0000); // Idle (ICBT)

    commander->pos = PositionXYZ_decl(-623, 260, -1044); // Default West Siauliai Woods start position
    commander->dir = PositionXZ_decl(0.0, 0.0); // Default West Siauliai Woods start position
    commander->currentXP = 0;
    commander->maxXP = 0xC; // ICBT
    commander->pcId = -1;
    commander->currentHP = 238;
    commander->maxHP = 238;
    commander->currentSP = 105;
    commander->maxSP = 105;
    commander->currentStamina = 25000;
    commander->maxStamina = 25000;

    if (!(inventoryInit(&commander->inventory))) {
        error("Cannot initialize inventory.");
        return false;
    }

    if (!(skillsManagerInit(&commander->skillsManager))) {
        error("Cannot initialize inventory.");
        return false;
    }

    return true;
}

void commanderEquipmentPrint(CommanderEquipmentCPacket *equipment) {
    dbg("head_top = %d (%x)", equipment->head_top, equipment->head_top);
    dbg("head_middle = %d (%x)", equipment->head_middle, equipment->head_middle);
    dbg("itemUnk1 = %d (%x)", equipment->itemUnk1, equipment->itemUnk1);
    dbg("body_armor = %d (%x)", equipment->body_armor, equipment->body_armor);
    dbg("gloves = %d (%x)", equipment->gloves, equipment->gloves);
    dbg("boots = %d (%x)", equipment->boots, equipment->boots);
    dbg("helmet = %d (%x)", equipment->helmet, equipment->helmet);
    dbg("bracelet = %d (%x)", equipment->bracelet, equipment->bracelet);
    dbg("weapon = %d (%x)", equipment->weapon, equipment->weapon);
    dbg("shield = %d (%x)", equipment->shield, equipment->shield);
    dbg("costume = %d (%x)", equipment->costume, equipment->costume);
    dbg("itemUnk3 = %d (%x)", equipment->itemUnk3, equipment->itemUnk3);
    dbg("itemUnk4 = %d (%x)", equipment->itemUnk4, equipment->itemUnk4);
    dbg("itemUnk5 = %d (%x)", equipment->itemUnk5, equipment->itemUnk5);
    dbg("leg_armor = %d (%x)", equipment->leg_armor, equipment->leg_armor);
    dbg("itemUnk6 = %d (%x)", equipment->itemUnk6, equipment->itemUnk6);
    dbg("itemUnk7 = %d (%x)", equipment->itemUnk7, equipment->itemUnk7);
    dbg("ring_left = %d (%x)", equipment->ring_left, equipment->ring_left);
    dbg("ring_right = %d (%x)", equipment->ring_right, equipment->ring_right);
    dbg("necklace = %d (%x)", equipment->necklace, equipment->necklace);
}

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
    CommanderPose_t pose)
{
    memset(appearance, 0, sizeof(*appearance));

    memcpy(appearance->familyName, familyName, sizeof(appearance->familyName));
    memcpy(appearance->commanderName, commanderName, sizeof(appearance->commanderName));
    appearance->accountId = accountId;
    appearance->classId = classId;
    appearance->unk4 = SWAP_UINT16(0x2220); // ICBT
    appearance->jobId = jobId;
    appearance->gender = gender;
    appearance->unk5 = 0; // ICBT
    appearance->level = level;
    commanderEquipmentCPacketInit(&appearance->equipment);
    appearance->hairId = hairId;
    appearance->unk6 = 0;
    appearance->pose = pose;

    /*
    appearance->accountId = -1;
    appearance->classId = COMMANDER_CLASS_CLERIC;
    appearance->unk4 = SWAP_UINT16(0x2220); // ICBT
    appearance->jobId = COMMANDER_JOB_CLERIC; // Cleric
    appearance->gender = COMMANDER_GENDER_FEMALE; // Female
    appearance->unk5 = 0; // ICBT
    appearance->level = 1;
    commanderEquipmentCPacketInit(&appearance->equipment);
    appearance->hairId = 0x10;
    appearance->pose = SWAP_UINT16(0x0000); // Idle (ICBT)
    */

    return true;
}


void commanderPrint(Commander *commander) {
    dbg("commanderName = %s", commander->commanderName);
    dbg("familyName = %s", commander->familyName);
    dbg("accountId = %llu (%llx)", commander->accountId, commander->accountId);
    dbg("classId = %d (%x)", commander->classId, commander->classId);
    dbg("jobId = %d (%x)", commander->jobId, commander->jobId);
    dbg("gender = %d (%x)", commander->gender, commander->gender);
    dbg("level = %d (%x)", commander->level, commander->level);
    dbg("hairId = %d (%x)", commander->hairId, commander->hairId);
    dbg("pose = %d (%x)", commander->pose, commander->pose);
    dbg("posX = %f %f %f (%x %x %x)",
         commander->pos.x, commander->pos.y, commander->pos.z,
         commander->pos.x, commander->pos.y, commander->pos.z);
    dbg("currentXP = %d (%x)", commander->currentXP, commander->currentXP);
    dbg("maxXP = %d (%x)", commander->maxXP, commander->maxXP);
    dbg("socialInfoId = %llu (%llx)", commander->socialInfoId, commander->socialInfoId);
    dbg("commanderId = %llu (%llx)", commander->commanderId, commander->commanderId);
    dbg("pcId = %llu (%llx)", commander->pcId, commander->pcId);
    dbg("currentHP = %d (%x)", commander->currentHP, commander->currentHP);
    dbg("maxHP = %d (%x)", commander->maxHP, commander->maxHP);
    dbg("currentSP = %d (%x)", commander->currentSP, commander->currentSP);
    dbg("maxSP = %d (%x)", commander->maxSP, commander->maxSP);
    dbg("currentStamina = %d (%x)", commander->currentStamina, commander->currentStamina);
    dbg("maxStamina = %d (%x)", commander->maxStamina, commander->maxStamina);
}

void commanderFree(Commander *self) {
    // TODO : free inventory?
}

void commanderDestroy(Commander **_self) {
    Commander *self = *_self;

    if (_self && self) {
        commanderFree(self);
        free(self);
        *_self = NULL;
    }
}

size_t commanderGetSPacketSize(Commander *self) {
    size_t size = 0;
    size += sizeof(CommanderSPacket);
    size += inventoryGetSPacketSize(&self->inventory);
    return size;
}

void commanderSerializeSPacket(Commander *self, PacketStream *stream) {

    packetStreamDebugStart(stream, commanderGetSPacketSize(self));
    packetStreamIn(stream, self->commanderName);
    packetStreamIn(stream, self->familyName);
    packetStreamIn(stream, &self->gender);
    packetStreamIn(stream, &self->hairId);
    packetStreamIn(stream, &self->pose);
    packetStreamIn(stream, &self->pos);
    packetStreamIn(stream, &self->dir);
    packetStreamIn(stream, &self->classId);
    packetStreamIn(stream, &self->jobId);
    packetStreamIn(stream, &self->accountId);
    packetStreamIn(stream, &self->pcId);
    packetStreamIn(stream, &self->socialInfoId);
    packetStreamIn(stream, &self->commanderId);
    packetStreamIn(stream, &self->mapId);
    packetStreamIn(stream, &self->level);
    packetStreamIn(stream, &self->currentXP);
    packetStreamIn(stream, &self->maxXP);
    packetStreamIn(stream, &self->currentHP);
    packetStreamIn(stream, &self->maxHP);
    packetStreamIn(stream, &self->currentSP);
    packetStreamIn(stream, &self->maxSP);
    packetStreamIn(stream, &self->currentStamina);
    packetStreamIn(stream, &self->maxStamina);
    inventorySerializeSPacket(&self->inventory, stream);
    packetStreamDebugEnd(stream);
}

bool commanderUnserializeSPacket(Commander *self, PacketStream *stream) {

    packetStreamDebugStart(stream, stream->size);

    if (!commanderInit(self)) {
        error("Cannot initialize a commander to unserialize.");
        return false;
    }

    packetStreamOut(stream, self->commanderName);
    packetStreamOut(stream, self->familyName);
    packetStreamOut(stream, &self->gender);
    packetStreamOut(stream, &self->hairId);
    packetStreamOut(stream, &self->pose);
    packetStreamOut(stream, &self->pos);
    packetStreamOut(stream, &self->dir);
    packetStreamOut(stream, &self->classId);
    packetStreamOut(stream, &self->jobId);
    packetStreamOut(stream, &self->accountId);
    packetStreamOut(stream, &self->pcId);
    packetStreamOut(stream, &self->socialInfoId);
    packetStreamOut(stream, &self->commanderId);
    packetStreamOut(stream, &self->mapId);
    packetStreamOut(stream, &self->level);
    packetStreamOut(stream, &self->currentXP);
    packetStreamOut(stream, &self->maxXP);
    packetStreamOut(stream, &self->currentHP);
    packetStreamOut(stream, &self->maxHP);
    packetStreamOut(stream, &self->currentSP);
    packetStreamOut(stream, &self->maxSP);
    packetStreamOut(stream, &self->currentStamina);
    packetStreamOut(stream, &self->maxStamina);

    if (!(inventoryUnserializeSPacket(&self->inventory, stream))) {
        error("Cannot unserialize inventory.");
        return false;
    }

    packetStreamDebugEnd(stream);

    return true;
}
