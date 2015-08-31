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
#include "social_event_handler.h"
#include "social_builder.h"
#include "social_event.h"
#include "social_server/social_event_server.h"
#include "common/server/game_event.h"

/**
 * @brief socialEventHandlers is a global table containing all the social event handlers.
 */
const EventHandler socialEventHandlers[EVENT_TYPE_SOCIAL_END] = {
    #define REGISTER_PACKET_HANDLER(packetName, _handler) \
       [packetName] = {.handler = (void *) _handler, .eventName = STRINGIFY(packetName

    #undef REGISTER_PACKET_HANDLER
};
