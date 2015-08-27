cmake_minimum_required(VERSION 3.3)
project(R1Emu)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99")

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
    ${ROOT_PATH}/ZoneServer/ZoneHandler/AdminCmd.c
    ${ROOT_PATH}/ZoneServer/ZoneHandler/AdminCmd.h
    ${ROOT_PATH}/ZoneServer/ZoneHandler/ZoneBuilder.c
    ${ROOT_PATH}/ZoneServer/ZoneHandler/ZoneBuilder.h
    ${ROOT_PATH}/ZoneServer/ZoneHandler/ZoneHandler.c
    ${ROOT_PATH}/ZoneServer/ZoneHandler/ZoneHandler.h
)
add_library(zone_handlers STATIC ${ZONEHANDLERS_SRC})
target_link_libraries(zone_handlers common)

set(COMMON_SRC
    ${ROOT_PATH}/Common/Commander/Commander.c
    ${ROOT_PATH}/Common/Crypto/bf/bf_ecb.c
    ${ROOT_PATH}/Common/Crypto/bf/bf_enc.c
    ${ROOT_PATH}/Common/Crypto/bf/bf_skey.c
    ${ROOT_PATH}/Common/Crypto/Crypto.c
    ${ROOT_PATH}/Common/dbg/dbg.c
    ${ROOT_PATH}/Common/Graph/Graph.c
    ${ROOT_PATH}/Common/MySQL/Fields/MySQLSession.c
    ${ROOT_PATH}/Common/MySQL/MySQL.c
    ${ROOT_PATH}/Common/Packet/Packet.c
    ${ROOT_PATH}/Common/Packet/PacketStream.c
    ${ROOT_PATH}/Common/Packet/PacketType.c
    ${ROOT_PATH}/Common/Redis/Fields/RedisGameSession.c
    ${ROOT_PATH}/Common/Redis/Fields/RedisSession.c
    ${ROOT_PATH}/Common/Redis/Fields/RedisSocketSession.c
    ${ROOT_PATH}/Common/Redis/Redis.c
    ${ROOT_PATH}/Common/Server/EventHandler.c
    ${ROOT_PATH}/Common/Server/EventServer.c
    ${ROOT_PATH}/Common/Server/Router.c
    ${ROOT_PATH}/Common/Server/RouterMonitor.c
    ${ROOT_PATH}/Common/Server/Server.c
    ${ROOT_PATH}/Common/Server/ServerFactory.c
    ${ROOT_PATH}/Common/Server/Worker.c
    ${ROOT_PATH}/Common/Session/AccountSession.c
    ${ROOT_PATH}/Common/Session/BarrackSession.c
    ${ROOT_PATH}/Common/Session/CommanderSession.c
    ${ROOT_PATH}/Common/Session/GameSession.c
    ${ROOT_PATH}/Common/Session/Session.c
    ${ROOT_PATH}/Common/Session/SocketSession.c
    ${ROOT_PATH}/Common/utils/math.c
    ${ROOT_PATH}/Common/utils/position.c
    ${ROOT_PATH}/Common/utils/random.c
    ${ROOT_PATH}/Common/utils/string.c
    ${ROOT_PATH}/Common/utils/time.c
    ${ROOT_PATH}/Common/utils/utils.c
    ${ROOT_PATH}/Common/utils/zlib.c
)
add_library(common STATIC ${COMMON_SRC})
target_link_libraries(common z zone_handlers)

set(BARRACK_SRC
    ${ROOT_PATH}/BarrackServer/BarrackHandler/BarrackBuilder.c
    ${ROOT_PATH}/BarrackServer/BarrackHandler/BarrackHandler.c
    ${ROOT_PATH}/BarrackServer/BarrackServer.c
)
add_library(barrack STATIC ${BARRACK_SRC})
target_link_libraries(barrack common)

set(SOCIAL_SRC
    ${ROOT_PATH}/SocialServer/SocialHandler/SocialHandler.c
    ${ROOT_PATH}/SocialServer/SocialHandler/SocialHandler.h
    ${ROOT_PATH}/SocialServer/SocialServer.c
    ${ROOT_PATH}/SocialServer/SocialServer.h
)
add_library(social STATIC ${SOCIAL_SRC})
target_link_libraries(social common)

set(LIBS common barrack social z czmq hiredis zmq mysqlclient)

set(GLOBAL_SRC
    ${ROOT_PATH}/GlobalServer/GlobalServer.c
    ${ROOT_PATH}/GlobalServer/GlobalServer.h
    ${ROOT_PATH}/GlobalServer/main.c
)
add_executable(GlobalServer ${GLOBAL_SRC})
target_link_libraries(GlobalServer ${LIBS})

set(ZONE_SRC
        ${ROOT_PATH}/ZoneServer/main.c
        ${ROOT_PATH}/ZoneServer/ZoneServer.c
        ${ROOT_PATH}/ZoneServer/ZoneServer.h
)
add_executable(ZoneServer ${ZONE_SRC} )
target_link_libraries(ZoneServer ${LIBS})