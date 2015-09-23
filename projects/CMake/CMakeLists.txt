cmake_minimum_required(VERSION 3.0)
project(R1Emu)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99")
add_definitions(-D__DBG_ACTIVATED__)

set(ROOT_PATH ${PROJECT_SOURCE_DIR}/../../src/)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../../bin)
link_directories(${PROJECT_SOURCE_DIR}/../../lib/x64)
include_directories(
    ${ROOT_PATH}
    ${ROOT_PATH}/../include
    ${ROOT_PATH}/../include/mysql
    ${ROOT_PATH}/../include/zlib
    ${ROOT_PATH}/../include/hiredis
    ${ROOT_PATH}/../include/zmq)

set(ZONEHANDLERS_SRC
    ${ROOT_PATH}/zone_server/zone_handler/admin_cmd.c
    ${ROOT_PATH}/zone_server/zone_handler/admin_cmd.h
    ${ROOT_PATH}/zone_server/zone_handler/zone_builder.c
    ${ROOT_PATH}/zone_server/zone_handler/zone_builder.h
    ${ROOT_PATH}/zone_server/zone_handler/zone_handler.c
    ${ROOT_PATH}/zone_server/zone_handler/zone_handler.h
    ${ROOT_PATH}/zone_server/zone_event_server.c
    ${ROOT_PATH}/zone_server/zone_event_server.h
    ${ROOT_PATH}/zone_server/zone_handler/zone_event_handler.c
    ${ROOT_PATH}/zone_server/zone_handler/zone_event_handler.h
)
add_library(zone_handlers STATIC ${ZONEHANDLERS_SRC})
target_link_libraries(zone_handlers common)

set(COMMON_SRC
    ${ROOT_PATH}/common/commander/commander.c
    ${ROOT_PATH}/common/commander/inventory.c
    ${ROOT_PATH}/common/crypto/bf/bf_ecb.c
    ${ROOT_PATH}/common/crypto/bf/bf_enc.c
    ${ROOT_PATH}/common/crypto/bf/bf_skey.c
    ${ROOT_PATH}/common/crypto/crypto.c
    ${ROOT_PATH}/common/db/db_client.c
    ${ROOT_PATH}/common/db/db_object.c
    ${ROOT_PATH}/common/db/db.c
    ${ROOT_PATH}/common/dbg/dbg.c
    ${ROOT_PATH}/common/graph/graph.c
    ${ROOT_PATH}/common/item/item.c
    ${ROOT_PATH}/common/item/item_attribute.c
    ${ROOT_PATH}/common/mysql/fields/mysql_session.c
    ${ROOT_PATH}/common/mysql/fields/mysql_account_session.c
    ${ROOT_PATH}/common/mysql/fields/mysql_commander_session.c
    ${ROOT_PATH}/common/mysql/fields/mysql_commander.c
    ${ROOT_PATH}/common/mysql/mysql.c
    ${ROOT_PATH}/common/packet/packet.c
    ${ROOT_PATH}/common/packet/packet_stream.c
    ${ROOT_PATH}/common/packet/packet_type.c
    ${ROOT_PATH}/common/redis/fields/redis_game_session.c
    ${ROOT_PATH}/common/redis/fields/redis_session.c
    ${ROOT_PATH}/common/redis/fields/redis_socket_session.c
    ${ROOT_PATH}/common/redis/redis.c
    ${ROOT_PATH}/common/server/event_handler.c
    ${ROOT_PATH}/common/server/event_server.c
    ${ROOT_PATH}/common/server/router.c
    ${ROOT_PATH}/common/server/router_monitor.c
    ${ROOT_PATH}/common/server/server.c
    ${ROOT_PATH}/common/server/server_factory.c
    ${ROOT_PATH}/common/server/worker.c
    ${ROOT_PATH}/common/session/account_session.c
    ${ROOT_PATH}/common/session/barrack_session.c
    ${ROOT_PATH}/common/session/commander_session.c
    ${ROOT_PATH}/common/session/game_session.c
    ${ROOT_PATH}/common/session/session.c
    ${ROOT_PATH}/common/session/socket_session.c
    ${ROOT_PATH}/common/utils/math.c
    ${ROOT_PATH}/common/utils/memory.c
    ${ROOT_PATH}/common/utils/position.c
    ${ROOT_PATH}/common/utils/random.c
    ${ROOT_PATH}/common/utils/string.c
    ${ROOT_PATH}/common/utils/time.c
    ${ROOT_PATH}/common/utils/utils.c
    ${ROOT_PATH}/common/utils/zlib.c
)
add_library(common STATIC ${COMMON_SRC})
target_link_libraries(common z zone_handlers)

set(BARRACK_SRC
    ${ROOT_PATH}/barrack_server/barrack_handler/barrack_builder.c
    ${ROOT_PATH}/barrack_server/barrack_handler/barrack_handler.c
    ${ROOT_PATH}/barrack_server/barrack_server.c
    ${ROOT_PATH}/barrack_server/barrack_event_server.c
    ${ROOT_PATH}/barrack_server/barrack_event_server.h
    ${ROOT_PATH}/barrack_server/barrack_handler/barrack_event_handler.c
    ${ROOT_PATH}/barrack_server/barrack_handler/barrack_event_handler.h
)
add_library(barrack STATIC ${BARRACK_SRC})
target_link_libraries(barrack common)

set(SOCIAL_SRC
    ${ROOT_PATH}/social_server/social_handler/social_handler.c
    ${ROOT_PATH}/social_server/social_handler/social_handler.h
    ${ROOT_PATH}/social_server/social_server.c
    ${ROOT_PATH}/social_server/social_server.h
    ${ROOT_PATH}/social_server/social_event_server.c
    ${ROOT_PATH}/social_server/social_event_server.h
    ${ROOT_PATH}/social_server/social_handler/social_event_handler.c
    ${ROOT_PATH}/social_server/social_handler/social_event_handler.h
)
add_library(social STATIC ${SOCIAL_SRC})
target_link_libraries(social common)

set(LIBS common barrack social z czmq hiredis zmq mysqlclient jansson)

set(GLOBAL_SRC
    ${ROOT_PATH}/global_server/global_server.c
    ${ROOT_PATH}/global_server/global_server.h
    ${ROOT_PATH}/global_server/main.c
)
add_executable(global_server ${GLOBAL_SRC})
target_link_libraries(global_server ${LIBS})

set(ZONE_SRC
        ${ROOT_PATH}/zone_server/main.c
        ${ROOT_PATH}/zone_server/zone_server.c
        ${ROOT_PATH}/zone_server/zone_server.h
)
add_executable(zone_server ${ZONE_SRC} )
target_link_libraries(zone_server ${LIBS})
