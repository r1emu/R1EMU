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
#include "packet_type.h"
#include <string.h>

// ------ Structure declaration -------
/**
* @brief Enumerates all the packets used in the game between the client and the server.
*        It gives more info than PacketType (packet size and string)
*        Its purpose is to give information during runtime execution, contrary to PacketType that is used during the compilation.
*/
PacketTypeInfo packetTypeInfo;

bool packetTypeInit(void) {

	PacketTypeEntry _packets[] = {
		FOREACH_PACKET_TYPE(GENERATE_PACKET_TYPE_ENTRY)
	};

	for (int id = 0; id < PACKET_TYPES_MAX_INDEX; id++) {
		packetTypeInfo.packets[id].name = "UNKNOWN_PACKET";

		for (int j = 0; j < sizeof_array(_packets); j++) {
			PacketTypeEntry *entry = &_packets[j];

			if (entry->id == id) {
				memcpy(&packetTypeInfo.packets[id], entry, sizeof(packetTypeInfo.packets[id]));
			}
		}
	}

	return true;
}

char *packetTypeToString (PacketType type) {
	if (type <= 0 || type >= PACKET_TYPE_COUNT) {
		return "UNKNOWN_PACKET";
	}
    return packetTypeInfo.packets [type].name;
}
