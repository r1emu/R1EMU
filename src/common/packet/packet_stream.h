/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file packet_stream.h
 * @brief PacketStream is a utility module for constructing packets as stream
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#pragma once

#include "R1EMU.h"

// Macro helper
#define packetStreamIn(self, data) \
    packetStreamAppend(self, (void *) (data), sizeof(*(data)))

#define packetStreamOut(self, data) \
    packetStreamGet(self, (void *) (data), sizeof(*(data)))

/**
 * @brief PacketStream is a buffer with a pointer to the last data written
 *
 * PacketStream is
 */
struct PacketStream {
    uint8_t *buffer;
    unsigned int position;
};

typedef struct PacketStream PacketStream;

/**
 * @brief Allocate a new PacketStream structure.
 * @param buffer An allocated buffer for the stream. It must be big enough for the stream or it will overflow.
 * @return A pointer to an allocated PacketStream.
 */
PacketStream *packetStreamNew(void *buffer);

/**
 * @brief Initialize an allocated PacketStream structure.
 * @param self An allocated PacketStream to initialize.
 * @param buffer An allocated buffer for the stream. It must be big enough for the stream or it will overflow.
 * @return true on success, false otherwise.
 */
void packetStreamInit(PacketStream *self, void *buffer);

/**
 * @brief Move the position depending of the offset argument (position = position + offet)
 * @param self An allocated PacketStream
 * @param offset An offset
 */
void packetStreamAddOffset(PacketStream *self, unsigned int offset);

/**
 * @brief Get the stream at the current position
 * @param self An allocated PacketStream
 */
void *packetStreamGetCurrentBuffer(PacketStream *self);

/**
 * @brief Put data at the end of the stream
 * @param self An allocated PacketStream
 * @param data The data to append to the packet
 * @param dataSize the size of the data
 * @return
 */
void packetStreamAppend(PacketStream *self, void *data, size_t dataSize);

/**
 * @brief Get the data from the current stream position and remove it
 * @param self An allocated PacketStream
 * @param data The data to append to the packet
 * @param dataSize the size of the data
 * @return
 */
void packetStreamGet(PacketStream *self, void *data, size_t dataSize);

/**
 * @brief Free an allocated PacketStream structure and nullify the content of the pointer.
 *  It does *NOT* free the content of the stream, only the structure
 * @param self A pointer to an allocated PacketStream.
 */
void packetStreamDestroy(PacketStream **self);
