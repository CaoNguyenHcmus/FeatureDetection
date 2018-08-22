# Minimum required to read this makefile
cmake_minimum_required(VERSION 2.8)

# Add flags C++14 standard, warning errors
add_compile_options(-std=c++14 -Wall -Werror -Wextra -pedantic)
set(EXECUTABLE_OUTPUT_PATH "../bin")
set(LIBRARY_OUTPUT_PATH "../libs")
set(ARCHIVE_OUTPUT_PATH "../archive")

message("CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")
if (CMAKE_BUILD_TYPE MATCHES "Debug")
    message("Debug mode")
    set(EXECUTABLE_OUTPUT_PATH "../bin/Debug")
elseif (CMAKE_BUILD_TYPE MATCHES "Release")
    message("Release mode")
    set(EXECUTABLE_OUTPUT_PATH "../bin/Release")
endif ()

# First for the generic no-config case (e.g. with mingw)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_PATH})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${ARCHIVE_OUTPUT_PATH})
# Second, for multi-config builds (e.g. msvc)
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${EXECUTABLE_OUTPUT_PATH})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${LIBRARY_OUTPUT_PATH})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${ARCHIVE_OUTPUT_PATH})
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

# Project name
project(matching)

# Bring needed packages
find_package(OpenCV REQUIRED)

message(STATUS "")
message(STATUS "General configuration for ${PROJECT_NAME} =====================================")
message(STATUS "")
message(STATUS "    OpenCV path:                ${OpenCV_DIR}")
message(STATUS "    OpenCV version:             ${OpenCV_VERSION}")
message(STATUS "    Compiler:                   ${CMAKE_CXX_COMPILER}")
message(STATUS "    C++ flags (Release):        ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}")
message(STATUS "    C++ flags (Debug):          ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}")

# Bring all header files from "include" directory
include_directories(include)

# However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "src/*.cpp")

# Build all file from "source" directory
add_executable(${PROJECT_NAME} ${SOURCES})

# Link to OpenCV libraries
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
