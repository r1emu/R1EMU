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

#include "packet.h"

void clientPacketUnwrapHeader (
    uint8_t **packet,
    size_t *packetSize,
    ClientPacketHeader *header,
    bool isCrypted
) {
    int headerSize = sizeof (ClientPacketHeader);

    if (!isCrypted) {
        headerSize -= sizeof_struct_member (ClientPacketHeader, checksum);
    }

    memcpy (header, *packet, headerSize);

    *packet = (*packet) + headerSize;
    *packetSize -= headerSize;
}

void
cryptPacketUnwrapHeader (
    uint8_t **packet,
    size_t *packetSize,
    CryptPacketHeader *header
) {
    memcpy (header, *packet, sizeof (CryptPacketHeader));

    *packet = (*packet) + sizeof (CryptPacketHeader);
    *packetSize -= sizeof (CryptPacketHeader);
}

void
cryptPacketGetHeader (
    uint8_t *packet,
    CryptPacketHeader *header
) {
    memcpy (header, packet, sizeof (CryptPacketHeader));
}

void
packetNormalHeaderInit (
    PacketNormalHeader *normalHeader,
    uint32_t subtype,
    uint32_t packetSize
) {
    variableSizePacketHeaderInit (&normalHeader->variableSizeHeader, BC_NORMAL, packetSize);
    normalHeader->subtype = subtype;
}

void
variableSizePacketHeaderInit (
    VariableSizePacketHeader *variableSizeHeader,
    uint16_t packetType,
    uint32_t packetSize
) {
    serverPacketHeaderInit (&variableSizeHeader->serverHeader, packetType);
    variableSizeHeader->packetSize = packetSize;
}

void
serverPacketHeaderInit (
    ServerPacketHeader *serverHeader,
    uint16_t packetType
) {
    serverHeader->type = packetType;
    serverHeader->reserved = -1;
}
