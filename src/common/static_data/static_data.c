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
#include "static_data.h"

// extend debug messages
#define sdError(self, x, ...) error("[%s] " x, self->name, ##__VA_ARGS__)
#define sdInfo(self, x, ...)  info("[%s] " x, self->name, ##__VA_ARGS__)
#define sdSpecial(self, x, ...)  special("[%s] " x, self->name, ##__VA_ARGS__)
#define sdDbg(self, x, ...)  dbg("[%s] " x, self->name, ##__VA_ARGS__)

typedef uint8_t StaticDataKey[sizeof(StaticDataId) + 1];

/**
 * @brief StaticData is a database of items that is loaded at startup and locked when it is finished.
 */
struct StaticData {
    char *name;
    zhash_t *hashtable; /** <char *keyId, void *object> */
    bool locked;
};

StaticData *staticDataNew(char *name) {

    StaticData *self;

    if ((self = malloc(sizeof(StaticData))) == NULL) {
        return NULL;
    }

    if (!staticDataInit(self, name)) {
        staticDataDestroy(&self);
        error("StaticData failed to initialize.");
        return NULL;
    }

    return self;
}

bool staticDataInit(StaticData *self, char *name) {

    memset(self, 0, sizeof(StaticData));

    self->locked = false;
    self->name = strdup(name);

    if (!(self->hashtable = zhash_new())) {
        sdError(self, "Cannot allocate a new hashtable.");
        return false;
    }

    return true;
}

void staticDataLock(StaticData *self) {
    self->locked = true;
}

static void staticDataGenKey(StaticData *self, StaticDataId id, StaticDataKey key) {
    snprintf(key, (sizeof(id) * 2) + 1, "%llx", id);
}

void staticDataDebug(StaticData *self, void (*printer)(void *object)) {
    for (void *object = zhash_first(self->hashtable); object != NULL; object = zhash_next(self->hashtable)) {
        printer(object);
    }
}

bool staticDataAdd(StaticData *self, StaticDataId id, void *object) {

    if (self->locked) {
        sdError(self, "StaticData has been locked and cannot be modified anymore.");
        return false;
    }

    StaticDataKey key;
    staticDataGenKey(self, id, key);

    if (zhash_insert(self->hashtable, key, object) != 0) {
        sdError(self, "Cannot insert the object '%s'", key);
        return false;
    }

    return true;
}

bool staticDataGet(StaticData *self, StaticDataId id, void *_out) {

    void **out = _out;

    if (!self->locked) {
        sdError(self, "StaticData should have been locked before querying it.");
        return false;
    }

    StaticDataKey key;
    staticDataGenKey(self, id, key);

    void *object = NULL;
    if (!(object = zhash_lookup(self->hashtable, key))) {
        sdError(self, "Cannot find the static data for object '%s'", key);
        return false;
    }

    *out = object;
    return true;
}

void staticDataPrint(StaticData *self) {
    sdDbg(self, "==== StaticData %p ====", self);
}

void staticDataFree(StaticData *self) {
    zhash_destroy(&self->hashtable);
}

void staticDataDestroy(StaticData **_self) {
    StaticData *self = *_self;

    if (_self && self) {
        staticDataFree(self);
        free(self);
        *_self = NULL;
    }
}
