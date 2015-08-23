/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @license <license placeholder>
 */

// ---------- Includes ------------
#include "GlobalServer.h"
#include "ZoneServer/ZoneServer.h"
#include "SocialServer/SocialServer.h"
#include "BarrackServer/BarrackServer.h"
#include "Common/Server/ServerFactory.h"


// ------ Structure declaration -------
/**
 * @brief GlobalServer detains the authority on all the zone servers
 * It communicates with the nodes that need to communicates with all the zone servers.
 */
struct GlobalServer
{
    /** Global information */
    GlobalServerStartupInfo info;

    /** Socket listening to the CLI */
    zsock_t *cliConnection;

    /** Socket talking to the zones */
    zsock_t *zonesConnection;

    /** Connection to Redis */
    Redis *redis;
};


// ------ Static declaration ------


// ------ Extern function implementation ------

GlobalServer *
GlobalServer_new (
    GlobalServerStartupInfo *info
) {
    GlobalServer *self;

    if ((self = calloc (1, sizeof (GlobalServer))) == NULL) {
        return NULL;
    }

    if (!GlobalServer_init (self, info)) {
        GlobalServer_destroy (&self);
        error ("GlobalServer failed to initialize.");
        return NULL;
    }

    return self;
}


bool
GlobalServer_init (
    GlobalServer *self,
    GlobalServerStartupInfo *info
) {
    memcpy (&self->info, info, sizeof (self->info));

    // ==========================
    //   Allocate ZMQ objects
    // ==========================
    if (!(self->cliConnection = zsock_new (ZMQ_RAW_ROUTER))) {
        error ("Cannot allocate a new CLI zsock.");
        return false;
    }
    if (!(self->zonesConnection = zsock_new (ZMQ_REQ))) {
        error ("Cannot allocate a new zones zsock.");
        return false;
    }

    // Connect to Redis Server
    if (!(self->redis = Redis_new (&self->info.redisInfo))) {
        error ("Cannot initialize Redis connection.");
        return false;
    }

    if (!(Redis_connect (self->redis))) {
        error ("Cannot connect to Redis.");
        return false;
    }

    return true;
}


bool
GlobalServer_flushRedis (
    GlobalServer *self
) {
    return Redis_flushDatabase (self->redis);
}

bool
GlobalServerStartupInfo_init (
    GlobalServerStartupInfo *self,
    char *confFilePath
) {
    memset (self, 0, sizeof (GlobalServerStartupInfo));

    bool result = true;
    zconfig_t *conf = NULL;
    char *portsArray = NULL;

    // ===================================
    //       Read Global configuration
    // ===================================
    // Open the configuration file
    if (!(conf = zconfig_load (confFilePath))) {
        error ("Cannot read the global configuration file (%s).", confFilePath);
        result = false;
        goto cleanup;
    }

    // Read the CLI serverIP
    if (!(self->ip = strdup (zconfig_resolve (conf, "globalServer/serverIP", NULL)))
    ) {
        warning ("Cannot read correctly the CLI serverIP in the configuration file (%s). ", confFilePath);
        warning ("The default serverIP = %s has been used.", GLOBAL_SERVER_CLI_IP_DEFAULT);
        self->ip = GLOBAL_SERVER_CLI_IP_DEFAULT;
    }

    // Read the CLI port
    if (!(self->cliPort = atoi (zconfig_resolve (conf, "globalServer/port", NULL)))
    ) {
        warning ("Cannot read correctly the CLI port in the configuration file (%s). ", confFilePath);
        warning ("The default port = %d has been used.", GLOBAL_SERVER_CLI_PORT_DEFAULT);
        self->cliPort = GLOBAL_SERVER_CLI_PORT_DEFAULT;
    }

    // ===================================
    //       Read Zone configuration
    // ===================================
    // Read the zone ports array
    if (!(portsArray = zconfig_resolve (conf, "zoneServer/portsArray", NULL))) {
        warning ("Public Zone ports cannot be read for Global Server. Defaults ports have been used : %s", ZONE_SERVER_PORTS_DEFAULT);
        portsArray = ZONE_SERVER_PORTS_DEFAULT;
    }

    // Tokenize the ports array
    char *port = strtok (portsArray, " ");
    while (port != NULL) {
        self->zoneServersCount++;
        port = strtok (NULL, " ");
    }

    if (self->zoneServersCount == 0) {
        error ("Cannot read correctly the zone ports array.");
        result = false;
        goto cleanup;
    }

    // Fill the server ports array
    self->zoneServersPorts = calloc (self->zoneServersCount, sizeof (int));
    for (int portIndex = 0; portIndex < self->zoneServersCount; portIndex++) {
        self->zoneServersPorts[portIndex] = strtoul (portsArray, &portsArray, 10);
        portsArray++;
    }

    // Read the number of zone workers
    if (!(self->zoneWorkersCount = atoi (zconfig_resolve (conf, "zoneServer/workersCount", NULL)))) {
        warning ("Cannot read correctly the zone workers count in the configuration file (%s). ", confFilePath);
        warning ("The default worker count = %d has been used.", ZONE_SERVER_WORKERS_COUNT_DEFAULT);
        self->zoneWorkersCount = ZONE_SERVER_WORKERS_COUNT_DEFAULT;
    }

    char *zoneServersIp;
    // Read the zone server interfaces IP
    if (!(zoneServersIp = zconfig_resolve (conf, "zoneServer/serversIP", NULL))) {
        error ("Cannot read correctly the zone servers interface IP in the configuration file (%s). ", confFilePath);
        result = false;
        goto cleanup;
    }

    int nbZoneServersIp = 0;
    char *routerIp = strtok (zoneServersIp, " ");
    while (routerIp != NULL) {
        routerIp = strtok (NULL, " ");
        nbZoneServersIp++;
    }

    if (nbZoneServersIp != self->zoneServersCount) {
        error ("Number of zone ports different from the number of zone interfaces IP. (%d / %d)",
            nbZoneServersIp, self->zoneServersCount
        );
        result = false;
        goto cleanup;
    }

    // Fill the zone server IPs array
    self->zoneServersIp = calloc (self->zoneServersCount, sizeof (char *));
    for (int ipIndex = 0; ipIndex < self->zoneServersCount; ipIndex++) {
        self->zoneServersIp[ipIndex] = strdup (zoneServersIp);
        zoneServersIp += strlen (zoneServersIp) + 1;
    }

    // ===================================
    //       Read Social configuration
    // ===================================
    // Read the social ports array
    if (!(portsArray = zconfig_resolve (conf, "socialServer/portsArray", NULL))) {
        warning ("Public Social ports cannot be read for Global Server. Defaults ports have been used : %s", SOCIAL_SERVER_PORTS_DEFAULT);
        portsArray = SOCIAL_SERVER_PORTS_DEFAULT;
    }

    // Tokenize the ports array
    port = strtok (portsArray, " ");
    while (port != NULL) {
        self->socialServersCount++;
        port = strtok (NULL, " ");
    }

    if (self->socialServersCount == 0) {
        error ("Cannot read correctly the social ports array.");
        result = false;
        goto cleanup;
    }

    // Fill the server ports array
    self->socialServersPorts = calloc (self->socialServersCount, sizeof (int));
    for (int portIndex = 0; portIndex < self->socialServersCount; portIndex++) {
        self->socialServersPorts[portIndex] = strtoul (portsArray, &portsArray, 10);
        portsArray++;
    }

    // Read the number of social workers
    if (!(self->socialWorkersCount = atoi (zconfig_resolve (conf, "socialServer/workersCount", NULL)))) {
        warning ("Cannot read correctly the social workers count in the configuration file (%s). ", confFilePath);
        warning ("The default worker count = %d has been used.", SOCIAL_SERVER_WORKERS_COUNT_DEFAULT);
        self->socialWorkersCount = SOCIAL_SERVER_WORKERS_COUNT_DEFAULT;
    }

    char *socialServersIp;
    // Read the social server interfaces IP
    if (!(socialServersIp = zconfig_resolve (conf, "socialServer/serversIP", NULL))) {
        error ("Cannot read correctly the social servers interface IP in the configuration file (%s). ", confFilePath);
        result = false;
        goto cleanup;
    }

    int nbSocialServersIp = 0;
    routerIp = strtok (socialServersIp, " ");
    while (routerIp != NULL) {
        routerIp = strtok (NULL, " ");
        nbSocialServersIp++;
    }

    if (nbSocialServersIp != self->socialServersCount) {
        error ("Number of social ports different from the number of social interfaces IP. (%d / %d)",
            nbSocialServersIp, self->socialServersCount
        );
        result = false;
        goto cleanup;
    }

    // Fill the social server IPs array
    self->socialServersIp = calloc (self->socialServersCount, sizeof (char *));
    for (int ipIndex = 0; ipIndex < self->socialServersCount; ipIndex++) {
        self->socialServersIp[ipIndex] = strdup (socialServersIp);
        socialServersIp += strlen (socialServersIp) + 1;
    }

    // ===================================
    //       Read Barrack configuration
    // ===================================
    // Read the ports array
    if (!(portsArray = zconfig_resolve (conf, "barrackServer/portsArray", NULL))) {
        warning ("Ports cannot be read for Barrack Server. Defaults ports have been used : %s", BARRACK_SERVER_PORTS_DEFAULT);
        portsArray = BARRACK_SERVER_PORTS_DEFAULT;
    }

    // Tokenize the ports array
    port = strtok (portsArray, " ");
    while (port != NULL) {
        self->barrackServerPortCount++;
        port = strtok (NULL, " ");
    }

    // Fill the server ports array
    self->barrackServerPort = calloc (self->barrackServerPortCount, sizeof (int));
    for (int portIndex = 0; portIndex < self->barrackServerPortCount; portIndex++) {
        self->barrackServerPort[portIndex] = strtoul (portsArray, &portsArray, 10);
        portsArray++;
    }

    // Read the number of barrack server workers
    if (!(self->barrackWorkersCount = atoi (zconfig_resolve (conf, "barrackServer/workersCount", NULL)))) {
        warning ("Cannot read correctly the barrack workers count in the configuration file (%s). ", confFilePath);
        warning ("The default worker count = %d has been used.", BARRACK_SERVER_WORKERS_COUNT_DEFAULT);
        self->barrackWorkersCount = BARRACK_SERVER_WORKERS_COUNT_DEFAULT;
    }

    // Read the server interface IP
    if (!(self->barrackServerIp = strdup (zconfig_resolve (conf, "barrackServer/serverIP", NULL)))) {
        warning ("Cannot read correctly the barrack interface IP in the configuration file (%s). ", confFilePath);
        warning ("The default IP = %s has been used.", BARRACK_SERVER_FRONTEND_IP_DEFAULT);
        self->barrackServerIp = BARRACK_SERVER_FRONTEND_IP_DEFAULT;
    }

    // ===================================
    //       Read MySQL configuration
    // ===================================
    if (!(self->sqlInfo.hostname = strdup (zconfig_resolve (conf, "database/mysql_host", NULL)))
    ) {
        warning ("Cannot read correctly the MySQL host in the configuration file (%s). ", confFilePath);
        warning ("The default hostname = %s has been used.", MYSQL_HOSTNAME_DEFAULT);
        self->sqlInfo.hostname = MYSQL_HOSTNAME_DEFAULT;
    }
    if (!(self->sqlInfo.login = strdup (zconfig_resolve (conf, "database/mysql_user", NULL)))
    ) {
        warning ("Cannot read correctly the MySQL user in the configuration file (%s). ", confFilePath);
        warning ("The default hostname = %s has been used.", MYSQL_LOGIN_DEFAULT);
        self->sqlInfo.login = MYSQL_LOGIN_DEFAULT;
    }
    if (!(self->sqlInfo.password = strdup (zconfig_resolve (conf, "database/mysql_password", NULL)))
    ) {
        warning ("Cannot read correctly the MySQL password in the configuration file (%s). ", confFilePath);
        warning ("The default hostname = %s has been used.", MYSQL_PASSWORD_DEFAULT);
        self->sqlInfo.password = MYSQL_PASSWORD_DEFAULT;
    }
    if (!(self->sqlInfo.database = strdup (zconfig_resolve (conf, "database/mysql_database", NULL)))
    ) {
        warning ("Cannot read correctly the MySQL database in the configuration file (%s). ", confFilePath);
        warning ("The default hostname = %s has been used.", MYSQL_DATABASE_DEFAULT);
        self->sqlInfo.database = MYSQL_DATABASE_DEFAULT;
    }

    // ===================================
    //       Read Redis configuration
    // ===================================
    if (!(self->redisInfo.hostname = strdup (zconfig_resolve (conf, "redisServer/redis_host", NULL)))
    ) {
        warning ("Cannot read correctly the Redis host in the configuration file (%s). ", confFilePath);
        warning ("The default hostname = %s has been used.", REDIS_HOSTNAME_DEFAULT);
        self->redisInfo.hostname = REDIS_HOSTNAME_DEFAULT;
    }
    if (!(self->redisInfo.port = atoi (zconfig_resolve (conf, "redisServer/redis_port", NULL)))
    ) {
        warning ("Cannot read correctly the Redis port in the configuration file (%s). ", confFilePath);
        warning ("The default hostname = %d has been used.", REDIS_PORT_DEFAULT);
        self->redisInfo.port = REDIS_PORT_DEFAULT;
    }


cleanup:
    // Close the configuration file
    zconfig_destroy (&conf);

    return result;
}


int
GlobalServer_handleZonesRequest (
    GlobalServer *self,
    zsock_t *zone
) {
    zmsg_t *msg;

    if (!(msg = zmsg_recv (zone))) {
        dbg ("Global Server stops working.");
        return -2;
    }

    return 0;
}

int
GlobalServer_handleCliRequest (
    GlobalServer *self,
    zsock_t *zone
) {
    zmsg_t *msg;

    if (!(msg = zmsg_recv (zone))) {
        dbg ("Global Server stops working.");
        return -2;
    }

    return 0;
}


bool
GlobalServer_start (
    GlobalServer *self
) {
    #ifdef WIN32
        SetConsoleTitle ("GlobalServer");
    #endif // WIN32

    special ("======================");
    special ("=== Global server ====");
    special ("======================");

    GlobalServerStartupInfo *info = &self->info;

    zpoller_t *poller;
    bool isRunning = true;
    ServerStartupInfo serverInfo;

    // ===================================
    //     Initialize CLI connection
    // ===================================
    // CLI should communicates through BSD sockets
    zsock_set_router_raw (self->cliConnection, true);

    if (zsock_bind (self->cliConnection, GLOBAL_SERVER_CLI_ENDPOINT, info->ip, info->cliPort) == -1) {
        error ("Failed to bind CLI port.");
        return false;
    }
    info ("CLI connection binded on port %s.", zsys_sprintf (GLOBAL_SERVER_CLI_ENDPOINT, info->ip, info->cliPort));


    // ===================================
    //     Initialize Zones connection
    // ===================================
    if ((info->zonesPort = zsock_bind (self->zonesConnection, GLOBAL_SERVER_ZONES_ENDPOINT, info->ip)) == -1) {
        error ("Failed to bind zones port.");
        return false;
    }
    info ("Zones connection binded on port %s.", zsys_sprintf (GLOBAL_SERVER_ZONES_ENDPOINT, info->ip, info->zonesPort));

    // ===================================
    //     Initialize 1 Barrack Server
    // ===================================
    if (!(ServerFactory_initServerInfo (&serverInfo,
        SERVER_TYPE_BARRACK,
        BARRACK_SERVER_ROUTER_ID,
        info->barrackServerIp,
        info->barrackServerPortCount, info->barrackServerPort,
        info->zoneWorkersCount,
        info->ip, info->cliPort,
        info->sqlInfo.hostname, info->sqlInfo.login, info->sqlInfo.password, info->sqlInfo.database,
        info->redisInfo.hostname, info->redisInfo.port)
    )) {
        error ("[Barrack] Cannot create a new ServerInfo.");
        return false;
    }

    if (!(Server_createProcess (&serverInfo, ZONE_SERVER_EXECUTABLE_NAME))) {
        error ("[Barrack] Can't launch a new Server process.");
        return false;
    }

    // ===================================
    //     Initialize N Social Server
    // ===================================
    for (uint16_t routerId = 0; routerId < info->socialServersCount; routerId++) {
        ServerFactory_initServerInfo (&serverInfo,
            SERVER_TYPE_SOCIAL,
            SOCIAL_SERVER_ROUTER_ID - routerId,
            info->socialServersIp[routerId],
            1, &info->socialServersPorts[routerId], // Only 1 port for each social server
            info->socialWorkersCount,
            info->ip, info->cliPort,
            info->sqlInfo.hostname, info->sqlInfo.login, info->sqlInfo.password, info->sqlInfo.database,
            info->redisInfo.hostname, info->redisInfo.port
        );

        if (!(Server_createProcess (&serverInfo, ZONE_SERVER_EXECUTABLE_NAME))) {
            error ("[routerId=%d] Can't launch a new Server process.", routerId);
            return false;
        }
    }

    // ===================================
    //     Initialize N Zone Server
    // ===================================
    for (uint16_t routerId = 0; routerId < info->zoneServersCount; routerId++) {
        ServerFactory_initServerInfo (&serverInfo,
            SERVER_TYPE_ZONE,
            routerId,
            info->zoneServersIp[routerId],
            1, &info->zoneServersPorts[routerId], // Only 1 port for each Zone server
            info->zoneWorkersCount,
            info->ip, info->cliPort,
            info->sqlInfo.hostname, info->sqlInfo.login, info->sqlInfo.password, info->sqlInfo.database,
            info->redisInfo.hostname, info->redisInfo.port
        );

        if (!(Server_createProcess (&serverInfo, ZONE_SERVER_EXECUTABLE_NAME))) {
            error ("[routerId=%d] Can't launch a new Server process.", routerId);
            return false;
        }
    }

    // Define a poller with the zones and the CLI sockets
    if (!(poller = zpoller_new (self->cliConnection, self->zonesConnection, NULL))) {
        error ("Global server cannot create a poller.");
        return false;
    }

    // Listens to requests
    info ("GlobalServer is ready and running.");

    while (isRunning) {
        zsock_t *actor = zpoller_wait (poller, -1);
        typedef int (*GlobalServerRequestHandler) (GlobalServer *self, zsock_t *actor);
        GlobalServerRequestHandler handler;

        // Get the correct handler based on the actor
        if (actor == self->zonesConnection) {
            handler = GlobalServer_handleZonesRequest;
        } else if (actor == self->cliConnection) {
            handler = GlobalServer_handleCliRequest;
        }
        else {
            warning ("An unknown actor talked to the Global Server. Maybe SIGINT signal ?");
            break;
        }

        switch (handler (self, actor)) {
            case -1: // ERROR
                error ("Global Server encountered an error when handling a request.");
            case -2: // Connection stopped
                isRunning = false;
            break;

            case 0: // OK
            break;
        }
    }

    return true;
}


void
GlobalServer_destroy (
    GlobalServer **_self
) {
    GlobalServer *self = *_self;

    free (self);
    *_self = NULL;
}
