/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#include "property.h"

size_t propertyFloatGetCPacketSize(float *value) {
    size_t size = 0;

    // PropertyFloatCPacket is fixed packet size
    if (value) {
        size += sizeof(PropertyFloatCPacket);
    }

    return size;
}

size_t propertyStringGetCPacketSize(char *value) {

    size_t size = 0;

    if (value) {
        size_t valueSize = strlen(value) + 1;
        #pragma pack(push, 1)
        DECLARE_PropertyStringCPacket(valueSize);
        #pragma pack(pop)

        size += sizeof(PropertyStringCPacket);
    }

    return size;
}

size_t propertyFloatGetSPacketSize(float *value) {
    size_t size = 0;

    // PropertyFloatSPacket is fixed packet size
    if (value) {
        size += sizeof(PropertyFloatSPacket);
    }

    return size;
}

size_t propertyStringGetSPacketSize(char *value) {

    size_t size = 0;

    if (value) {
        size_t valueSize = strlen(value) + 1;
        #pragma pack(push, 1)
        DECLARE_PropertyStringSPacket(valueSize);
        #pragma pack(pop)

        size += sizeof(PropertyStringSPacket);
    }

    return size;
}

void propertyFloatSerializeCPacket(PropertyId_t id, float *value, PacketStream *stream) {

    if (!value) {
        return;
    }

    PropertyFloatCPacket packet = {
        .id = id,
        .value = *value
    };

    packetStreamIn(stream, &packet);
}

void propertyStringSerializeCPacket(PropertyId_t id, char *value, PacketStream *stream) {

    if (!value) {
        return;
    }

    size_t valueSize = strlen(value) + 1;
    #pragma pack(push, 1)
    DECLARE_PropertyStringCPacket(valueSize);
    #pragma pack(pop)

    PropertyStringCPacket packet;
    packet.id = id;
    packet.size = valueSize;
    strncpy(packet.value, value, valueSize);

    packetStreamIn(stream, &packet);
}

void propertyFloatSerializeSPacket(PropertyId_t id, float *value, PacketStream *stream) {

    if (!value) {
        return;
    }

    PropertyFloatSPacket packet = {
        .id = id,
        .value = *value
    };

    packetStreamIn(stream, &packet);
}

void propertyStringSerializeSPacket(PropertyId_t id, char *value, PacketStream *stream) {

    if (!value) {
        return;
    }

    size_t valueSize = strlen(value) + 1;
    #pragma pack(push, 1)
    DECLARE_PropertyStringSPacket(valueSize);
    #pragma pack(pop)

    PropertyStringSPacket packet;
    packet.id = id;
    packet.size = valueSize;
    strncpy(packet.value, value, valueSize);

    packetStreamIn(stream, &packet);
}

bool propertyFloatUnserializeSPacket(PropertyId_t id, float **value, PacketStream *stream) {

    PropertyFloatSPacket *packet = packetStreamGetCurrentBuffer(stream);
    PropertyFloatSPacket output;

    // Check ID
    if (packet->id != id) {
        // The current property has not been found
        // This is a normal behavior : The property hasn't been serialized
        return true;
    }

    // Check memory
    if (!*value) {
        // Not allocated yet
        if (!(*value = malloc(sizeof(float *)))) {
            error("Cannot allocate a new float property.");
            return false;
        }
    }

    // Extract information from the stream
    packetStreamOut(stream, &output);
    **value = output.value;

    return true;
}

bool propertyStringUnserializeSPacket(PropertyId_t id, char **value, PacketStream *stream) {

    // Check ID and memory space
    size_t valueSize;
    {
        // Declare the structure with a fake size
        #pragma pack(push, 1)
        DECLARE_PropertyStringSPacket(0);
        #pragma pack(pop)

        PropertyStringSPacket *packet = packetStreamGetCurrentBuffer(stream);

        if (packet->id != id) {
            // The current property has not been found
            // This is a normal behavior : The property hasn't been serialized
            return true;
        }

        valueSize = packet->size;
    }

    // Redeclare the structure with the correct size
    #pragma pack(push, 1)
    DECLARE_PropertyStringSPacket(valueSize);
    #pragma pack(pop)
    PropertyStringSPacket *packet = packetStreamGetCurrentBuffer(stream);
    PropertyStringSPacket output;

    // Check memory space
    if (!*value) {
        // Not allocated yet
        if (!(*value = malloc(valueSize))) {
            error("Cannot allocate a new float property.");
            return false;
        }
    }
    else if (strlen(*value) + 1 != packet->size) {
        // The memory space has changed
        if (!(*value = realloc(*value, valueSize + 1))) {
            error("Cannot reallocate a new float property.");
            return false;
        }
    }

    // Extract information from the stream
    packetStreamOut(stream, &output);
    strncpy(*value, output.value, valueSize);

    return true;
}
