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

#include "crypto.h"

bool cryptoDecryptPacket(uint8_t **packet, size_t *packetSize) {
    // unwrap the crypt packet header, and check the cryptHeader size
    // in a single request process, it must match exactly the same size
    CryptPacketHeader cryptHeader;
    cryptPacketUnwrapHeader(packet, packetSize, &cryptHeader);

    if (*packetSize != cryptHeader.plainSize) {
        error("The real packet size (0x%x) doesn't match with the packet size in the header (0x%x). Ignore request.",
            *packetSize, cryptHeader.plainSize);
        return false;
    }

    // TODO
    return true;
}
