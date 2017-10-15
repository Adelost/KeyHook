# Project
cmake_minimum_required(VERSION 3.6)
project(KeyHook)

# Flags
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-static-libgcc -static-libstdc++")
if (NOT WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif ()

# Source files
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

# Definitions
if (WIN32)
    add_definitions(-D_WIN32_)
elseif (APPLE)
    add_definitions(-D_APPLE_)
elseif (UNIX AND NOT APPLE)
    add_definitions(-D_LINUX_)
endif ()

# Libraries
if (WIN32)
    include_directories(Libs/Interception)
    link_directories(Libs/Interception/x64)
    list(APPEND LIBS interception)
    list(APPEND COPY_TO_BIN Libs/Interception/x64/interception.dll)
endif ()

# Build
add_executable(KeyHook ${SOURCES})
target_link_libraries(KeyHook ${LIBS})

# Post build
foreach (file ${COPY_TO_BIN})
    message(${PROJECT_SOURCE_DIR}/${file})
    add_custom_command(TARGET KeyHook POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${PROJECT_SOURCE_DIR}/${file}"
            $<TARGET_FILE_DIR:KeyHook>)
endforeach (file)
