/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file utils.h
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
#include "math.h"
#include "position.h"
#include "random.h"
#include "string.h"
#include "zlib.h"

// ---------- Defines -------------
#define STRINGIFY(x) # x
#ifndef bool
#define bool char
#endif
#ifndef false
#define false 0
#endif
#ifndef true
#define true (!false)
#endif

// Size of local array keyword
#define sizeof_array(array) \
    ((int)(sizeof(array) / sizeof(*(array))))

// Size of structure member
#define sizeof_struct_member(struct, member) \
   (sizeof(((struct *)0)->member))

// Quotify a string
#define QUOTIFY(x) "\"" x "\""

// Byte swapping
#define SWAP_UINT64(x) (uint64_t) (SWAP_UINT32 ((uint32_t) (x >> 32)) | ((uint64_t) SWAP_UINT32 ((uint32_t) x) << 32))
#define SWAP_UINT32(x) (uint32_t) (((x) >> 24) | (((x) & 0x00FF0000) >> 8) | (((x) & 0x0000FF00) << 8) | ((x) << 24))
#define SWAP_UINT16(x) (uint16_t) (((x) >> 8) | ((x) << 8))

// SocketID static initialization
#define SOCKET_ID_ARRAY(x) { \
    x[0], x[1], x[2], x[3],  \
    x[4], x[5], x[6], x[7],  \
    x[8], x[9], x[10],       \
}

// ------ Structure declaration -------


// ----------- Functions ------------

/**
 * @brief Convert a log dump to memory
 * @param dump a formated dump, for instance :
 * [11:09:59][           ToSClient:                     dbgBuffer]  56 0B FF FF FF FF 2B 00 10 00 00 00 00 16 00 00 | V.....+.........
 * [11:09:59][           ToSClient:                     dbgBuffer]  00 13 9D 01 00 01 D1 A8 01 44 00 00 00 00 D0 13 | .........D......
 * [11:09:59][           ToSClient:                     dbgBuffer]  00 00 01 00 00 00 00 00 00 00 00                | ...........
 * @param memory target memory. Can be NULL
 * @param memSize the size of the target memory
 * @return memory pointer (newly allocated if memory is NULL)
 */

void *dumpToMem (
    char *dump,
    void *memory,
    size_t *memSize
);

/**
 * @brief Compare two chunks of memory and display the difference in the console
 */
void
compareMem (
    void *_mem1,
    size_t memSize1,
    void *_mem2,
    size_t memSize2
);
