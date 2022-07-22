set(SRCS
    thread.cpp
    timer.cc
    util.cc
)
include_directories(${PROJECT_SOURCE_DIR})
# include_directories(${PROJECT_SOURCE_DIR}/log)
# include_directories(${PROJECT_SOURCE_DIR}/server)
add_library(libserver_util ${SRCS})
target_link_libraries(libserver_util pthread rt)
set_target_properties(libserver_util PROPERTIES OUTPUT_NAME "server_util")