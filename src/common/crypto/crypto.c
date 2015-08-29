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
#include "crypto.h"
#include "schedule.h"
#include "bf/blowfish.h"

// ------ Static declaration -------
const uint32_t keyIndex = 11;
const uint32_t numPoint[] = {16, 2, 256, 768};
const uint32_t seekPoint[] = {4, 1056, 24, 284};
uint8_t schedule[sizeof(BF_KEY)];
const char* keyTable[] = {
    "jvwvqgfghyrqewea",
    "gsqsdafkssqshafd",
    "mmxi0dozaqblnmwo",
    "nyrqvwescuanhanu",
    "gbeugfaacudgaxcs",
    "rbneolithqb5yswm",
    "1udgandceoferaxe",
    "qarqvwessdicaqbe",
    "2udqaxxxiscogito",
    "affcbjwxymsi0doz",
    "ydigadeath8nnmwd",
    "hsunffalqyrqewes",
    "gqbnnmnmymsi0doz",
    "hvwvqgfl1nrqewes",
    "sxqsdafkssqsdafk",
    "ssqsaafsssqsdof6",
};

// ------ Extern function implementation ------

bool cryptoInit(void) {

    const unsigned char* key = keyTable[keyIndex];
    int offset = 0;

    for (int i = 0; i < 4; ++i)
    {
        if (numPoint[i] + offset > 1042) {
            error ("NumPoint invalid");
            return false;
        }

        memcpy((char*) schedule + offset * 4, (char *) initSchedule + seekPoint[i] * 4, numPoint[i] * 4);
        offset += numPoint[i];
    }

    BF_set_key_custom_sch((BF_KEY *) schedule, 16, key);

    return true;
}

bool cryptoDecryptPacket(uint8_t **packet, size_t *packetSize) {

    // Unwrap the crypt packet header, and check the cryptHeader size
    // In a single request process, it must match exactly the same size
    CryptPacketHeader cryptHeader;
    cryptPacketUnwrapHeader(packet, packetSize, &cryptHeader);

    if (*packetSize != cryptHeader.plainSize) {
        error ("The real packet size (0x%x) doesn't match with the packet size in the header (0x%x). Ignore request.",
            *packetSize, cryptHeader.plainSize);
        return false;
    }

    int blocks = cryptHeader.plainSize / BF_BLOCK;

    for (int i = 0; i < blocks; i++) {
        size_t offset = i * BF_BLOCK;
        BF_ecb_encrypt(*packet + offset, *packet + offset, (BF_KEY *) schedule, BF_DECRYPT);
    }

    return true;
}
