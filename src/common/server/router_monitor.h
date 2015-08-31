/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file router_monitor.h
 * @brief
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#pragma once

#include "R1EMU.h"
#include "common/redis/redis.h"
#include "common/mysql/mysql.h"

#define ROUTER_MONITOR_FDKEY_SIZE ((sizeof(uint64_t) * 2) + 1)
#define ROUTER_MONITOR_SUBSCRIBER_ENDPOINT "inproc://routerMonitorSubscriber-%d"

/** Server specific handler when a client disconnects */
typedef bool (*DisconnectEventHandler) (zsock_t *eventServer, Redis *redis, uint16_t routerId, uint8_t *sessionKeyStr);

/** All the Router Monitor Packet headers */
typedef enum {
    ROUTER_MONITOR_ADD_FD,
    ROUTER_MONITOR_READY,
} RouterMonitorHeader;

typedef struct RouterMonitor RouterMonitor;

typedef struct {
    // the Router Id
    uint16_t routerId;

    // the Router frontend socket
    zsock_t *frontend;

    // database info
    RedisStartupInfo redisInfo;
    MySQLStartupInfo sqlInfo;

    /** Server specific handler when a client disconnects */
    DisconnectEventHandler disconnectHandler;

} RouterMonitorStartupInfo;

/**
 * @brief Allocate a new RouterMonitor structure.
 * @param info A RouterMonitorStartupInfo initialized
 * @return A pointer to an allocated RouterMonitor, or NULL if an error occured.
 */
RouterMonitor *routerMonitorNew(RouterMonitorStartupInfo *info);

/**
 * @brief Initialize an allocated RouterMonitor structure.
 * @param self An allocated RouterMonitor to initialize.
 * @param info A RouterMonitorStartupInfo initialized
 * @return true on success, false otherwise.
 */
bool routerMonitorInit(RouterMonitor *self, RouterMonitorStartupInfo *info);

/**
 * @brief Allocate a new RouterMonitorStartupInfo structure.
 * @param
 * @return A pointer to an allocated RouterMonitorStartupInfo, or NULL if an error occured.
 */
RouterMonitorStartupInfo *routerMonitorStartupInfoNew(
    zsock_t *frontend,
    uint16_t routerId,
    RedisStartupInfo *redisInfo,
    MySQLStartupInfo *sqlInfo,
    DisconnectEventHandler disconnectHandler);

/**
 * @brief Initialize an allocated RouterMonitorStartupInfo structure.
 * @param self An allocated RouterMonitorStartupInfo to initialize.
 * @param
 * @return true on success, false otherwise.
 */
bool routerMonitorStartupInfoInit(
    RouterMonitorStartupInfo *self,
    zsock_t *frontend,
    uint16_t routerId,
    RedisStartupInfo *redisInfo,
    MySQLStartupInfo *sqlInfo,
    DisconnectEventHandler disconnectHandler);

/**
 * @brief Start the Router Monitor
 * @param pipe A pipe to the parent process
 * @param info a RouterMonitorStartupInfo initialized
 */
void routerMonitorStart(zsock_t *pipe, void *info);

/**
 * @brief Initialize the FD key from the fd value
 * @param fd The file descriptor
 * @param[out] fdKey The formated fd key
 * @return
 */
void routerMonitorGenKey(uint64_t fd, uint8_t fdKey[ROUTER_MONITOR_FDKEY_SIZE]);

/**
 * @brief Free the content of a RouterMonitor structure.
 * @param self A pointer to an allocated RouterMonitor.
 */
void routerMonitorFree(RouterMonitor *self);

/**
 * @brief Free an allocated RouterMonitor structure and nullify the content of the pointer.
 * @param self A pointer to an allocated RouterMonitor.
 */
void routerMonitorDestroy(RouterMonitor **self);

/**
 * @brief Free an allocated RouterMonitorStartupInfo structure and nullify the content of the pointer.
 * @param self A pointer to an allocated RouterMonitorStartupInfo.
 */
void routerMonitorStartupInfoDestroy(RouterMonitorStartupInfo **_self);
