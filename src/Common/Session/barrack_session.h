/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file barrack_session.h
 * @brief
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#pragma once

#include "R1EMU.h"

/**
 * @brief
 */
struct BarrackSession {;
    // number of characters registered in the barrack
    uint8_t charactersCreatedCount;
};

typedef struct BarrackSession BarrackSession;

/**
 * @brief Allocate a new BarrackSession structure.
 * @return A pointer to an allocated BarrackSession.
 */
BarrackSession *barrackSessionNew();

/**
 * @brief Initialize an allocated BarrackSession structure.
 * @param self An allocated BarrackSession to initialize.
 * @return true on success, false otherwise.
 */
bool barrackSessionInit(BarrackSession *self);

/**
 * @brief Prints a BarrackSession structure.
 * @param self An allocated BarrackSession
 */
void barrackSessionPrint(BarrackSession *self);

/**
 * @brief Free an allocated BarrackSession structure and nullify the content of the pointer.
 * @param self A pointer to an allocated BarrackSession.
 */
void barrackSessionDestroy(BarrackSession **self);

/**
 * @brief Free an allocated BarrackSession structure
 * @param self A pointer to an allocated BarrackSession.
 */
void barrackSessionFree(BarrackSession *self);
