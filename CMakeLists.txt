# minimum CMake version required
cmake_minimum_required(VERSION 3.22)
# Project name, version and description
project(p4dc VERSION 0.1 DESCRIPTION "P4DC bot by Alan Tao")

add_subdirectory(libs/DPP)
# add_subdirectory(libs/SQLiteCpp)

configure_file (env.h.in env.h @ONLY)
 
# Create an executable
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/api/create_role.cpp
    src/api/integration_help.cpp
    src/api/route_here.cpp
)
 
# Linking libraries
target_link_libraries(${PROJECT_NAME}
    dpp
#    SQLiteCpp
#    sqlite3
#    pthread
#    dl
)
 
# Specify includes
target_include_directories(${PROJECT_NAME} PRIVATE
    libs/DPP/include
    "include"
    ${CMAKE_CURRENT_BINARY_DIR}
)

# Set C++ version
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)