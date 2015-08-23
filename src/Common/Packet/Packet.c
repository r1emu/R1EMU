/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @license <license placeholder>
 */

// ---------- Includes ------------
#include "Packet.h"


// ------ Structure declaration -------


// ------ Static declaration -------


// ------ Extern function implementation ------

void
ClientPacket_unwrapHeader (
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
CryptPacket_unwrapHeader (
    uint8_t **packet,
    size_t *packetSize,
    CryptPacketHeader *header
) {
    memcpy (header, *packet, sizeof (CryptPacketHeader));

    *packet = (*packet) + sizeof (CryptPacketHeader);
    *packetSize -= sizeof (CryptPacketHeader);
}

void
CryptPacket_getHeader (
    uint8_t *packet,
    CryptPacketHeader *header
) {
    memcpy (header, packet, sizeof (CryptPacketHeader));
}

void
PacketNormalHeader_init (
    PacketNormalHeader *normalHeader,
    uint32_t subtype,
    uint32_t packetSize
) {
    VariableSizePacketHeader_init (&normalHeader->variableSizeHeader, BC_NORMAL, packetSize);
    normalHeader->subtype = subtype;
}

void
VariableSizePacketHeader_init (
    VariableSizePacketHeader *variableSizeHeader,
    uint16_t packetType,
    uint32_t packetSize
) {
    ServerPacketHeader_init (&variableSizeHeader->serverHeader, packetType);
    variableSizeHeader->packetSize = packetSize;
}

void
ServerPacketHeader_init (
    ServerPacketHeader *serverHeader,
    uint16_t packetType
) {
    serverHeader->type = packetType;
    serverHeader->reserved = -1;
}
