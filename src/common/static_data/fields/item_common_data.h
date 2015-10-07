/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file item_common_data.h
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
/**
 * @brief ItemCommonData contains static data about common items
 */
typedef struct
{
    int ClassID;
    char *ItemType;

}   ItemCommonData;

// ----------- Functions ------------

/**
 * Allocate a new ItemCommonData structure.
 * @return A pointer to an allocated ItemCommonData, or NULL if an error occured.
 */
ItemCommonData *itemCommonDataNew(MYSQL_ROW row);

/**
 * Initialize an allocated ItemCommonData structure.
 * @param self An allocated ItemCommonData to initialize.
 * @return true on success, false otherwise.
 */
bool itemCommonDataInit(ItemCommonData *self, MYSQL_ROW row);

/**
 * Free an allocated ItemCommonData structure.
 * @param self A pointer to an allocated ItemCommonData.
 */
void itemCommonDataFree(ItemCommonData *self);

/**
 * Free an allocated ItemCommonData structure and nullify the content of the pointer.
 * @param self A pointer to an allocated ItemCommonData.
 */
void itemCommonDataDestroy(ItemCommonData **self);

/**
 * Print a common item staic data
 */
void itemCommonDataPrint(ItemCommonData *self);
