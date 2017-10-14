cmake_minimum_required(VERSION 3.6)
project(KeyHook)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-static-libgcc -static-libstdc++")

if (NOT WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif ()

set(SOURCES
        Source/main.cpp
        Source/EnumClassHash.h
        Source/MouseKeyMapper.h
        Source/MouseKeyMapper.cpp
        Source/OSWrap.h
        Source/OSWrap.cpp
        Source/Keys.h
        Source/Keys.cpp
        Source/KeyHook.h
        Source/KeyHook.cpp
        Source/ScanCodes.h
        Source/ScanCodes.cpp
        )

include_directories(Libs/Interception)
link_directories(Libs/Interception/x64)

if (WIN32)
    add_definitions(-D_WIN32_)
    set(LIBS interception.lib)
elseif(APPLE)
    add_definitions(-D_APPLE_)
elseif(UNIX AND NOT APPLE)
    add_definitions(-D_LINUX_)
endif ()

add_executable(KeyHook ${SOURCES})
target_link_libraries(KeyHook ${LIBS})