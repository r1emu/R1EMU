/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file static_item.h
 * @brief
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#pragma once

// Includes
#include "R1EMU.h"
#include "static_data.h"
#include "fields/item_data.h"

// Type definition
typedef struct StaticItem StaticItem;

// Functions
/**
 * @brief Allocate a new StaticItem structure.
 * @return A pointer to an allocated StaticItem.
 */
StaticItem *staticItemNew(void);

/**
 * @brief Initialize an allocated StaticItem structure.
 * @param self An allocated StaticItem to initialize.
 * @return true on success, false otherwise.
 */
bool staticItemInit(StaticItem *self);

/**
 * @brief Prints a StaticItem structure.
 * @param self An allocated StaticItem
 */
void staticItemPrint(StaticItem *self);

/**
 * @brief Add an ItemData in the StaticItem database
 */
bool staticItemAdd(StaticItem *self, StaticDataId id, ItemData *item);

/**
 * @brief Free an allocated StaticItem structure and nullify the content of the pointer.
 * @param self A pointer to an allocated StaticItem.
 */
void staticItemDestroy(StaticItem **self);

/**
 * @brief Free an allocated StaticItem structure
 * @param self A pointer to an allocated StaticItem.
 */
void staticItemFree(StaticItem *self);
