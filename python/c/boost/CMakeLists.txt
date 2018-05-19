project(hello)

cmake_minimum_required(VERSION 2.6)

option(BUILD_SHARED_LIBS "Build shared instead of static library" ON)
option(BUILD_TESTS "Build tests" OFF)
option(BUILD_PKGCONFIG "Build in PKGCONFIG mode" OFF)

set(CMAKE_DEBUG_POSTFIX _d)
set(BUILD_DEFINES "" CACHE STRING "Build defines")

if(MSVC)
	option(STATIC_CRT "Use static CRT libraries" OFF)

	# Rewrite command line flags to use /MT if necessary
	if(STATIC_CRT)
		foreach(flag_var
				CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
				CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
			if(${flag_var} MATCHES "/MD")
				string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
			endif(${flag_var} MATCHES "/MD")
		endforeach(flag_var)
	endif()
endif()

# Pre-defines standard install locations on *nix systems.
include(GNUInstallDirs)
mark_as_advanced(CLEAR CMAKE_INSTALL_LIBDIR CMAKE_INSTALL_INCLUDEDIR)

set(HEADERS )
set(SOURCES ${HEADERS} hello.cpp)

if(DEFINED BUILD_DEFINES)
	foreach(DEFINE ${BUILD_DEFINES})
		add_definitions("-D" ${DEFINE})
	endforeach()
endif()



if(BUILD_SHARED_LIBS)
	add_library(hello SHARED ${SOURCES})
else()
	add_library(hello STATIC ${SOURCES})
endif()

set (EXTRA_LIBS_DEBUG ${EXTRA_LIBS_DEBUG} "python36_d.lib" "python3_d.lib")
set (EXTRA_LIBS_RLS ${EXTRA_LIBS_RLS} "python36.lib" "python3.lib")
target_link_libraries (hello debug ${EXTRA_LIBS_DEBUG} optimized ${EXTRA_LIBS_RLS} )

SET_TARGET_PROPERTIES(hello   PROPERTIES SUFFIX .pyd)

# Enable C++11 long long for compilers that are capable of it
if(NOT ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} STRLESS 3.1 AND ";${CMAKE_CXX_COMPILE_FEATURES};" MATCHES ";cxx_long_long_type;")
	target_compile_features(hello PUBLIC cxx_long_long_type)
endif()

set_target_properties(hello PROPERTIES VERSION 1.7 SOVERSION 1)
get_target_property(hello_VERSION_STRING hello VERSION)


