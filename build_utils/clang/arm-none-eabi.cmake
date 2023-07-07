set(CMAKE_SYSTEM_NAME "generic")
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_CROSSCOMPILING TRUE)
set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)

# Clang target triple
SET(TARGET arm-none-eabi)

SET (CMAKE_C_COMPILER   "clang")
SET (CMAKE_CXX_COMPILER "clang++")
SET (CMAKE_ASM_COMPILER "clang")
SET (CMAKE_AR           "llvm-ar-15")
SET (CMAKE_LINKER       "llvm-link-15")
SET (CMAKE_NM           "llvm-nm-15")
SET (CMAKE_OBJDUMP      "llvm-objdump-15")
SET (CMAKE_OBJCOPY      "llvm-objcopy-15")
SET (CMAKE_RANLIB       "llvm-ranlib-15")
SET (CMAKE_SIZE         "llvm-size-15")


# set(CMAKE_C_COMPILER_ID Clang)
# set(CMAKE_CXX_COMPILER_ID Clang)

set(CMAKE_EXECUTABLE_SUFFIX_C   .elf)
set(CMAKE_EXECUTABLE_SUFFIX_CXX .elf)
set(CMAKE_EXECUTABLE_SUFFIX_ASM .elf)

SET(CMAKE_SYSROOT "/lib/arm-none-eabi")
SET(CMAKE_SYSROOT_COMPILE "/lib/arm-none-eabi")
SET(CMAKE_SYSROOT_LINK "/lib/arm-none-eabi")

SET(CMAKE_C_COMPILER_TARGET "arm-none-eabi")
SET(CMAKE_CXX_COMPILER_TARGET "arm-none-eabi")
SET(CMAKE_ASM_COMPILER_TARGET "arm-none-eabi")

SET(CMAKE_SYSTEM_PROCESSOR "armv6-m")

SET(ARCH_FLAGS "-mcpu=cortex-m0plus -mhwdiv=thumb -mfloat-abi=soft -nostdlib -nodefaultlibs -v -Wno-unused-parameter -fno-builtin -fno-exceptions -fcheck-new -ffreestanding -Wall -Wshadow -fno-strict-aliasing -fomit-frame-pointer")

SET(CMAKE_C_FLAGS "-Wall -Wextra ${ARCH_FLAGS}" CACHE STRING "Common flags for C compiler")
SET(CMAKE_ASM_FLAGS "-x assembler-with-cpp ${ARCH_FLAGS}" CACHE STRING "Common flags for assembler")
SET(CMAKE_CXX_FLAGS "-Wall -Wextra -fno-exceptions -fno-threadsafe-statics ${ARCH_FLAGS}" CACHE STRING "Common flags for C++ compiler")

set(DIALOG_SDK_SOURCES_531
    ${DDIALOG_EXAMPLE_PATH}/build_utils/clang/startup_DA14531.S
)
