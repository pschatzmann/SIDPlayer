cmake_minimum_required(VERSION 3.16)

# set the project name
project(sidplayer)

# lots of warnings and all warnings as errors
# add_compile_options(-Wall -Wextra )

set(CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
include(FetchContent)

# Build with Audio Tool
FetchContent_Declare(arduino-audio-tools GIT_REPOSITORY "https://github.com/pschatzmann/arduino-audio-tools.git" GIT_TAG main )
FetchContent_GetProperties(arduino-audio-tools)
if(NOT arduino-audio-tools_POPULATED)
    FetchContent_Populate(arduino-audio-tools)
    add_subdirectory(${arduino-audio-tools_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/arduino-audio-tools)
endif()

# define source files
file(GLOB_RECURSE SRC_LIST_CPP CONFIGURE_DEPENDS  "${PROJECT_SOURCE_DIR}/src/*.cpp" )
file(GLOB_RECURSE SRC_LIST_C CONFIGURE_DEPENDS  "${PROJECT_SOURCE_DIR}/src/*.c" )

# define libraries
add_library (sidplayer STATIC ${SRC_LIST_CPP} ${SRC_LIST_C})

target_compile_definitions(sidplayer PUBLIC -DIS_DESKTOP )

# define location for header files
target_include_directories(sidplayer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src )

target_link_libraries(sidplayer arduino-audio-tools)
