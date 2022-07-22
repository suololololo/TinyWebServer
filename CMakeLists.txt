cmake_minimum_required(VERSION 2.8.3)

project(Webserver)

include_directories(${PROJECT_SOURCE_DIR})

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

add_executable(main main.cc)
target_link_libraries(main libserver_base)
target_link_libraries(main libserver_log)
target_link_libraries(main libserver_util)
add_subdirectory(server)
add_subdirectory(log)
add_subdirectory(util)





