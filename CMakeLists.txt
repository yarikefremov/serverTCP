cmake_minimum_required(VERSION 3.16)
project(serverTCP LANGUAGES CXX C)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
#set(CMAKE_C_FLAGS "")
#set(CMAKE_C_FLAGS_RELEASE "")
#set(CMAKE_C_FLAGS_DEBUG "")
#set(CMAKE_C_ARCHIVE_CREATE "<CMAKE_AR> -qcD <TARGET> <LINK_FLAGS> <OBJECTS>")
#set(CMAKE_C_ARCHIVE_FINISH "<CMAKE_RANLIB> -D <TARGET>")
#set(CMAKE_C_COMPILE_OBJECT "gcc -c <FLAGS> -o <OBJECT>")

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)

add_executable(main src/main.cpp
    src/tcpserver.cpp
    include/tcpserver.h
    include/dbtxt.h src/dbtxt.cpp
    include/pckg.h
)


#add_library(sqlite3c src/sqlite3.c)
#set_target_properties(sqlite3c PROPERTIES
#    IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/lib/sqlite3c.lib"
#    LINK_FLAGS "/DEF:${CMAKE_SOURCE_DIR}/lib/sqlite3.def"
#)

target_link_libraries(main PRIVATE sfml-graphics wsock32 ws2_32) #fwpuclnt ntdsapi sqlite3c
target_compile_features(main PRIVATE cxx_std_17)

if(WIN32)
    add_custom_command(
        TARGET main
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:main>
        VERBATIM)
endif()

#All server applications must set SO_EXCLUSIVEADDRUSE
