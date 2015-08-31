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

#include "social_event_server.h"
#include "social_handler/social_builder.h"
#include "social_handler/social_event_handler.h"
#include "common/server/event_handler.h"
#include "common/redis/fields/redis_session.h"
#include "common/redis/fields/redis_game_session.h"

bool socialEventServerProcess(EventServer *self, EventType type, void *eventData) {

    EventHandlerFunction handler;

    if (type <= EVENT_TYPE_SOCIAL_START || type >= EVENT_TYPE_SOCIAL_END) {
        error("Wrong event received from ZoneEventServer : %d", type);
        return false;
    }

    // Test if a handler is associated with the packet type requested.
    if (!(handler = socialEventHandlers [type].handler)) {
        error("Cannot find handler for the requested packet type : %s",
            (type <= EVENT_TYPE_SOCIAL_START || type >= EVENT_TYPE_SOCIAL_END) ?
               "UNKNOWN" : socialEventHandlers[type].eventName
        );
        return false;
    }

    return handler(self, eventData);
}

bool socialEventServerOnDisconnect (
    zsock_t *eventServer,
    Redis *redis,
    uint16_t routerId,
    uint8_t *sessionKeyStr
) {
    // Flush the Redis session of the client
    RedisSessionKey sessionKey = {
        .socketKey = {
            .routerId = routerId,
            .sessionKey = sessionKeyStr
        }
    };
    if (!(redisFlushSession (redis, &sessionKey))) {
        error ("Cannot flush the redis session '%s'", sessionKey);
        return false;
    }

    return true;
}
