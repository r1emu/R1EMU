/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file db.h
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
typedef struct _DbObject {
    size_t dataSize;
    void *data;
    bool sharedMemory;
}   DbObject;

// ----------- Functions ------------

/**
 * Allocate a new DbObject structure.
 * @param dataSize The size of the data pointed by \data
 * @param data the data to store into the object
 * @param sharedMemory if false, dbObject will allocate and copy data to its own memory.
 *                     if true, the object is shared
 * @return A pointer to an allocated DbObject, or NULL if an error occurred.
 */
DbObject *dbObjectNew(size_t dataSize, void *data, bool sharedMemory);

/**
 * Initialize an allocated DbObject structure.
 * @param self An allocated DbObject to initialize.
 * @return true on success, false otherwise.
 */
bool dbObjectInit(DbObject *self, size_t dataSize, void *data, bool sharedMemory);

/**
 * Free an allocated DbObject structure.
 * @param self A pointer to an allocated Db.
 */
void dbObjectFree(DbObject *self);

/**
 * Free an allocated DbObject structure and nullify the content of the pointer.
 * @param self A pointer to an allocated Db.
 */
void dbObjectDestroy(DbObject **_self);
