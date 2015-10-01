project(O3DGC_TEST)
include(${CMAKE_COMMON_INC})
add_executable(test_o3dgc ${PROJECT_CPP_FILES} ${PROJECT_C_FILES} ${PROJECT_INC_FILES} ${PROJECT_INL_FILES})

include_directories("${${PROJECT_NAME}_SOURCE_DIR}/../o3dgc_decode_lib/inc" "${${PROJECT_NAME}_SOURCE_DIR}/../o3dgc_encode_lib/inc" "${${PROJECT_NAME}_SOURCE_DIR}/../o3dgc_common_lib/inc")

target_link_libraries(test_o3dgc o3dgc_dec_lib o3dgc_enc_lib o3dgc_common_lib)
IF(WIN32)
target_link_libraries(test_o3dgc o3dgc_common_lib)
ELSEIF(APPLE)
target_link_libraries(test_o3dgc o3dgc_common_lib)
ELSE ()
set(CMAKE_CXX_FLAGS "-g -Wall")
target_link_libraries(test_o3dgc o3dgc_common_lib rt)
ENDIF()




