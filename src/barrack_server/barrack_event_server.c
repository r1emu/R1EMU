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

#include "barrack_event_server.h"
#include "barrack_handler/barrack_builder.h"
#include "barrack_handler/barrack_event_handler.h"
#include "common/server/event_handler.h"
#include "common/redis/fields/redis_session.h"

bool barrackEventServerProcess(EventServer *self, EventType type, void *eventData)
{
    EventHandlerFunction handler;

    if (type <= EVENT_TYPE_BARRACK_START || type >= EVENT_TYPE_BARRACK_END) {
        error("Wrong event received from barrackEventServer : %d", type);
        return false;
    }

    // Test if a handler is associated with the packet type requested.
    if (!(handler = barrackEventHandlers [type].handler)) {
        error("Cannot find handler for the requested packet type : %s",
            (type <= EVENT_TYPE_BARRACK_START || type >= EVENT_TYPE_BARRACK_END) ?
               "UNKNOWN" : barrackEventHandlers[type].eventName
        );
        return false;
    }

    return handler(self, eventData);
}

bool barrackEventServerOnDisconnect (
    zsock_t *eventServer,
    Redis *redis,
    MySQL *mysql,
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
