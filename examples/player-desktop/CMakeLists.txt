cmake_minimum_required(VERSION 3.20)

# set the project name
project(player-desktop)
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
set_source_files_properties(player-desktop.ino PROPERTIES LANGUAGE CXX)
add_executable (player-desktop player-desktop.ino)

# set preprocessor defines
target_compile_definitions(arduino_emulator PUBLIC -DDEFINE_MAIN)
target_compile_definitions(player-desktop PUBLIC -DARDUINO -DIS_DESKTOP -DEXIT_ON_STOP)
target_compile_definitions(sidplayer PUBLIC -DARDUINO -DIS_DESKTOP -DEXIT_ON_STOP)

# specify libraries
target_link_libraries(player-desktop sidplayer arduino_emulator arduino-audio-tools)
