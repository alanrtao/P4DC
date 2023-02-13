# minimum CMake version required
cmake_minimum_required(VERSION 3.22)
# Project name, version and description
project(p4dc VERSION 0.1 DESCRIPTION "P4DC bot by Alan Tao")

# add_subdirectory(libs/DPP)
# add_subdirectory(libs/SQLiteCpp)
add_library(dpp SHARED IMPORTED)
set_property(TARGET dpp PROPERTY
    IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/libs/DPP/lib/libdpp.so
)
find_package(LibArchive REQUIRED)
configure_file (env.h.in env.h @ONLY)
file (COPY res/templates DESTINATION .)

# Create an executable
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/msg_handlers/pull_request.cpp
    src/slash_commands/integration_help.cpp
    src/slash_commands/route_here.cpp
    src/utils/hydratable.cpp
    src/utils/api_utils.cpp
)

set(BUILD_VOICE_SUPPORT OFF)
 
# Linking libraries
target_link_libraries(${PROJECT_NAME}
    LibArchive::LibArchive
    dpp
#    SQLiteCpp
#    sqlite3
#    pthread
#    dl
)
 
# Specify includes
target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/libs/DPP/include"
    "${CMAKE_CURRENT_LIST_DIR}/include"
    ${CMAKE_CURRENT_BINARY_DIR}
)

# Set C++ version
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)