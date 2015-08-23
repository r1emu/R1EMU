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
 */

#pragma once

// Integers printf format
#include <inttypes.h>

// CZMQ environment
#include <zmq.h>
#include <czmq.h>
/** This is only for differenciate ZMQ RAW ROUTER from ZMQ ROUTER */
#define ZMQ_RAW_ROUTER ZMQ_ROUTER
#define PACKET_HEADER(x) (typeof(x)[]){x}

/** Global types of the servers of the architecture */
typedef enum {
    SERVER_TYPE_GLOBAL,
    SERVER_TYPE_BARRACK,
    SERVER_TYPE_SOCIAL,
    SERVER_TYPE_ZONE
} ServerType;

// MySQL environment
#include <mysql/mysql.h>

// Redis environment
#include <hiredis.h>

// zlib environment
#include <zlib.h>

// Debugging environment
/** If defined, the debugging functions will be included in the executable */
#define __DBG_ACTIVATED__ TRUE
#include "Common/dbg/dbg.h"

// Utils
#include "Common/utils/utils.h"
