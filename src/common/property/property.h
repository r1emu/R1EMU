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
// Property generators
#define GENERATE_PROPERTY_ID(TYPE, NAME, VALUE) TYPE##_PROPERTY_ID_##NAME = VALUE,
#define GENERATE_PROPERTY_ENUM(TYPE, NAME, VALUE) TYPE##_PROPERTY_##NAME,
#define GENERATE_PROPERTY_ASSOC(TYPE, NAME, VALUE) [TYPE##_PROPERTY_ID_##NAME] = TYPE##_PROPERTY_##NAME,

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
} PropertyCPacket

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
} PropertyStringCPacket

// ===== Server packet structures =====

/** Property server packet structure */
#define DECLARE_PropertySPacket(x)             \
typedef struct {                               \
    uint16_t size;                             \
    uint8_t data[x];                           \
} PropertySPacket

/** Float property server packet structure */
#pragma pack(push, 1)
typedef struct {
    PropertyId_t id;
    float value;
} PropertyFloatSPacket;
#pragma pack(pop)

/** String property dynamic server packet structure */
#define DECLARE_PropertyStringSPacket(x)       \
typedef struct {                               \
    PropertyId_t id;                           \
    uint16_t size;                             \
    uint8_t  value[x];                         \
} PropertyStringSPacket

/** String property server packet structure */
typedef struct {
    PropertyId_t id;
    uint16_t size;
    uint8_t  value[0];
} PropertyStringSPacket;

// ----------- Functions ------------

size_t propertyFloatGetCPacketSize(float *value);
size_t propertyFloatGetSPacketSize(float *value);
void propertyFloatSerializeCPacket(PropertyId_t id, float *value, PacketStream *stream);
void propertyFloatSerializeSPacket(PropertyId_t id, float *value, PacketStream *stream);
bool propertyFloatUnserializeSPacket(PropertyId_t id, float **value, PacketStream *stream);

size_t propertyStringGetCPacketSize(char *value);
size_t propertyStringGetSPacketSize(char *value);
void propertyStringSerializeCPacket(PropertyId_t id, char *value, PacketStream *stream);
void propertyStringSerializeSPacket(PropertyId_t id, char *value, PacketStream *stream);
bool propertyStringUnserializeSPacket(PropertyId_t id, char **value, PacketStream *stream);
