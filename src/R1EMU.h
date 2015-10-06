/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file R1EMU.h
 * @brief Global variables shared between all the files
 *
 *  It also contains utilities functions.
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#pragma once

// integers printf format
#include <inttypes.h>

// CZMQ environment
#include <zmq.h>
#include <czmq.h>
/** This is only for differenciate ZMQ RAW ROUTER from ZMQ ROUTER */
#define ZMQ_RAW_ROUTER ZMQ_ROUTER

// MySQL environment
#include <mysql/mysql.h>

// redis environment
#include <hiredis.h>

// zlib environment
#include <zlib.h>

// debugging environment
/** If defined, the debugging functions will be included in the executable */
#include "common/dbg/dbg.h"

// utils
#include "common/utils/utils.h"
#include "common/utils/memory.h"

// Servers
/** @brief Global types of the servers of the architecture */
typedef enum {
    SERVER_TYPE_GLOBAL,
    SERVER_TYPE_BARRACK,
    SERVER_TYPE_SOCIAL,
    SERVER_TYPE_ZONE
} ServerType;

// Global types
typedef uint16_t MapId_t;
typedef uint16_t RouterId_t;
typedef uint8_t GameMode_t;
