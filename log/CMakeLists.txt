set(SRCS
    async_logging.cc
    file_util.cc
    log_file.cc
    log_stream.cc
    logging.cc
)
include_directories(${PROJECT_SOURCE_DIR})
# include_directories(${PROJECT_SOURCE_DIR}/log)
# include_directories(${PROJECT_SOURCE_DIR}/server)
add_library(libserver_log ${SRCS})
target_link_libraries(libserver_log pthread rt)
set_target_properties(libserver_log PROPERTIES OUTPUT_NAME "server_log")