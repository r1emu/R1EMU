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

size_t propertyFloatGetCPacketSize(void) {
    size_t size = 0;

    // PropertyFloatCPacket is fixed packet size
    size += sizeof(PropertyFloatCPacket);

    return size;
}

size_t propertyStringGetCPacketSize(char *value) {

    size_t size = 0;

    size_t valueSize = strlen(value) + 1;
    #pragma pack(push, 1)
    DECLARE_PropertyStringCPacket(valueSize);
    #pragma pack(pop)

    size += sizeof(PropertyStringCPacket);

    return size;
}

void propertyFloatGetCPacket(PropertyId id, float value, PacketStream *stream) {

    PropertyFloatCPacket packet = {
        .id = id,
        .value = value
    };

    packetStreamAppend(stream, &packet, sizeof(packet));
}


void propertyStringGetCPacket(PropertyId id, char *value, PacketStream *stream) {

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

