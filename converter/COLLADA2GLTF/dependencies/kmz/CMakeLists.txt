
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	if(NOT DEFINED WITH_IN_SOURCE_BUILD)
		message(FATAL_ERROR
			"CMake generation for Kmz2gltf is not allowed within the source directory!"
			"\n Remove the CMakeCache.txt file and try again from another folder, e.g.:"
			"\n "
			"\n rm CMakeCache.txt"
			"\n cd .."
			"\n mkdir cmake-make"
			"\n cd cmake-make"
			"\n "
		)
	endif()
endif()
cmake_minimum_required(VERSION 2.6)
LINK_DIRECTORIES(${COLLADA2GLTF_BINARY_DIR}/zlib)