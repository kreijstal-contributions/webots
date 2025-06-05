# CMakeLists.txt for resources/projects/controllers
# Converted from Makefile system to CMake

cmake_minimum_required(VERSION 3.10)
project(webots_resources_projects_controllers)

message(STATUS "Configuring resources/projects/controllers")

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Platform detection
if(WIN32)
    set(WEBOTS_PLATFORM "windows")
    set(WEBOTS_LIB_EXTENSION ".dll")
    set(WEBOTS_EXE_EXTENSION ".exe")
elseif(APPLE)
    set(WEBOTS_PLATFORM "darwin")
    set(WEBOTS_LIB_EXTENSION ".dylib")
    set(WEBOTS_EXE_EXTENSION "")
else()
    set(WEBOTS_PLATFORM "linux")
    set(WEBOTS_LIB_EXTENSION ".so")
    set(WEBOTS_EXE_EXTENSION "")
endif()

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
    set(WEBOTS_LIB_PATH "${WEBOTS_HOME}/Contents/lib")
else()
    set(WEBOTS_INCLUDE_DIR "${WEBOTS_HOME}/include")
    set(WEBOTS_CONTROLLER_LIB_PATH "${WEBOTS_HOME}/lib/controller")
    set(WEBOTS_LIB_PATH "${WEBOTS_HOME}/lib")
endif()

# Add include directories
include_directories("${WEBOTS_INCLUDE_DIR}/controller/c")

# Link directories
link_directories("${WEBOTS_CONTROLLER_LIB_PATH}")

# Compiler flags based on build type
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-ggdb -Wall)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O3 -DNDEBUG -Wall)
    if(NOT APPLE)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s")
    endif()
else()
    # Default flags
    add_compile_options(-Wall)
endif()

# Platform-specific compiler flags
if(WIN32)
    add_compile_definitions(_GLIBCXX_USE_CXX11_ABI=1)
    add_compile_options(-mwindows)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-subsystem,windows")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--enable-auto-import")
elseif(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "12.0")
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -target arm64-apple-macos12")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -target arm64-apple-macos12")
    else()
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -target x86_64-apple-macos12")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -target x86_64-apple-macos12")
    endif()
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mmacosx-version-min=12.0")
elseif(UNIX)
    # Linux specific flags
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

# Function to create a Webots controller
function(add_webots_controller CONTROLLER_NAME)
    # Find source files
    file(GLOB CONTROLLER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/${CONTROLLER_NAME}/*.c")
    file(GLOB CONTROLLER_CXX_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/${CONTROLLER_NAME}/*.cpp")
    
    if(NOT CONTROLLER_SOURCES AND NOT CONTROLLER_CXX_SOURCES)
        message(WARNING "No source files found for controller: ${CONTROLLER_NAME}")
        return()
    endif()
    
    # Create the executable
    add_executable(${CONTROLLER_NAME} ${CONTROLLER_SOURCES} ${CONTROLLER_CXX_SOURCES})
    
    # Set target properties
    set_target_properties(${CONTROLLER_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${CONTROLLER_NAME}"
        OUTPUT_NAME "${CONTROLLER_NAME}${WEBOTS_EXE_EXTENSION}"
    )
    
    # Link against Webots Controller library
    target_link_libraries(${CONTROLLER_NAME} Controller)
    
    # Platform-specific linking
    if(UNIX AND NOT APPLE)
        target_link_libraries(${CONTROLLER_NAME} m)  # Math library
    endif()
    
    # Add include directory for this controller
    target_include_directories(${CONTROLLER_NAME} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/${CONTROLLER_NAME}"
        "${WEBOTS_INCLUDE_DIR}/controller/c"
    )
    
    message(STATUS "Added Webots controller: ${CONTROLLER_NAME}")
endfunction()

# Discover and build all controller subdirectories
file(GLOB CONTROLLER_DIRS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/*/")

foreach(CONTROLLER_DIR ${CONTROLLER_DIRS})
    # Remove trailing slash
    string(REGEX REPLACE "/$" "" CONTROLLER_NAME "${CONTROLLER_DIR}")
    
    # Skip directories that are not controllers (like build directories)
    if(NOT "${CONTROLLER_NAME}" MATCHES "^(build|CMakeFiles)$")
        # Check if directory contains source files
        file(GLOB CONTROLLER_C_FILES "${CMAKE_CURRENT_SOURCE_DIR}/${CONTROLLER_NAME}/*.c")
        file(GLOB CONTROLLER_CPP_FILES "${CMAKE_CURRENT_SOURCE_DIR}/${CONTROLLER_NAME}/*.cpp")
        
        if(CONTROLLER_C_FILES OR CONTROLLER_CPP_FILES)
            add_webots_controller("${CONTROLLER_NAME}")
        endif()
    endif()
endforeach()

# Create a custom target to clean all controller binaries
add_custom_target(clean-controllers
    COMMAND ${CMAKE_COMMAND} -E echo "Cleaning controller binaries..."
    COMMAND ${CMAKE_COMMAND} -E remove -f "${CMAKE_CURRENT_SOURCE_DIR}/generic/generic${WEBOTS_EXE_EXTENSION}"
    COMMENT "Removing controller binaries"
)

# Installation rules (optional)
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${WEBOTS_HOME}" CACHE PATH "Install path prefix" FORCE)
endif()

# Install controllers to appropriate location
foreach(CONTROLLER_DIR ${CONTROLLER_DIRS})
    string(REGEX REPLACE "/$" "" CONTROLLER_NAME "${CONTROLLER_DIR}")
    
    if(NOT "${CONTROLLER_NAME}" MATCHES "^(build|CMakeFiles)$")
        file(GLOB CONTROLLER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/${CONTROLLER_NAME}/*.c"
                                   "${CMAKE_CURRENT_SOURCE_DIR}/${CONTROLLER_NAME}/*.cpp")
        if(CONTROLLER_FILES)
            install(TARGETS ${CONTROLLER_NAME}
                    RUNTIME DESTINATION "resources/projects/controllers/${CONTROLLER_NAME}")
        endif()
    endif()
endforeach()

message(STATUS "Configured ${CMAKE_PROJECT_NAME} with discovered controllers")