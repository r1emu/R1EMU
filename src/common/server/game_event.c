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

#include "game_event.h"


/**
 * Allocate a new GameEvent structure.
 * @return A pointer to an allocated GameEvent, or NULL if an error occured.
 */
GameEvent *gameEventNew(uint8_t *sessionKey, EventType type, void *event, size_t eventSize) {
    GameEvent *self;

    if ((self = malloc(sizeof(GameEvent))) == NULL) {
        return NULL;
    }

    if (!gameEventInit(self, sessionKey, type, event, eventSize)) {
        gameEventDestroy(&self);
        error("GameEvent failed to initialize.");
        return NULL;
    }

    return self;
}

/**
 * Initialize an allocated GameEvent structure.
 * @param self An allocated GameEvent to initialize.
 * @return true on success, false otherwise.
 */
bool gameEventInit(GameEvent *self, uint8_t *sessionKey, EventType type, void *event, size_t eventSize) {
    memset(self, 0, sizeof(GameEvent));

    return true;
}

/**
 * Free an allocated GameEvent structure.
 * @param self A pointer to an allocated GameEvent.
 */
void gameEventFree(GameEvent *self) {
    // TODO
}

/**
 * Free an allocated GameEvent structure and nullify the content of the pointer.
 * @param self A pointer to an allocated GameEvent.
 */
void gameEventDestroy(GameEvent **_self) {
    GameEvent *self = *_self;

    if (_self && self) {
        gameEventFree(self);
        free(self);
        *_self = NULL;
    }
}
