/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file PacketStream.h
 * @brief PacketStream is a utility module for constructing packets as stream
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
 * @brief PacketStream is a buffer with a pointer to the last data written
 *
 * PacketStream is
 */
struct PacketStream
{
    uint8_t *buffer;
    unsigned int position;
};

typedef struct PacketStream PacketStream;

// ----------- Functions ------------

/**
 * @brief Allocate a new PacketStream structure.
 * @param buffer An allocated buffer for the stream. It must be big enough for the stream or it will overflow.
 * @return A pointer to an allocated PacketStream.
 */
PacketStream *
PacketStream_new (
    uint8_t *buffer
);


/**
 * @brief Initialize an allocated PacketStream structure.
 * @param self An allocated PacketStream to initialize.
 * @param buffer An allocated buffer for the stream. It must be big enough for the stream or it will overflow.
 * @return true on success, false otherwise.
 */
bool
PacketStream_init (
    PacketStream *self,
    uint8_t *buffer
);

/**
 * @brief Move the position depending of the offset argument (position = position + offet)
 * @param self An allocated PacketStream
 * @param offset An offset
 */
void
PacketStream_addOffset (
    PacketStream *self,
    unsigned int offset
);

/**
 * @brief
 * @param self An allocated PacketStream
 * @param data The data to append to the packet
 * @param dataSize the size of the data
 * @return
 */
void
PacketStream_append (
    PacketStream *self,
    void *data,
    size_t dataSize
);


/**
 * @brief Free an allocated PacketStream structure and nullify the content of the pointer.
 *  It does *NOT* free the content of the stream, only the structure
 * @param self A pointer to an allocated PacketStream.
 */
void
PacketStream_destroy (
    PacketStream **self
);

