/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file db_client.h
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
#include "db_object.h"

// ---------- Defines -------------

// ------ Structure declaration -------
typedef struct DbClient DbClient;

typedef struct DbClientInfo {
    char *name;
    uint16_t routerId;
}   DbClientInfo;

// ----------- Functions ------------

/**
 * Allocate a new DbClient structure.
 * @return A pointer to an allocated DbClient, or NULL if an error occured.
 */
DbClient *dbClientNew(DbClientInfo *startInfo);

/**
 * Initialize an allocated DbClient structure.
 * @param self An allocated DbClient to initialize.
 * @return true on success, false otherwise.
 */
bool dbClientInit(DbClient *self, DbClientInfo *startInfo);

/**
 * Allocate a new DbClientInfo structure.
 * @return A pointer to an allocated DbClientInfo, or NULL if an error occured.
 */
DbClientInfo *dbClientInfoNew(char *name, uint16_t routerId);

/**
 * Initialize an allocated DbClientInfo structure.
 * @param self An allocated DbClientInfo to initialize.
 * @return true on success, false otherwise.
 */
bool dbClientInfoInit(DbClientInfo *self, char *name, uint16_t routerId);

/**
 * Request value from database by key
 * @param self A connected dbClient
 * @param keys Array of keys
 * @param keysCount number of keys
 * @return true on success, false otherwise.
 */
bool dbClientRequestValue(DbClient *self, char *key);
bool dbClientRequestValues(DbClient *self, char **keys, size_t keysCount);

/**
 * Get value from Db previously requested
 * @param self A connected dbClient
 * @param out A pointer to an unallocated zhash pointer.
              Contains the hashtable of <key, DbObject *> if the function success
 * @return true on success, false otherwise.
 */
bool dbClientGetValues(DbClient *self, zhash_t **_out);
bool dbClientGetValue(DbClient *self, DbObject **out);

/**
 * Remove a value from Db
 * @param self A connected dbClient
 * @param keys Array of keys
 * @param keysCount number of keys
 * @return true on success, false otherwise.
 */
bool dbClientRemoveValues(DbClient *self, char **keys, size_t keysCount);
bool dbClientRemoveValue(DbClient *self, char *key);

/**
 * Update a value from Db
 * @param self A connected dbClient
 * @param objects A hashtable <char *key, DbObject *object> to update
 * @return true on success, false otherwise.
 */
bool dbClientUpdateValues(DbClient *self, zhash_t *objects);
bool dbClientUpdateValue(DbClient *self, char *key, DbObject *object);
/**
 * Free an allocated DbClient structure.
 * @param self A pointer to an allocated DbClient.
 */
void dbClientFree(DbClient *self);

/**
 * Free an allocated DbClient structure and nullify the content of the pointer.
 * @param self A pointer to an allocated DbClient.
 */
void dbClientDestroy(DbClient **self);

/**
 * Free an allocated DbClientInfo structure.
 * @param self A pointer to an allocated DbClientInfo.
 */
void dbClientInfoFree(DbClientInfo *self);

/**
 * Free an allocated DbClientInfo structure and nullify the content of the pointer.
 * @param self A pointer to an allocated DbClientInfo.
 */
void dbClientInfoDestroy(DbClientInfo **self);
