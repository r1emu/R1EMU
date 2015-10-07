/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file npc.h
 * @brief
 *
 *
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#pragma once

// ---------- Includes ------------
#include "R1EMU.h"
#include "common/actor/actor.h"

// ---------- Defines -------------


// ------ Structure declaration -------
typedef uint32_t NpcId_t;

typedef struct {
    Actor actor;
    NpcId_t id;
} Npc;

// ----------- Functions ------------

/**
 * Allocate a new Npc structure.
 * @return A pointer to an allocated Npc, or NULL if an error occured.
 */
Npc *npcNew(Actor *actor);

/**
 * Initialize an allocated Npc structure.
 * @param self An allocated Npc to initialize.
 * @return true on success, false otherwise.
 */
bool npcInit(Npc *self, Actor *actor);

/**
 * Free an allocated Npc structure.
 * @param self A pointer to an allocated Npc.
 */
void npcFree(Npc *self);

/**
 * Free an allocated Npc structure and nullify the content of the pointer.
 * @param self A pointer to an allocated Npc.
 */
void npcDestroy(Npc **self);

