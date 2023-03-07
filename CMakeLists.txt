cmake_minimum_required(VERSION 3.16)

# set the project name
project(sidplayer)

# lots of warnings and all warnings as errors
# add_compile_options(-Wall -Wextra )

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG}")
include(FetchContent)


# Build with Audio Tool
set (ADD_STD_LIB OFF CACHE INTERNAL  "Turn off extended libs")
set (ADD_PORTAUDIO OFF CACHE INTERNAL  "Portraudio lib")
set (ADD_ARDUINO_EMULATOR OFF CACHE INTERNAL  "Arduino Emulator lib")
FetchContent_Declare(arduino-audio-tools GIT_REPOSITORY "https://github.com/pschatzmann/arduino-audio-tools.git" GIT_TAG main )
FetchContent_GetProperties(arduino-audio-tools)
if(NOT arduino-audio-tools_POPULATED)
    FetchContent_Populate(arduino-audio-tools)
    add_subdirectory(${arduino-audio-tools_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/arduino-audio-tools)
endif()

# define source files
file(GLOB_RECURSE SRC_LIST_CPP CONFIGURE_DEPENDS  "${PROJECT_SOURCE_DIR}/src/*.cpp" )

# define libraries
add_library (sidplayer STATIC ${SRC_LIST_CPP})

target_compile_definitions(sidplayer PUBLIC -DIS_DESKTOP_WITH_TIME_ONLY -DEXIT_ON_STOP )
target_compile_definitions(arduino-audio-tools INTERFACE -DIS_DESKTOP_WITH_TIME_ONLY -DEXIT_ON_STOP)

# define location for header files
target_include_directories(sidplayer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src )

target_link_libraries(sidplayer arduino-audio-tools)
