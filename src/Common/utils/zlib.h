/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file zlib.h
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
#define ZLIB_MAGIC_HEADER 0xFA8D

#define ZLIB_GET_COMPRESSED_PACKET_SIZE(_zlib, _pktSize) \
    (_zlib)->header.size + sizeof (ZlibHeader) + (_pktSize - sizeof (Zlib));


// ------ Structure declaration -------
typedef struct {
    uint16_t magic;
    uint16_t size;
} ZlibHeader;

typedef struct {
    ZlibHeader header;
    uint8_t buffer [0x40000];
} Zlib;

// ----------- Functions ------------
/**
 * @brief : Compress a given data to a Zlib
 */
bool
Zlib_compress (
    Zlib *self,
    void *data,
    size_t dataSize
);

/**
 * @brief : Decompress a given data to a Zlib
 */
bool
Zlib_decompress (
    Zlib *self,
    void *data,
    size_t dataSize
);
