/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file actor.h
 * @brief
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#pragma once

// ---------- Includes ------------
#include "R1EMU.h"

// ---------- Defines -------------
#define ACTOR_KEY_SIZE 17


// ------ Structure declaration -------
typedef uint8_t ActorKey[ACTOR_KEY_SIZE];
typedef uint64_t ActorId_t;

typedef struct Actor {
   ActorId_t uid;
} Actor;


// ----------- Functions ------------
bool actorInit(Actor *self, ActorId_t id);

/**
 * Getters & Setters
 */
inline ActorId_t actorGetUId(Actor *self) {
    return self->uid;
}

void actorGenKey(ActorId_t uid, ActorKey key);
