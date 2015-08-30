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
#include "barrack_event_handler.h"
#include "barrack_builder.h"
#include "barrack_event.h"
#include "barrack_server/barrack_event_server.h"
#include "common/server/game_event.h"

/**
 * @brief barrackEventHandlers is a global table containing all the barrack event handlers.
 */
const EventHandler barrackEventHandlers[EVENT_TYPE_BARRACK_END] = {
    #define REGISTER_PACKET_HANDLER(packetName, _handler) \
       [packetName] = {.handler = (void *) _handler, .eventName = STRINGIFY(packetName)}

    #undef REGISTER_PACKET_HANDLER
};
