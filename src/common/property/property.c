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

void propertyFloatGetCPacket(PropertyId_t id, float *value, PacketStream *stream) {

    if (!value) {
        return;
    }

    PropertyFloatCPacket packet = {
        .id = id,
        .value = *value
    };

    packetStreamAppend(stream, &packet, sizeof(packet));
}


void propertyStringGetCPacket(PropertyId_t id, char *value, PacketStream *stream) {

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

    packetStreamAppend(stream, &packet, sizeof(packet));
}

