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

#include "global_server.h"
#include "zone_server/zone_server.h"
#include "social_server/social_server.h"
#include "barrack_server/barrack_server.h"
#include "common/server/server_factory.h"
#include <jansson.h>

/**
 * @brief GlobalServer detains the authority on all the zone servers
 * It communicates with the nodes that need to communicates with all the zone servers.
 */
struct GlobalServer
{
    // global information
    GlobalServerStartupInfo info;

    // socket listening to the CLI
    zsock_t *cliConnection;

    // socket talking to the zones
    zsock_t *zonesConnection;

    // connection to Redis */
    Redis *redis;
};

GlobalServer *globalServerNew(GlobalServerStartupInfo *info) {
    GlobalServer *self;

    if ((self = calloc(1, sizeof(GlobalServer))) == NULL) {
        return NULL;
    }

    if (!globalServerInit(self, info)) {
        globalServerDestroy(&self);
        error("GlobalServer failed to initialize.");
        return NULL;
    }

    return self;
}

bool globalServerInit(GlobalServer *self, GlobalServerStartupInfo *info) {
    memcpy(&self->info, info, sizeof(self->info));

    // allocate ZMQ objects
    if (!(self->cliConnection = zsock_new(ZMQ_RAW_ROUTER))) {
        error("Cannot allocate a new CLI zsock.");
        return false;
    }
    if (!(self->zonesConnection = zsock_new(ZMQ_REQ))) {
        error("Cannot allocate a new zones zsock.");
        return false;
    }

    // connect to Redis Server
    if (!(self->redis = redisNew(&self->info.redisInfo))) {
        error("Cannot initialize Redis connection.");
        return false;
    }

    if (!(redisConnection(self->redis))) {
        error("Cannot connect to Redis.");
        return false;
    }

    return true;
}

bool globalServerFlushRedis(GlobalServer *self) {
    return redisFlushDatabase(self->redis);
}

bool globalServerReadBasicServerConf(BasicServerConf *basicConf, json_t *servers, int serverId)
{
    bool result = true;
    json_t *server = NULL;
    json_t *field = NULL;

    if (!(server = json_array_get(servers, serverId))) {
        error("Cannot get element ID = %d.", serverId);
        result = false;
        goto cleanup;
    }

    // read IP
    if (!(field = json_object_get(server, "ip"))
    ||  !(json_is_string(field)))
    {
        error("Cannot read 'ip' field.");
        result = false;
        goto cleanup;
    }
    basicConf->ip = strdup(json_string_value(field));

    // read port
    if (!(field = json_object_get(server, "port"))
    ||  !(json_is_string(field)))
    {
        error("Cannot read 'port' field.");
        result = false;
        goto cleanup;
    }
    basicConf->port = atoi(json_string_value(field));

    // read workers count
    if (!(field = json_object_get(server, "workersCount"))
    ||  !(json_is_string(field)))
    {
        error("Cannot read 'workersCount' field.");
        result = false;
        goto cleanup;
    }
    basicConf->workersCount = atoi(json_string_value(field));


cleanup:
    return result;
}

bool globalServerStartupInfoInit(GlobalServerStartupInfo *self, char *confFilePath) {
    memset(self, 0, sizeof(GlobalServerStartupInfo));

    bool result = true;
    char *confStr = NULL;
    json_t *root = NULL;
    json_error_t error;
    json_t *field;

    // open the configuration file
    if (!(confStr = fileGetContents(confFilePath, NULL))) {
        error("Cannot read the configuration file (%s).", confFilePath);
        result = false;
        goto cleanup;
    }

    if (!(root = json_loads(confStr, 0, &error))) {
        error("Cannot load JSON correctly : Line %d : %s", error.line, error.text);
        result = false;
        goto cleanup;
    }

    if (!(json_is_object(root))) {
        error("Cannot read JSON correctly.");
        result = false;
        goto cleanup;
    }

    // ===================================
    //       Read Global configuration
    // ===================================
    json_t *globalServer;
    if (!(globalServer = json_object_get(root, "globalServer"))
    ||  !(json_is_object(globalServer)))
    {
        error("Cannot read 'globalServer' section correctly.");
        result = false;
        goto cleanup;
    }

    // read IP
    if (!(field = json_object_get(globalServer, "ip"))
    ||  !(json_is_string(field)))
    {
        error("Cannot read 'ip' field in 'globalServer' section correctly.");
        result = false;
        goto cleanup;
    }
    self->globalConf.ip = strdup(json_string_value(field));

    // read port
    if (!(field = json_object_get(globalServer, "port"))
    ||  !(json_is_string(field)))
    {
        error("Cannot read 'port' field in 'globalServer' section correctly.");
        result = false;
        goto cleanup;
    }
    self->globalConf.port = atoi(json_string_value(field));

    // ===================================
    //     Read Barracks configuration
    // ===================================
    json_t *barrackServers;
    if (!(barrackServers = json_object_get(root, "barrackServers"))
    ||  !(json_is_array(barrackServers)))
    {
        error("Cannot read 'barrackServers' section correctly.");
        result = false;
        goto cleanup;
    }

    // allocate configuration
    self->barracksConf.count = json_array_size(barrackServers);
    if (!(self->barracksConf.confs = malloc (sizeof (BarrackServerConf) * self->barracksConf.count))) {
        error("Cannot allocate barracks configuration.");
        result = false;
        goto cleanup;
    }

    // read configuration
    for (int id = 0; id < json_array_size(barrackServers); id++) {
        BarrackServerConf *curConf = &self->barracksConf.confs[id];
        if (!(globalServerReadBasicServerConf(&curConf->basicConf, barrackServers, id))) {
            error("Cannot read barrack server entry %d in 'barrackServers'", id);
            result = false;
            goto cleanup;
        }
    }

    // ===================================
    //     Read Socials configuration
    // ===================================
    json_t *socialServers;
    if (!(socialServers = json_object_get(root, "socialServers"))
    ||  !(json_is_array(socialServers)))
    {
        error("Cannot read 'socialServers' section correctly.");
        result = false;
        goto cleanup;
    }

    // allocate configuration
    self->socialsConf.count = json_array_size(socialServers);
    if (!(self->socialsConf.confs = malloc (sizeof (SocialServerConf) * self->socialsConf.count))) {
        error("Cannot allocate socials configuration.");
        result = false;
        goto cleanup;
    }

    // read configuration
    for (int id = 0; id < json_array_size(socialServers); id++) {
        SocialServerConf *curConf = &self->socialsConf.confs[id];
        if (!(globalServerReadBasicServerConf(&curConf->basicConf, socialServers, id))) {
            error("Cannot read social server entry %d in 'socialServers'", id);
            result = false;
            goto cleanup;
        }
    }

    // ===================================
    //     Read Zones configuration
    // ===================================
    json_t *zoneServers;
    if (!(zoneServers = json_object_get(root, "zoneServers"))
    ||  !(json_is_array(zoneServers)))
    {
        error("Cannot read 'zoneServers' section correctly.");
        result = false;
        goto cleanup;
    }

    // allocate configuration
    self->zonesConf.count = json_array_size(zoneServers);
    if (!(self->zonesConf.confs = malloc (sizeof (ZoneServerConf) * self->zonesConf.count))) {
        error("Cannot allocate zones configuration.");
        result = false;
        goto cleanup;
    }

    // read configuration
    for (int id = 0; id < json_array_size(zoneServers); id++) {
        ZoneServerConf *curConf = &self->zonesConf.confs[id];
        if (!(globalServerReadBasicServerConf(&curConf->basicConf, zoneServers, id))) {
            error("Cannot read zone server entry %d in 'zoneServers'", id);
            result = false;
            goto cleanup;
        }
    }

    // ===================================
    //       Read MySQL configuration
    // ===================================
    json_t *mysqlServer;
    if (!(mysqlServer = json_object_get(root, "mysqlServer"))
    ||  !(json_is_object(mysqlServer)))
    {
        error("Cannot read 'mysqlServer' section correctly.");
        result = false;
        goto cleanup;
    }

    // read host
    if (!(field = json_object_get(mysqlServer, "host"))
    ||  !(json_is_string(field)))
    {
        error("Cannot read 'host' field in 'mysqlServer' section correctly.");
        result = false;
        goto cleanup;
    }
    self->sqlInfo.hostname = strdup(json_string_value(field));

    // read user
    if (!(field = json_object_get(mysqlServer, "user"))
    ||  !(json_is_string(field)))
    {
        error("Cannot read 'user' field in 'mysqlServer' section correctly.");
        result = false;
        goto cleanup;
    }
    self->sqlInfo.user = strdup(json_string_value(field));

    // read password
    if (!(field = json_object_get(mysqlServer, "password"))
    ||  !(json_is_string(field)))
    {
        error("Cannot read 'password' field in 'mysqlServer' section correctly.");
        result = false;
        goto cleanup;
    }
    self->sqlInfo.password = strdup(json_string_value(field));

    // read database
    if (!(field = json_object_get(mysqlServer, "database"))
    ||  !(json_is_string(field)))
    {
        error("Cannot read 'database' field in 'mysqlServer' section correctly.");
        result = false;
        goto cleanup;
    }
    self->sqlInfo.database = strdup(json_string_value(field));

    // ===================================
    //       Read Redis configuration
    // ===================================
    json_t *redisServer;
    if (!(redisServer = json_object_get(root, "redisServer"))
    ||  !(json_is_object(redisServer)))
    {
        error("Cannot read 'redisServer' section correctly.");
        result = false;
        goto cleanup;
    }

    // read host
    if (!(field = json_object_get(redisServer, "host"))
    ||  !(json_is_string(field)))
    {
        error("Cannot read 'host' field in 'redisServer' section correctly.");
        result = false;
        goto cleanup;
    }
    self->redisInfo.hostname = strdup(json_string_value(field));

    // read port
    if (!(field = json_object_get(redisServer, "port"))
    ||  !(json_is_string(field)))
    {
        error("Cannot read 'port' field in 'redisServer' section correctly.");
        result = false;
        goto cleanup;
    }
    self->redisInfo.port = atoi(json_string_value(field));

cleanup:
    // close the configuration file
    free(confStr);
    return result;
}

int globalServerHandleZonesRequest(GlobalServer *self, zsock_t *zone) {
    zmsg_t *msg;

    if (!(msg = zmsg_recv(zone))) {
        dbg("Global Server stops working.");
        return -2;
    }

    return 0;
}

int globalServerHandleCliRequest(GlobalServer *self, zsock_t *zone) {
    zmsg_t *msg;

    if (!(msg = zmsg_recv(zone))) {
        dbg("Global Server stops working.");
        return -2;
    }

    return 0;
}

bool globalServerStart(GlobalServer *self) {
    #ifdef WIN32
        SetConsoleTitle("GlobalServer");
    #endif // WIN32

    special("======================");
    special("=== Global server ====");
    special("======================");

    GlobalServerStartupInfo *globalInfo = &self->info;

    zpoller_t *poller;
    bool isRunning = true;
    ServerStartupInfo serverInfo;

    // ===================================
    //     Initialize CLI connection
    // ===================================
    // CLI should communicates through BSD sockets
    zsock_set_router_raw(self->cliConnection, true);

    if (zsock_bind(self->cliConnection, GLOBAL_SERVER_CLI_ENDPOINT,
            globalInfo->globalConf.ip, globalInfo->globalConf.port) == -1) {
        error("Failed to bind CLI port.");
        return false;
    }

    info("CLI connection binded on port %s.",
         zsys_sprintf(GLOBAL_SERVER_CLI_ENDPOINT,
            globalInfo->globalConf.ip, globalInfo->globalConf.port));

    // ===================================
    //     Initialize Zones connection
    // ===================================
    if ((globalInfo->zonesConf.globalZonePort = zsock_bind(self->zonesConnection,
            GLOBAL_SERVER_ZONES_ENDPOINT, globalInfo->globalConf.ip)) == -1)
    {
        error("Failed to bind zones port.");
        return false;
    }

    info("Zones connection binded on port %s.",
         zsys_sprintf(GLOBAL_SERVER_ZONES_ENDPOINT, globalInfo->globalConf.ip));

    // ===================================
    //     Initialize N Barrack Server
    // ===================================
    for (uint16_t id = 0; id < globalInfo->barracksConf.count; id++) {
        BarrackServerConf *curConf = &globalInfo->barracksConf.confs[id];
        BasicServerConf *basicConf = &curConf->basicConf;
        if (!(serverFactoryInitServerInfo(&serverInfo,
            SERVER_TYPE_BARRACK,
            BARRACKS_SERVER_ROUTER_ID + id,
            basicConf->ip,
            basicConf->port,
            basicConf->workersCount,
            globalInfo->globalConf.ip, globalInfo->globalConf.port,
            globalInfo->sqlInfo.hostname, globalInfo->sqlInfo.user,
            globalInfo->sqlInfo.password, globalInfo->sqlInfo.database,
            globalInfo->redisInfo.hostname, globalInfo->redisInfo.port)))
        {
            error("[Barrack] Cannot create a new ServerInfo.");
            return false;
        }

        if (!(serverCreateProcess(&serverInfo, ZONE_SERVER_EXECUTABLE_NAME))) {
            error("[Barrack] Can't launch a new Server process.");
            return false;
        }
    }

    // ===================================
    //     Initialize N Social Server
    // ===================================
    for (uint16_t id = 0; id < globalInfo->socialsConf.count; id++) {
        SocialServerConf *curConf = &globalInfo->socialsConf.confs[id];
        BasicServerConf *basicConf = &curConf->basicConf;
        if (!(serverFactoryInitServerInfo(&serverInfo,
            SERVER_TYPE_SOCIAL,
            SOCIALS_SERVER_ROUTER_ID + id,
            basicConf->ip,
            basicConf->port,
            basicConf->workersCount,
            globalInfo->globalConf.ip, globalInfo->globalConf.port,
            globalInfo->sqlInfo.hostname, globalInfo->sqlInfo.user,
            globalInfo->sqlInfo.password, globalInfo->sqlInfo.database,
            globalInfo->redisInfo.hostname, globalInfo->redisInfo.port)))
        {
            error("[Social] Cannot create a new ServerInfo.");
            return false;
        }

        if (!(serverCreateProcess(&serverInfo, ZONE_SERVER_EXECUTABLE_NAME))) {
            error("[Social] Can't launch a new Server process.");
            return false;
        }
    }

    // ===================================
    //     Initialize N Zone Server
    // ===================================
    for (uint16_t id = 0; id < globalInfo->zonesConf.count; id++) {
        ZoneServerConf *curConf = &globalInfo->zonesConf.confs[id];
        BasicServerConf *basicConf = &curConf->basicConf;
        if (!(serverFactoryInitServerInfo(&serverInfo,
            SERVER_TYPE_ZONE,
            ZONES_SERVER_ROUTER_ID + id,
            basicConf->ip,
            basicConf->port,
            basicConf->workersCount,
            globalInfo->globalConf.ip, globalInfo->globalConf.port,
            globalInfo->sqlInfo.hostname, globalInfo->sqlInfo.user,
            globalInfo->sqlInfo.password, globalInfo->sqlInfo.database,
            globalInfo->redisInfo.hostname, globalInfo->redisInfo.port)))
        {
            error("[Zone] Cannot create a new ServerInfo.");
            return false;
        }

        if (!(serverCreateProcess(&serverInfo, ZONE_SERVER_EXECUTABLE_NAME))) {
            error("[Zone] Can't launch a new Server process.");
            return false;
        }
    }

    // define a poller with the zones and the CLI sockets
    if (!(poller = zpoller_new(self->cliConnection, self->zonesConnection, NULL))) {
        error("Global server cannot create a poller.");
        return false;
    }

    // listens to requests
    info("GlobalServer is ready and running.");

    while (isRunning) {
        zsock_t *actor = zpoller_wait(poller, -1);
        typedef int (*GlobalServerRequestHandler)(GlobalServer *self, zsock_t *actor);
        GlobalServerRequestHandler handler;

        // get the correct handler based on the actor
        if (actor == self->zonesConnection) {
            handler = globalServerHandleZonesRequest;
        } else if (actor == self->cliConnection) {
            handler = globalServerHandleCliRequest;
        }  else {
            warning("An unknown actor talked to the Global Server. Maybe SIGINT signal?");
            break;
        }

        switch (handler(self, actor)) {
            case -1: // ERROR
                error("Global Server encountered an error when handling a request.");
            case -2: // connection stopped
                isRunning = false;
            break;
            case 0: // OK
            break;
        }
    }

    return true;
}

void globalServerDestroy(GlobalServer **_self) {
    GlobalServer *self = *_self;

    free(self);
    *_self = NULL;
}
