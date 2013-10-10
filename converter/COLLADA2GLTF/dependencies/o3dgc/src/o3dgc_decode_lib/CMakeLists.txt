project(O3DGC_DEC_LIB CXX C)
include(${CMAKE_COMMON_INC})

set(LIB_TYPE "STATIC" CACHE STRING "STATIC, SHARED or MODULE?")
message("[o3dgc] \t LIB_TYPE " ${LIB_TYPE})
#set(CMAKE_CXX_FLAGS "-g -Wall")
add_library(o3dgc_dec_lib ${PROJECT_CPP_FILES} ${PROJECT_C_FILES} ${PROJECT_INC_FILES} ${PROJECT_INL_FILES})
include_directories("${${PROJECT_NAME}_SOURCE_DIR}/../o3dgc_common_lib/inc")

