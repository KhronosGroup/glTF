cmake_minimum_required(VERSION 2.8)
project(COLLADA2GLTF)

if (NOT WIN32)
#http://www.guyrutenberg.com/2014/01/05/enabling-c11-c0x-in-cmake/
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    message("-- C++11 Enabled")
elseif(COMPILER_SUPPORTS_CXX0X)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
    message("-- C++0x Enabled")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()
endif()

set(USE_OPEN3DGC "ON")
set(WITH_IN_SOURCE_BUILD "ON")
set(COLLADA2GLTF_BINARY_DIR, COLLADA2GLTF_SOURCE_DIR)
set(BUILD_SHARED_LIBS "OFF")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(GetGitRevisionDescription)
get_git_head_revision(GIT_REFSPEC GIT_SHA1)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/GitSHA1.cpp.in" "${CMAKE_CURRENT_BINARY_DIR}/GitSHA1.cpp" @ONLY)

set(TARGET_LIBS GeneratedSaxParser_static OpenCOLLADABaseUtils_static UTF_static ftoa_static MathMLSolver_static OpenCOLLADASaxFrameworkLoader_static OpenCOLLADAFramework_static buffer_static)

if (NOT WIN32)
    set(CMAKE_FIND_LIBRARY_SUFFIXES .so .a .dylib)
endif()

# Lets libxml2 work in a shared library
add_definitions(-DLIBXML_STATIC_FOR_DLL)

include_directories(${COLLADA2GLTF_SOURCE_DIR})
include_directories(${COLLADA2GLTF_SOURCE_DIR}/GLTF)
include_directories(${COLLADA2GLTF_SOURCE_DIR}/JSON)
include_directories(${COLLADA2GLTF_SOURCE_DIR}/shaders)
include_directories(${COLLADA2GLTF_SOURCE_DIR}/helpers)
include_directories(${COLLADA2GLTF_SOURCE_DIR}/convert)
include_directories(${COLLADA2GLTF_SOURCE_DIR}/assetModifiers)
include_directories(${COLLADA2GLTF_SOURCE_DIR}/dependencies/json/include/rapidjson)
include_directories(${COLLADA2GLTF_SOURCE_DIR}/dependencies/OpenCOLLADA/COLLADAFramework/include)
include_directories(${COLLADA2GLTF_SOURCE_DIR}/dependencies/OpenCOLLADA/COLLADABaseUtils/include)
include_directories(${COLLADA2GLTF_SOURCE_DIR}/dependencies/OpenCOLLADA/COLLADASaxFrameworkLoader/include)
include_directories(${COLLADA2GLTF_SOURCE_DIR}/dependencies/OpenCOLLADA/GeneratedSaxParser/include)
if (WIN32)
    include_directories(${COLLADA2GLTF_SOURCE_DIR}/dependencies/misc)
endif()

if (USE_OPEN3DGC)
    add_definitions( -DUSE_OPEN3DGC )
    include_directories(${COLLADA2GLTF_SOURCE_DIR}/extensions/o3dgc-compression)

    include_directories(${COLLADA2GLTF_SOURCE_DIR}/dependencies/o3dgc/src)
    include_directories(${COLLADA2GLTF_SOURCE_DIR}/dependencies/o3dgc/src/o3dgc_common_lib/inc)
    include_directories(${COLLADA2GLTF_SOURCE_DIR}/dependencies/o3dgc/src/o3dgc_encode_lib/inc)
    include_directories(${COLLADA2GLTF_SOURCE_DIR}/dependencies/o3dgc/src/o3dgc_decode_lib/inc)
endif()

if (NOT WIN32)
    find_package(PNG REQUIRED)
    include_directories(${PNG_INCLUDE_DIR})
    find_package(ZLIB REQUIRED)
    include_directories(${ZLIB_INCLUDE_DIR})
endif()

link_directories(${COLLADA2GLTF_BINARY_DIR}/lib)

if (WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-DWIN32)
    add_definitions(-EHsc)
endif()

add_subdirectory(dependencies/OpenCOLLADA)

if (USE_OPEN3DGC)
    add_subdirectory(dependencies/o3dgc/src)
endif()

set(GLTF_SOURCES
    COLLADA2GLTFWriter.h
    COLLADA2GLTFWriter.cpp
    GLTFOpenCOLLADA.h
    GLTFOpenCOLLADAUtils.h
    GLTFOpenCOLLADAUtils.cpp

    GLTF/GLTFAccessorCache.cpp
    GLTF/GLTFAccessorCache.h
    GLTF/GLTFAsset.cpp
    GLTF/GLTFAnimation.cpp
    GLTF/GLTFAccessor.cpp
    GLTF/GLTFBuffer.cpp
    GLTF/GLTFEffect.cpp
    GLTF/GLTFMesh.cpp
    GLTF/GLTFPrimitive.cpp
    GLTF/GLTFUtils.cpp
    GLTF/GLTFWriter.cpp
    GLTF/GLTFSkin.cpp
    GLTF/GLTF.h
    GLTF/GLTFTypesAndConstants.h
    GLTF/GLTFAsset.h
    GLTF/GLTFAnimation.h
    GLTF/GLTFAccessor.h
    GLTF/GLTFBuffer.h
    GLTF/GLTFEffect.h
    GLTF/GLTFMesh.h
    GLTF/GLTFPrimitive.h
    GLTF/GLTFUtils.h
    GLTF/GLTFWriter.h
    GLTF/GLTFSkin.h
    GLTF/GLTFExtraDataHandler.h
    GLTF/GLTFExtraDataHandler.cpp
    GLTF/GLTFProfile.h
    GLTF/GLTFProfile.cpp
    GLTF/GLTFInputStream.h
    GLTF/GLTFInputStream.cpp
    GLTF/GLTFOutputStream.h
    GLTF/GLTFOutputStream.cpp
    GLTF/GLTFConfig.cpp
    GLTF/GLTFConfig.h

    profiles/webgl-1.0/webgl-idl.h
    profiles/webgl-1.0/GLTFWebGL_1_0_Profile.cpp
    profiles/webgl-1.0/GLTFWebGL_1_0_Profile.h

    shaders/commonProfileShaders.h
    shaders/commonProfileShaders.cpp

    helpers/geometryHelpers.h
    helpers/geometryHelpers.cpp
    helpers/mathHelpers.h
    helpers/mathHelpers.cpp

    convert/meshConverter.cpp
    convert/meshConverter.h
    convert/animationConverter.cpp
    convert/animationConverter.h

    JSON/JSONArray.cpp
    JSON/JSONNumber.cpp
    JSON/JSONObject.cpp
    JSON/JSONString.cpp
    JSON/JSONValue.cpp
    JSON/JSONArray.h
    JSON/JSONNumber.h
    JSON/JSONObject.h
    JSON/JSONString.h
    JSON/JSONValue.h
    
    assetModifiers/GLTFAssetModifier.h
    assetModifiers/GLTFFlipUVModifier.h
    assetModifiers/GLTFFlipUVModifier.cpp

    ${CMAKE_CURRENT_BINARY_DIR}/GitSHA1.cpp
    GitSHA1.h)

if (USE_OPEN3DGC)
LIST(APPEND GLTF_SOURCES
    extensions/o3dgc-compression/GLTF-Open3DGC.cpp
    extensions/o3dgc-compression/GLTF-Open3DGC.h)
endif()

add_library(collada2gltfConvert SHARED ${GLTF_SOURCES})

#Make sure the dll is in the same directory as the executable
if (WIN32)
    set_target_properties(collada2gltfConvert PROPERTIES RUNTIME_OUTPUT_DIRECTORY "bin")
endif()

if (NOT WIN32)
LIST(APPEND TARGET_LIBS ${PNG_LIBRARY} ${ZLIB_LIBRARY})
endif()

if (USE_OPEN3DGC)
LIST(APPEND TARGET_LIBS o3dgc_common_lib o3dgc_enc_lib o3dgc_dec_lib)
endif()

IF("${CMAKE_SYSTEM}" MATCHES "Linux")
    LIST(APPEND TARGET_LIBS rt)
endif("${CMAKE_SYSTEM}" MATCHES "Linux")

target_link_libraries (collada2gltfConvert ${TARGET_LIBS})

set(GLTF_EXE_SOURCES  main.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/GitSHA1.cpp
    GitSHA1.h)

if (WIN32)
LIST(APPEND GLTF_EXE_SOURCES ${COLLADA2GLTF_SOURCE_DIR}/dependencies/misc/getopt_long.c ${COLLADA2GLTF_SOURCE_DIR}/dependencies/misc/getopt.c ${COLLADA2GLTF_SOURCE_DIR}/dependencies/misc/getopt.h)
endif()

add_executable(collada2gltf ${GLTF_EXE_SOURCES})
target_link_libraries (collada2gltf collada2gltfConvert)
