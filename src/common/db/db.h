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
#include "db_client.h"
#include "common/packet/packet.h"

// ---------- Defines -------------
#define DB_ENDPOINT "inproc://db-%s-%d"

// ------ Structure declaration -------
typedef struct Db Db;

typedef bool (*DbProcessMsgHandler)(void *heritage, zmsg_t *msg, zmsg_t *out);

typedef struct DbInfo {
    char *name;
    uint16_t routerId;
    void *heritage;
    DbProcessMsgHandler handler;
}   DbInfo;

typedef enum {
    // format :
    // Request : [key1][object1] [key2][object2] [...][...]
    // Answer : [status]
    DB_UPDATE_OBJECT,
    DB_UPDATE_ARRAY,

    // format : [key1][key2][...]
    // Answer : [status][object1][object2][...]
    DB_GET_OBJECT,
    DB_GET_ARRAY,

    // format : [key1][key2][...]
    // Answer : [status]
    DB_REMOVE_OBJECT,
    DB_REMOVE_ARRAY,
} DbPacketType;

typedef enum {
    DB_STATUS_SUCCESS,
    DB_STATUS_CANNOT_UPDATE,
    DB_STATUS_CANNOT_GET,
    DB_STATUS_CANNOT_REMOVE
} DbStatus;

// ----------- Functions ------------

/**
 * Allocate a new Db structure.
 * @return A pointer to an allocated Db, or NULL if an error occured.
 */
Db *dbNew(DbInfo *dbInfo);

/**
 * Initialize an allocated Db structure.
 * @param self An allocated Db to initialize.
 * @return true on success, false otherwise.
 */
bool dbInit(Db *self, DbInfo *dbInfo);

/**
 * Allocate a new DbInfo structure.
 * @return A pointer to an allocated DbInfo, or NULL if an error occured.
 */
DbInfo *dbInfoNew(
    uint16_t routerId,
    char *dbName,
    void *heritage,
    DbProcessMsgHandler handler);

/**
 * Initialize an allocated DbInfo structure.
 * @param self An allocated DbInfo to initialize.
 * @return true on success, false otherwise.
 */
bool dbInfoInit(
    DbInfo *self,
    uint16_t routerId,
    char *dbName,
    void *heritage,
    DbProcessMsgHandler handler);

/**
 * Start the db actor
 * @param self A pointer to an allocated Db.
 */
bool dbStart(Db *self);

/**
 * Free an allocated Db structure.
 * @param self A pointer to an allocated Db.
 */
void dbFree(Db *self);

/**
 * Free an allocated Db structure and nullify the content of the pointer.
 * @param self A pointer to an allocated Db.
 */
void dbDestroy(Db **self);

/**
 * Free an allocated DbInfo structure.
 * @param self A pointer to an allocated DbInfo.
 */
void dbInfoFree(DbInfo *self);

/**
 * Free an allocated DbInfo structure and nullify the content of the pointer.
 * @param self A pointer to an allocated DbInfo.
 */
void dbInfoDestroy(DbInfo **self);
