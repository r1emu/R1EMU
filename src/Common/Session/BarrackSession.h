/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file BarrackSession.h
 * @brief
 *
 *
 *
 * @license <license placeholder>
 */

#pragma once

// ---------- Includes ------------
#include "R1EMU.h"

// ---------- Defines -------------

// ------ Structure declaration -------
/**
 * @brief
 */
struct BarrackSession
{
    /** Number of characters registered in the barrack */
    uint8_t charactersCreatedCount;
};


typedef struct BarrackSession BarrackSession;

// ----------- Functions ------------

/**
 * @brief Allocate a new BarrackSession structure.
 * @return A pointer to an allocated BarrackSession.
 */
BarrackSession *
BarrackSession_new (
);


/**
 * @brief Initialize an allocated BarrackSession structure.
 * @param self An allocated BarrackSession to initialize.
 * @return true on success, false otherwise.
 */
bool
BarrackSession_init (
    BarrackSession *self
);

/**
 * @brief Prints a BarrackSession structure.
 * @param self An allocated BarrackSession
 */
void
BarrackSession_print (
    BarrackSession *self
);

/**
 * @brief Free an allocated BarrackSession structure and nullify the content of the pointer.
 * @param self A pointer to an allocated BarrackSession.
 */
void
BarrackSession_destroy (
    BarrackSession **self
);


/**
 * @brief Free an allocated BarrackSession structure
 * @param self A pointer to an allocated BarrackSession.
 */
void
BarrackSession_free (
    BarrackSession *self
);
