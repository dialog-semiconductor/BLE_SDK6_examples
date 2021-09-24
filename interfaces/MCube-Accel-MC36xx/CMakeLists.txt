cmake_minimum_required(VERSION 3.16)

project(${DEVICE_NAME} LANGUAGES C CXX ASM)

message("DEVICE_NAME = ${DEVICE_NAME}")
message("DIALOG_SDK_PATH = ${DIALOG_SDK_PATH}")
message("DIALOG_EXAMPLE_PATH = ${DIALOG_EXAMPLE_PATH}")
message("GCC_TOOLCHAIN_PATH = ${GCC_TOOLCHAIN_PATH}")

set(CMAKE_VERBOSE_MAKEFILE OFF)

include_directories(src/user_app
                    src/user_drivers/platform/console
                    src/user_drivers/platform/bus
                    src/user_drivers/sensor/accel/mc36xx
                    src/user_drivers/sensor/accel 
)

include (${DIALOG_EXAMPLE_PATH}/build_utils/gcc/example_build.cmake)

