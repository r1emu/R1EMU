/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file item_attribute.h
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
#include "common/packet/packet_stream.h"

// ---------- Defines -------------


// ------ Structure declaration -------
/** Property client/server ID size */
typedef uint16_t PropertyId;

/** Property formats */
typedef enum {
    FLOAT_PROPERTY,
    STRING_PROPERTY,
} PropertyFormat;

/** Property server/client compatible packet structure */
#define DECLARE_PropertyCPacket(x)             \
typedef struct {                               \
    uint16_t size;                             \
    uint8_t data[x];                           \
} PropertyCPacket;

/** Float property server/client compatible packet structure */
#pragma pack(push, 1)
typedef struct {
    PropertyId id;
    float value;
} PropertyFloatCPacket;
#pragma pack(pop)

/** String property server/client compatible packet structure */
#define DECLARE_PropertyStringCPacket(x)       \
typedef struct {                               \
    PropertyId id;                             \
    uint16_t size;                             \
    uint8_t  value[x];                         \
} PropertyStringCPacket;

// ----------- Functions ------------

size_t propertyFloatGetCPacketSize(void);
void propertyFloatGetCPacket(PropertyId id, float value, PacketStream *stream);

size_t propertyStringGetCPacketSize(char *value);
void propertyStringGetCPacket(PropertyId id, char *value, PacketStream *stream);
