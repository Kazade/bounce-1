# Bounce CMake script
# Author: Irlan Robson
# https://cmake.org/cmake

cmake_minimum_required(VERSION 3.8)

project(bounce VERSION 0.0.0)

set(CMAKE_CONFIGURATION_TYPES "Debug;RelWithDebInfo" CACHE STRING "" FORCE)

set(BOUNCE_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(BOUNCE_SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(BOUNCE_EXAMPLES_DIR "${CMAKE_SOURCE_DIR}/examples")

add_subdirectory(src)

option(BOUNCE_BUILD_EXAMPLES "Build the Bounce examples" ON)
option(BOUNCE_BUILD_DOCS "Build the Bounce documentation" OFF)
option(BOUNCE_USER_SETTINGS "Override Bounce settings with user_settings.h" OFF)

if (BOUNCE_USER_SETTINGS)
	add_compile_definitions(B3_USER_SETTINGS)
endif()

if (BOUNCE_BUILD_DOCS)
 	set(DOXYGEN_SKIP_DOT TRUE)
    	find_package(Doxygen)

	if (DOXYGEN_FOUND)
	    add_subdirectory(doc)
	endif()

endif()

if (BOUNCE_BUILD_EXAMPLES)
	add_subdirectory(examples/hello_world)
	add_subdirectory(external/glad)
	add_subdirectory(external/glfw)
	add_subdirectory(external/imgui)
	add_subdirectory(examples/testbed)

	# default startup project for Visual Studio
	if (MSVC)
		set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT testbed)
		set_property(TARGET testbed PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/examples/testbed")
	endif()
endif()
