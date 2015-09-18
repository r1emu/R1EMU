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
    char data[0];
}   DbObject;

// ----------- Functions ------------

/**
 * Allocate a new DbObject structure.
 * @return A pointer to an allocated DbObject, or NULL if an error occured.
 */
DbObject *dbObjectNew(size_t dataSize, void *data);

/**
 * Initialize an allocated DbObject structure.
 * @param self An allocated DbObject to initialize.
 * @return true on success, false otherwise.
 */
bool dbObjectInit(DbObject *self, size_t dataSize, void *data);

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
