set(SRCS
    channel.cc
    epoller.cc
    eventloop.cc
    eventloopthread.cc
    eventloopthreadpool.cc
    httpdata.cc
    server.cc
)
include_directories(${PROJECT_SOURCE_DIR})

add_library(libserver_base ${SRCS})
target_link_libraries(libserver_base pthread rt)
set_target_properties(libserver_base PROPERTIES OUTPUT_NAME "server_base")