if("${GCC_TOOLCHAIN_PATH}" STREQUAL "")
    message(FATAL_ERROR "GCC_TOOLCHAIN_PATH not set")
endif()

set(CMAKE_SYSTEM_NAME "Cortex-M0plus")
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)

find_program(CMAKE_C_COMPILER NAMES arm-none-eabi-gcc PATHS ${GCC_TOOLCHAIN_PATH}/bin/)
find_program(CMAKE_CXX_COMPILER NAMES arm-none-eabi-g++ PATHS ${GCC_TOOLCHAIN_PATH}/bin/)
find_program(CMAKE_ASM_COMPILER NAMES arm-none-eabi-gcc PATHS ${GCC_TOOLCHAIN_PATH}/bin/)
find_program(CMAKE_OBJCOPY NAMES arm-none-eabi-objcopy PATHS ${GCC_TOOLCHAIN_PATH}/bin/)
find_program(CMAKE_SIZE NAMES arm-none-eabi-size PATHS ${GCC_TOOLCHAIN_PATH}/bin/)
find_program(CMAKE_AR NAMES arm-none-eabi-ar PATHS ${GCC_TOOLCHAIN_PATH}/bin/)

set(CMAKE_C_COMPILER_ID GNU)
set(CMAKE_CXX_COMPILER_ID GNU)

set(CMAKE_EXECUTABLE_SUFFIX_C   .elf)
set(CMAKE_EXECUTABLE_SUFFIX_CXX .elf)
set(CMAKE_EXECUTABLE_SUFFIX_ASM .elf)

SET(ARCH_FLAGS "-v")

SET(CMAKE_C_FLAGS "${ARCH_FLAGS}" CACHE STRING "Common flags for C compiler")
SET(CMAKE_ASM_FLAGS "${ARCH_FLAGS}" CACHE STRING "Common flags for assembler")
SET(CMAKE_CXX_FLAGS "${ARCH_FLAGS}" CACHE STRING "Common flags for C++ compiler")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --specs=nosys.specs")