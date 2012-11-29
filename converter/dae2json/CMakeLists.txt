cmake_minimum_required(VERSION 2.6)
project(COLLADA2JSON)

set(WITH_IN_SOURCE_BUILD "ON")
set(COLLADA2JSON_BINARY_DIR, COLLADA2JSON_SOURCE_DIR)

include_directories(${COLLADA2JSON_SOURCE_DIR}/JSONExport)
include_directories(${COLLADA2JSON_SOURCE_DIR}/shaders)
include_directories(${COLLADA2JSON_SOURCE_DIR}/dependencies/rapidjson/include/rapidjson)
include_directories(${COLLADA2JSON_SOURCE_DIR}/dependencies/OpenCOLLADA/COLLADAFramework/include)
include_directories(${COLLADA2JSON_SOURCE_DIR}/dependencies/OpenCOLLADA/COLLADABaseUtils/include)
include_directories(${COLLADA2JSON_SOURCE_DIR}/dependencies/OpenCOLLADA/COLLADASaxFrameworkLoader/include)

link_directories(${COLLADA2JSON_BINARY_DIR}/lib)

if (WIN32)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-DWIN32)
    add_definitions(-EHsc)
endif()

add_subdirectory(dependencies/OpenCOLLADA)

add_executable(collada2json main.cpp 
    DAE2JSONWriter.cpp
    JSONExport/JSONAccessor.cpp
    JSONExport/JSONArray.cpp
    JSONExport/JSONBuffer.cpp
    JSONExport/JSONEffect.cpp
    JSONExport/JSONIndices.cpp
    JSONExport/JSONMesh.cpp
    JSONExport/JSONNumber.cpp
    JSONExport/JSONObject.cpp
    JSONExport/JSONPrimitive.cpp
    JSONExport/JSONString.cpp
    JSONExport/JSONUtils.cpp
    JSONExport/JSONValue.cpp
    JSONExport/JSONWriter.cpp
    DAE2JSONWriter.h
    JSONExport/JSONExport.h
    JSONExport/JSONExportTypesAndConstants.h
    JSONExport/JSONAccessor.h
    JSONExport/JSONArray.h
    JSONExport/JSONBuffer.h
    JSONExport/JSONEffect.h
    JSONExport/JSONIndices.h
    JSONExport/JSONMesh.h
    JSONExport/JSONNumber.h
    JSONExport/JSONObject.h
    JSONExport/JSONPrimitive.h
    JSONExport/JSONString.h
    JSONExport/JSONUtils.h
    JSONExport/JSONValue.h
    JSONExport/JSONWriter.h
    shaders/commonProfileShaders.h
    shaders/commonProfileShaders.cpp
    helpers/geometryHelpers.h
    helpers/geometryHelpers.cpp)

target_link_libraries (collada2json GeneratedSaxParser_static OpenCOLLADABaseUtils_static UTF_static ftoa_static MathMLSolver_static OpenCOLLADASaxFrameworkLoader_static OpenCOLLADAFramework_static buffer_static $(LIBXML2_LIBRARIES))
