# Project
cmake_minimum_required(VERSION 3.6)
project(KeyHook)
include(utils.cmake)

# Flags
set(CMAKE_CXX_STANDARD 14)
append_default_cxx_flags(CMAKE_CXX_FLAGS)

# Source files
find_sources(SOURCES Source/)
watch_source_change("${SOURCES}" "sources.cmake")

# External files
set(LIBS)
## Testing
include_directories(Libs/Catch)
## Windows keyboard/mouse reading
if (WIN32)
    include_directories(Libs/Interception)
    link_directories(Libs/Interception/x64)
    append(LIBS interception)
    append(COPY_TO_BIN Libs/Interception/x64/interception.dll)
endif ()

# Build binaries
add_executable(KeyHook ${SOURCES})
target_link_libraries(KeyHook ${LIBS})

# Post build
copy_to_bin_on_build("${COPY_TO_BIN}" KeyHook)
