/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file BarrackHandler.h
 * @brief BarrackHandler is a module containing all the handlers for the Barrack system
 *
 * @license <license placeholder>
 */

#pragma once

// ---------- Includes ------------
#include "R1EMU.h"
#include "Common/Packet/PacketType.h"
#include "Common/Server/Worker.h"

// ---------- Defines -------------

// ----------- Functions ------------
// Handlers
/** Global table of handlers for the Barrack. It contains only the packets allowed. */
extern const PacketHandler barrackHandlers [PACKET_TYPE_COUNT];
