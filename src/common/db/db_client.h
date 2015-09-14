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

// ---------- Defines -------------

typedef enum {
    TESTKEY
} DbKey;

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
 * Get by value from database by key
 * @param self A connected dbClient
 * @param key A key from DbKey enum
 * @param value A pointer to a string which will become the corresponding
 *        value of the key
 * @return true on success, false otherwise.
 */
bool dbClientGetValue(DbClient *self, DbKey key, char **value);

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
