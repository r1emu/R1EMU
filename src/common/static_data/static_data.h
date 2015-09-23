/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file static_data.h
 * @brief
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#pragma once

// Includes
#include "R1EMU.h"

// Type definition
typedef struct StaticData StaticData;
typedef uint64_t StaticDataId;

// Functions
/**
 * @brief Allocate a new StaticData structure.
 * @return A pointer to an allocated StaticData.
 */
StaticData *staticDataNew(char *name, size_t keySize);

/**
 * @brief Initialize an allocated StaticData structure.
 * @param self An allocated StaticData to initialize.
 * @return true on success, false otherwise.
 */
bool staticDataInit(StaticData *self, char *name, size_t keySize);

/**
 * @brief Add a new object in the static data hashtable.
 * @param self An allocated StaticData to initialize.
 * @param id The object ID
 * @param item The item associated with the id
 * @return true on success, false otherwise.
 */
bool staticDataAdd(StaticData *self, StaticDataId id, void *item);

/**
 * @brief Lock a StaticData so it cannot be written again (read only)
 */
void staticDataLock(StaticData *self);

/**
 * @brief Prints a StaticData structure.
 * @param self An allocated StaticData
 */
void staticDataPrint(StaticData *self);

/**
 * @brief Free an allocated StaticData structure and nullify the content of the pointer.
 * @param self A pointer to an allocated StaticData.
 */
void staticDataDestroy(StaticData **self);

/**
 * @brief Free an allocated StaticData structure
 * @param self A pointer to an allocated StaticData.
 */
void staticDataFree(StaticData *self);
