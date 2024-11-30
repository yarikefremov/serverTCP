cmake_minimum_required(VERSION 3.16)
project(serverTCP LANGUAGES CXX C)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

add_executable(main src/main.cpp
    src/tcpserver.cpp
    include/tcpserver.h
    include/dbtxt.h src/dbtxt.cpp
    include/pckg.h
)

#резерв для db
#add_library(sqlite3c src/sqlite3.c)
#set_target_properties(sqlite3c PROPERTIES
#    IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/lib/sqlite3c.lib"
#    LINK_FLAGS "/DEF:${CMAKE_SOURCE_DIR}/lib/sqlite3.def"
#)

target_link_libraries(main PRIVATE wsock32 ws2_32) #fwpuclnt ntdsapi sqlite3c
target_compile_features(main PRIVATE cxx_std_17)

#All server applications must set SO_EXCLUSIVEADDRUSE maybe my too)
