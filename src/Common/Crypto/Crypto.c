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

// ---------- Includes ------------
#include "Crypto.h"


// ------ Static declaration -------


// ------ Extern function implementation ------

bool
Crypto_decryptPacket (
    uint8_t **packet,
    size_t *packetSize
) {
    // Unwrap the crypt packet header, and check the cryptHeader size
    // In a single request process, it must match exactly the same size
    CryptPacketHeader cryptHeader;
    CryptPacket_unwrapHeader (packet, packetSize, &cryptHeader);
    if (*packetSize != cryptHeader.plainSize) {
        error ("The real packet size (0x%x) doesn't match with the packet size in the header (0x%x). Ignore request.",
            *packetSize, cryptHeader.plainSize);
        return false;
    }

    // TODO

    return true;
}
