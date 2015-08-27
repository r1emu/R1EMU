/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file Crypto.h
 * @brief Crypto contains cryptographic functions needed for the communication between the client and the server
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#pragma once

// ---------- Includes ------------
#include "R1EMU.h"
#include "Common/Packet/Packet.h"

// ---------- Defines -------------



// ----------- Functions ------------


/**
 * @brief Initialize client crypto components
 */
bool cryptoInit(void);

/**
 * @brief Unwrap the client packet header and decrypt the packet.
 * @param[in,out] packet The packet ciphered. After this call, the packet is decrypted.
 * @param[in] packetSize A pointer to the crypted packet size
 * @return true on success, false otherwise
 */
bool cryptoDecryptPacket(uint8_t **packet, size_t *packetSize);
