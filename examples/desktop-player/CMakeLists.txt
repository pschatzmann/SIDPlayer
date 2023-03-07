cmake_minimum_required(VERSION 3.20)

# set the project name
project(desktop-player)
set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG}")

include(FetchContent)
option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)


# Build with arduino-audio-tools
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../.. ${CMAKE_CURRENT_BINARY_DIR}/sidplayer )
endif()

# build sketch as executable
set_source_files_properties(desktop-player.ino PROPERTIES LANGUAGE CXX)
add_executable (desktop-player desktop-player.ino)

# set preprocessor defines
target_compile_definitions(desktop-player PUBLIC -DEXIT_ON_STOP -DIS_DESKTOP_WITH_TIME_ONLY)

# specify libraries
target_link_libraries(desktop-player sidplayer arduino-audio-tools)
