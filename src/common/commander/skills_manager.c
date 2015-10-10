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


// ---------- Includes ------------
#include "skills_manager.h"

// ------ Structure declaration -------

// ------ Static declaration -------


// ------ Extern function implementation -------

SkillsManager* skillsManagerNew(void) {
    SkillsManager *self;

    if ((self = malloc(sizeof(SkillsManager))) == NULL) {
        return NULL;
    }

    if (!skillsManagerInit(self)) {
        skillsManagerDestroy(&self);
        error("SkillsManager failed to initialize.");
        return NULL;
    }

    return self;
}

bool skillsManagerInit(SkillsManager *self) {
    memset(self, 0, sizeof(SkillsManager));

    // Initialize hash for skills storage
    if (!(self->skills = zhash_new())) {
        error("Cannot allocate hashtable for skills.");
        return false;
    }

    return true;
}

void skillsManagerFree(SkillsManager *self) {
    // Destroy hashtable of skills
    zhash_destroy (&self->skills);
}

void skillsManagerDestroy(SkillsManager **_self) {
    SkillsManager *self = *_self;

    if (_self && self) {
        skillsManagerFree(self);
        free(self);
        *_self = NULL;
    }
}
