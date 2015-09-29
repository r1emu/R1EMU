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
#include "social_handler.h"
#include "common/packet/packet.h"
#include "common/commander/commander.h"
#include "common/packet/packet_stream.h"
#include "common/redis/fields/redis_game_session.h"
#include "common/server/worker.h"
#include "barrack_server/barrack_server.h"


// ------ Static declaration -------
/** On log in */
static PacketHandlerState socialHandlerLogin        (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *reply);


// ------ Structure declaration -------
/**
 * @brief socialHandlers is a global table containing all the social handlers.
 */
const PacketHandler socialHandlers [PACKET_TYPE_COUNT] = {
    #define REGISTER_PACKET_HANDLER(packetName, handler) \
        [packetName] = {handler, STRINGIFY(packetName)}

    REGISTER_PACKET_HANDLER(CS_LOGIN, socialHandlerLogin),

    #undef REGISTER_PACKET_HANDLER
};

static PacketHandlerState socialHandlerLogin(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *reply)
{
    #pragma pack(push, 1)
    struct {
        char accountName[ACCOUNT_SESSION_ACCOUNT_NAME_MAXSIZE];
        char md5[17];
        uint64_t accountId;
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

    CHECK_CLIENT_PACKET_SIZE(*clientPacket, packetSize, CS_LOGIN);

    // Authenticate here
    // TODO

    // Authentication OK!
    session->socket.accountId = clientPacket->accountId;
    accountSessionInit(&session->game.accountSession, clientPacket->accountName, session->socket.sessionKey,
        ACCOUNT_SESSION_PRIVILEGES_ADMIN);
    session->socket.authenticated = true;

    // TODO : Answer something

    return PACKET_HANDLER_UPDATE_SESSION;
}
