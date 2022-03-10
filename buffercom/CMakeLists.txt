cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c++17")

# set(UTIL_INCLUDE "${CMAKE_SOURCE_DIR}/util/benchmark_util/include")
# FILE (GLOB UTIL_SOURCE "${CMAKE_SOURCE_DIR}/util/benchmark_util/lib/*.cpp" )

# set(POSTPROCESS_INCLUDE "${CMAKE_SOURCE_DIR}/postprocess/pose_check/include")
# FILE (GLOB POSTPROCESS_SOURCE "${CMAKE_SOURCE_DIR}/postprocess/pose_check/*.cpp" )

project(buffercom)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# aarch64 arm compiler
# set(CMAKE_C_COMPILER  ${TOOLCHAIN_PATH}/aarch64-linux-gnu-gcc)
# set(CMAKE_CXX_COMPILER  ${TOOLCHAIN_PATH}/aarch64-linux-gnu-g++)

set(BUFFERCOM_INCLUDE "${CMAKE_SOURCE_DIR}/buffercom")
FILE (GLOB BUFFERCOM_SOURCE "${CMAKE_SOURCE_DIR}/buffercom/*.cpp" )

set(DEPS_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../deps/x86)
# set(DEPS_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/deps/aarch64)

message(${DEPS_ROOT})

include_directories(
    ${BUFFERCOM_INCLUDE}
    ${BUFFERCOM_SOURCE}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${DEPS_ROOT}/gflags/include
    ${DEPS_ROOT}/opencv/include
)

link_directories(
    ${DEPS_ROOT}/gflags/lib
    ${DEPS_ROOT}/opencv/lib
)

set(name buffercom_test)

add_executable(
    ${name}
    ${CMAKE_CURRENT_SOURCE_DIR}/buffercom_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/rawbyte.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/buffercom.cpp
)
target_link_libraries(${name} gflags opencv_world zlib dl pthread)


# macro (BC_BUILD name file inference_model_file)
#     add_executable(
#         ${name}
#         ${CMAKE_CURRENT_SOURCE_DIR}/${file}
#         ${CMAKE_CURRENT_SOURCE_DIR}/${inference_model_file}
#         ${UTIL_SOURCE}
#         ${POSTPROCESS_SOURCE}
#         ${VIRTUALIZATION_SOURCE}
#     )
#     if (${PLATFORM} STREQUAL "horizon_x86")
#         target_link_libraries(${name} ${BPU_libs} ${Python3_LIBRARIES} Eigen3::Eigen sqlite3 gflags opencv_world zlib dl pthread)
#     elseif (${PLATFORM} STREQUAL "horizon_arm")
#         target_link_libraries(${name} ${BPU_libs} Eigen3::Eigen gflags opencv_world zlib dl pthread)
#     endif()
# endmacro()


# macro (build_horizon_net name file inference_model_file tnc_file)
#     message("NAME: ${name}")  
#     message("FILE: ${file}")  
#     message("FILE: ${inference_model_file}")

#     add_executable(${name}
#                 ${CMAKE_CURRENT_SOURCE_DIR}/${file}
#                 ${CMAKE_CURRENT_SOURCE_DIR}/${inference_model_file}
#                 ${CMAKE_CURRENT_SOURCE_DIR}/${tnc_file}
#                 ${UTIL_SOURCE}
#                 ${POSTPROCESS_SOURCE}
#                 ${VIRTUALIZATION_SOURCE})
#     if (${PLATFORM} STREQUAL "horizon_x86")
#         target_link_libraries(${name} ${BPU_libs} ${Python3_LIBRARIES} Eigen3::Eigen sqlite3 gflags opencv_world zlib dl pthread)
#     elseif (${PLATFORM} STREQUAL "horizon_arm")
#         target_link_libraries(${name} ${BPU_libs} Eigen3::Eigen gflags opencv_world zlib dl pthread)
#     endif()
# endmacro()

# #pose_keypoints benchmark
# HORIZON_EXAMPLE(run_pose_modular run_hrnet_modular.cpp hrnet_skeleton.cpp)
# HORIZON_EXAMPLE(run_pose_io run_hrnet_io.cpp hrnet_skeleton.cpp)
# HORIZON_EXAMPLE(sender buffercom_test.cpp buffercom.cpp buffercom.h buffercom_test.h)

# # with network lib
# build_horizon_net(horizon_pose_inference horizon_pose_inference.cpp hrnet_skeleton.cpp buffercom.cpp)
# build_horizon_net(horizon_pose_benchmark horizon_pose_benchmark.cpp hrnet_skeleton.cpp buffercom.cpp)

