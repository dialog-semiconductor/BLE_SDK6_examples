cmake_minimum_required(VERSION 3.16)

project(${DEVICE_NAME} LANGUAGES C CXX ASM)

message("DEVICE_NAME = ${DEVICE_NAME}")
message("DIALOG_SDK_PATH = ${DIALOG_SDK_PATH}")
message("DIALOG_EXAMPLE_PATH = ${DIALOG_EXAMPLE_PATH}")
message("GCC_TOOLCHAIN_PATH = ${GCC_TOOLCHAIN_PATH}")

set(CMAKE_VERBOSE_MAKEFILE OFF)

include (${DIALOG_EXAMPLE_PATH}/build_utils/gcc/example_build.cmake)

include_directories(src/user_app src/user_drivers)

