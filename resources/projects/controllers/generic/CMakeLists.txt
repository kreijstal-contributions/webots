# CMakeLists.txt for generic controller
# This is an example of a controller-specific CMakeLists.txt

cmake_minimum_required(VERSION 3.10)
project(generic_controller)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Find Webots installation
if(DEFINED ENV{WEBOTS_HOME})
    set(WEBOTS_HOME $ENV{WEBOTS_HOME})
else()
    message(FATAL_ERROR "WEBOTS_HOME environment variable is not defined")
endif()

# Set Webots paths based on platform
if(APPLE)
    set(WEBOTS_INCLUDE_DIR "${WEBOTS_HOME}/Contents/include")
    set(WEBOTS_CONTROLLER_LIB_PATH "${WEBOTS_HOME}/Contents/lib/controller")
else()
    set(WEBOTS_INCLUDE_DIR "${WEBOTS_HOME}/include")
    set(WEBOTS_CONTROLLER_LIB_PATH "${WEBOTS_HOME}/lib/controller")
endif()

# Platform detection for executable extension
if(WIN32)
    set(EXE_EXTENSION ".exe")
else()
    set(EXE_EXTENSION "")
endif()

# Add include directories
include_directories("${WEBOTS_INCLUDE_DIR}/controller/c")

# Link directories
link_directories("${WEBOTS_CONTROLLER_LIB_PATH}")

# Compiler flags
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-ggdb -Wall)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O3 -DNDEBUG -Wall)
    if(NOT APPLE)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s")
    endif()
else()
    add_compile_options(-Wall)
endif()

# Platform-specific flags
if(WIN32)
    add_compile_definitions(_GLIBCXX_USE_CXX11_ABI=1)
    add_compile_options(-mwindows)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-subsystem,windows")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--enable-auto-import")
elseif(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "12.0")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mmacosx-version-min=12.0")
endif()

# Create the generic controller executable
add_executable(generic generic.c)

# Set output name and directory
set_target_properties(generic PROPERTIES
    OUTPUT_NAME "generic${EXE_EXTENSION}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

# Link against Webots Controller library
target_link_libraries(generic Controller)

# Platform-specific linking
if(UNIX AND NOT APPLE)
    target_link_libraries(generic m)  # Math library on Linux
endif()

# Custom target to clean the binary
add_custom_target(clean-generic
    COMMAND ${CMAKE_COMMAND} -E remove -f "${CMAKE_CURRENT_SOURCE_DIR}/generic${EXE_EXTENSION}"
    COMMENT "Removing generic controller binary"
)

message(STATUS "Configured generic controller")