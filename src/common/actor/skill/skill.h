/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file skill.h
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

// ---------- Defines -------------


// ------ Structure declaration -------
typedef uint32_t SkillId_t;
typedef uint32_t SkillLevel_t;

typedef struct {

} Skill;

// ----------- Functions ------------

/**
 * Allocate a new Skill structure.
 * @return A pointer to an allocated Skill, or NULL if an error occured.
 */
Skill *skillNew(void);

/**
 * Initialize an allocated Skill structure.
 * @param self An allocated Skill to initialize.
 * @return true on success, false otherwise.
 */
bool skillInit(Skill *self);

/**
 * Free an allocated Skill structure.
 * @param self A pointer to an allocated Skill.
 */
void skillFree(Skill *self);

/**
 * Free an allocated Skill structure and nullify the content of the pointer.
 * @param self A pointer to an allocated Skill.
 */
void skillDestroy(Skill **self);

