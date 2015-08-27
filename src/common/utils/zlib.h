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
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#pragma once

#include "R1EMU.h"

#define ZLIB_MAGIC_HEADER 0xFA8D

#define ZLIB_GET_COMPRESSED_PACKET_SIZE(_zlib, _pktSize) \
    (_zlib)->header.size + sizeof(ZlibHeader) + (_pktSize - sizeof(Zlib));

typedef struct {
uint16_t magic;
    uint16_t size;
} ZlibHeader;

typedef struct {
    ZlibHeader header;
    uint8_t buffer [0x40000];
} Zlib;

/**
 * @brief : Compress a given data to a Zlib
 */
bool zlibCompress(Zlib *self, void *data, size_t dataSize);

/**
 * @brief : Decompress a given data to a Zlib
 */
bool zlibDecompress(Zlib *self, void *data, size_t dataSize);
