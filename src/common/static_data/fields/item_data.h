/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file item_data.h
 * @brief
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#pragma once

// Includes
#include "R1EMU.h"

// Type definition
typedef struct ItemData ItemData;

// Functions
/**
 * @brief Allocate a new ItemData structure.
 * @return A pointer to an allocated ItemData.
 */
ItemData *itemDataNew(void);

/**
 * @brief Initialize an allocated ItemData structure.
 * @param self An allocated ItemData to initialize.
 * @return true on success, false otherwise.
 */
bool itemDataInit(ItemData *self);

/**
 * @brief Prints a ItemData structure.
 * @param self An allocated ItemData
 */
void itemDataPrint(ItemData *self);

/**
 * @brief Free an allocated ItemData structure and nullify the content of the pointer.
 * @param self A pointer to an allocated ItemData.
 */
void itemDataDestroy(ItemData **self);

/**
 * @brief Free an allocated ItemData structure
 * @param self A pointer to an allocated ItemData.
 */
void itemDataFree(ItemData *self);
