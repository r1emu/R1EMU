/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file skills_manager.h
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
#include "common/actor/skill/skill.h"

// ---------- Defines -------------

// ------ Structure declaration -------

typedef struct SkillsManager SkillsManager;

/**
 * @brief SkillsManager contains
 */
struct SkillsManager
{
    zhash_t *skills;
};

// ----------- Functions ------------
/**
 * Allocate a new SkillsManager structure.
 * @return A pointer to an allocated SkillsManager, or NULL if an error occured.
 */
SkillsManager *skillsManagerNew(void);

/**
 * Initialize an allocated SkillsManager structure.
 * @param self An allocated SkillsManager to initialize.
 * @return true on success, false otherwise.
 */
bool skillsManagerInit(SkillsManager *self);

/**
 * Free an allocated SkillsManager structure.
 * @param self A pointer to an allocated SkillsManager.
 */
void skillsManagerFree(SkillsManager *self);

/**
 * Free an allocated SkillsManager structure and nullify the content of the pointer.
 * @param self A pointer to an allocated SkillsManager.
 */
void skillsManagerDestroy(SkillsManager **self);

/**
 * Adds an skill to skillsManager
 * @param self A pointer to an allocated SkillsManager.
 * @param skill an Skill.
 */
bool skillsManagerAddSkill(SkillsManager *self, Skill *itemToAdd);
bool skillsManagerRemoveSkill(SkillsManager *self, Skill *itemToRemove);
