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
#include "SocialHandler.h"
#include "Common/Packet/Packet.h"
#include "Common/Commander/Commander.h"
#include "Common/Packet/PacketStream.h"
#include "Common/Redis/Fields/RedisGameSession.h"
#include "Common/Server/Worker.h"
#include "BarrackServer/BarrackServer.h"


// ------ Static declaration -------
/** On log in */
static PacketHandlerState SocialHandler_login        (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *reply);


// ------ Structure declaration -------
/**
 * @brief socialHandlers is a global table containing all the social handlers.
 */
const PacketHandler socialHandlers [PACKET_TYPE_COUNT] = {
    #define REGISTER_PACKET_HANDLER(packetName, handler) \
        [packetName] = {handler, STRINGIFY (packetName)}

    REGISTER_PACKET_HANDLER (CS_LOGIN, SocialHandler_login),

    #undef REGISTER_PACKET_HANDLER
};

static PacketHandlerState
SocialHandler_login (
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *reply
) {
    #pragma pack(push, 1)
    typedef struct {
        char accountLogin[ACCOUNT_SESSION_LOGIN_MAXSIZE];
        char md5[17];
        uint64_t accountId;
    } CsLoginPacket;
    #pragma pack(pop)

    if (sizeof (CsLoginPacket) != packetSize) {
        error ("The packet size received isn't correct. (packet size = %d, correct size = %d)",
            packetSize, sizeof (CsLoginPacket));

        return PACKET_HANDLER_ERROR;
    }

    CsLoginPacket *clientPacket = (CsLoginPacket *) packet;
    dbg ("accountLogin = %s", clientPacket->accountLogin);
    buffer_print (clientPacket->md5, 17, "md5 = ");
    dbg ("accountId = %llx", clientPacket->accountId);

    // Authenticate here
    // TODO

    // Authentication OK!
    session->socket.accountId = clientPacket->accountId;
    accountSessionInit (&session->game.accountSession, clientPacket->accountLogin, session->socket.sessionKey, ACCOUNT_SESSION_PRIVILEGES_ADMIN);
    session->socket.authenticated = true;

    // TODO : Answer something

    return PACKET_HANDLER_UPDATE_SESSION;
}
