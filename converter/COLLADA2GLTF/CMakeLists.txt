cmake_minimum_required(VERSION 2.8)
project(COLLADA2GLTF)

set(USE_OPEN3DGC "ON")
set(USE_WEBGLLOADER "OFF")
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

include_directories(${COLLADA2GLTF_SOURCE_DIR}/GLTF)
include_directories(${COLLADA2GLTF_SOURCE_DIR}/shaders)
include_directories(${COLLADA2GLTF_SOURCE_DIR}/helpers)
include_directories(${COLLADA2GLTF_SOURCE_DIR}/convert)
include_directories(${COLLADA2GLTF_SOURCE_DIR}/dependencies/json/include/rapidjson)
include_directories(${COLLADA2GLTF_SOURCE_DIR}/dependencies/OpenCOLLADA/COLLADAFramework/include)
include_directories(${COLLADA2GLTF_SOURCE_DIR}/dependencies/OpenCOLLADA/COLLADABaseUtils/include)
include_directories(${COLLADA2GLTF_SOURCE_DIR}/dependencies/OpenCOLLADA/COLLADASaxFrameworkLoader/include)
include_directories(${COLLADA2GLTF_SOURCE_DIR}/dependencies/OpenCOLLADA/GeneratedSaxParser/include)
if (WIN32)
    include_directories(${COLLADA2GLTF_SOURCE_DIR}/dependencies/misc)
endif()

if (USE_WEBGLLOADER)
    add_definitions( -DUSE_WEBGLLOADER )
    include_directories(${COLLADA2GLTF_SOURCE_DIR}/dependencies/webgl-loader/src)
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

if (USE_WEBGLLOADER)
    add_subdirectory(dependencies/webgl-loader)
endif()

if (USE_OPEN3DGC)
    add_subdirectory(dependencies/o3dgc/src)
endif()

set(GLTF_SOURCES main.cpp 
    COLLADA2GLTFWriter.cpp
    GLTFConverterContext.cpp
    GLTF/JSONArray.cpp
    GLTF/JSONNumber.cpp
    GLTF/JSONObject.cpp
    GLTF/JSONString.cpp
    GLTF/JSONValue.cpp
    GLTF/GLTFAnimation.cpp
    GLTF/GLTFMeshAttribute.cpp
    GLTF/GLTFBuffer.cpp
    GLTF/GLTFEffect.cpp
    GLTF/GLTFIndices.cpp
    GLTF/GLTFMesh.cpp
    GLTF/GLTFPrimitive.cpp
    GLTF/GLTFUtils.cpp
    GLTF/GLTFWriter.cpp
    GLTF/GLTFSkin.cpp
    COLLADA2GLTFWriter.h
    GLTF-OpenCOLLADA.h
    GLTF/GLTF.h
    GLTF/GLTFTypesAndConstants.h
    GLTFConverterContext.h
    GLTF/JSONArray.h
    GLTF/JSONNumber.h
    GLTF/JSONObject.h
    GLTF/JSONString.h
    GLTF/JSONValue.h
    GLTF/GLTFAnimation.h
    GLTF/GLTFMeshAttribute.h
    GLTF/GLTFBuffer.h
    GLTF/GLTFEffect.h
    GLTF/GLTFIndices.h
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
    ${CMAKE_CURRENT_BINARY_DIR}/GitSHA1.cpp
    GitSHA1.h)

if (USE_OPEN3DGC)
LIST(APPEND GLTF_SOURCES
    extensions/o3dgc-compression/GLTF-Open3DGC.cpp
    extensions/o3dgc-compression/GLTF-Open3DGC.h)
endif()

if (WIN32)
LIST(APPEND GLTF_SOURCES ${COLLADA2GLTF_SOURCE_DIR}/dependencies/misc/getopt_long.c ${COLLADA2GLTF_SOURCE_DIR}/dependencies/misc/getopt.c ${COLLADA2GLTF_SOURCE_DIR}/dependencies/misc/getopt.h)
endif()

add_executable(collada2gltf ${GLTF_SOURCES})

if (NOT WIN32)
LIST(APPEND TARGET_LIBS ${PNG_LIBRARY} ${ZLIB_LIBRARY})
endif()

if (USE_OPEN3DGC)
LIST(APPEND TARGET_LIBS o3dgc_common_lib o3dgc_enc_lib o3dgc_dec_lib)
endif()

if (USE_WEBGLLOADER)
LIST(APPEND TARGET_LIBS WEBGLLOADER)
endif()


target_link_libraries (collada2gltf ${TARGET_LIBS})

