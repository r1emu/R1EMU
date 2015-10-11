/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file property.h
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
typedef uint16_t PropertyId_t;

/** Property formats */
typedef enum {
    FLOAT_PROPERTY,
    STRING_PROPERTY,
} PropertyFormat;

// ===== Client packet structures =====

/** Property client packet structure */
#define DECLARE_PropertyCPacket(x)             \
typedef struct {                               \
    uint16_t size;                             \
    uint8_t data[x];                           \
} PropertyCPacket;

/** Float property client packet structure */
#pragma pack(push, 1)
typedef struct {
    PropertyId_t id;
    float value;
} PropertyFloatCPacket;
#pragma pack(pop)

/** String property client packet structure */
#define DECLARE_PropertyStringCPacket(x)       \
typedef struct {                               \
    PropertyId_t id;                           \
    uint16_t size;                             \
    uint8_t  value[x];                         \
} PropertyStringCPacket;

// ===== Server packet structures =====

/** Property server packet structure */
#define DECLARE_PropertySPacket(x)             \
typedef struct {                               \
    uint16_t size;                             \
    uint8_t data[x];                           \
} PropertySPacket;

/** Float property server packet structure */
#pragma pack(push, 1)
typedef struct {
    PropertyId_t id;
    float value;
} PropertyFloatSPacket;
#pragma pack(pop)

/** String property server packet structure */
#define DECLARE_PropertyStringSPacket(x)       \
typedef struct {                               \
    PropertyId_t id;                           \
    uint16_t size;                             \
    uint8_t  value[x];                         \
} PropertyStringSPacket;

// ----------- Functions ------------

size_t propertyFloatGetCPacketSize(float *value);
void propertyFloatGetCPacket(PropertyId_t id, float *value, PacketStream *stream);
size_t propertyFloatGetSPacketSize(float *value);
void propertyFloatGetSPacket(PropertyId_t id, float *value, PacketStream *stream);

size_t propertyStringGetCPacketSize(char *value);
void propertyStringGetCPacket(PropertyId_t id, char *value, PacketStream *stream);
size_t propertyStringGetSPacketSize(char *value);
void propertyStringGetSPacket(PropertyId_t id, char *value, PacketStream *stream);
