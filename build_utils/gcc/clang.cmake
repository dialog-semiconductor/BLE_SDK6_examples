set(CMAKE_SYSTEM_NAME "generic")
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_CROSSCOMPILING TRUE)
#set(CMAKE_C_COMPILER_FORCED TRUE)
#set(CMAKE_CXX_COMPILER_FORCED TRUE)

# Clang target triple
SET(TARGET arm-none-eabi)

SET (CMAKE_C_COMPILER   "clang")
SET (CMAKE_CXX_COMPILER "clang++")
SET (CMAKE_ASM_COMPILER "clang")
SET (CMAKE_AR           "llvm-ar")
SET (CMAKE_LINKER       "llvm-ld")
SET (CMAKE_CXX_LINK_EXECUTABLE  )
#SET (CMAKE_NM           "llvm-nm")
#SET (CMAKE_OBJDUMP      "llvm-objdump")
SET (CMAKE_OBJCOPY      "llvm-objcopy")
#SET (CMAKE_RANLIB       "llvm-ranlib")
SET (CMAKE_SIZE         "llvm-size")

# set(CMAKE_C_COMPILER_ID Clang)
# set(CMAKE_CXX_COMPILER_ID Clang)

set(CMAKE_EXECUTABLE_SUFFIX_C   .elf)
set(CMAKE_EXECUTABLE_SUFFIX_CXX .elf)
set(CMAKE_EXECUTABLE_SUFFIX_ASM .elf)

# C/C++ toolchain
# # SET(TOOLCHAIN "/usr/local/opt/llvm/bin")
SET(CMAKE_C_COMPILER_EXTERNAL_TOOLCHAIN ${TOOLCHAIN})
SET(CMAKE_CXX_COMPILER_EXTERNAL_TOOLCHAIN ${TOOLCHAIN})

SET(ARCH_FLAGS "--target=arm-unknown-none-eabi -march=armv6-m -mcpu=cortex-m0plus -mhwdiv=thumb -mfloat-abi=soft --sysroot=/lib/arm-none-eabi -nostdlib -nodefaultlibs -v -Wno-unused-parameter")
SET(CMAKE_C_FLAGS "-Wall -Wextra ${ARCH_FLAGS}" CACHE STRING "Common flags for C compiler")
SET(CMAKE_ASM_FLAGS "-x assembler-with-cpp ${ARCH_FLAGS}" CACHE STRING "Common flags for assembler")
SET(CMAKE_CXX_FLAGS "-Wall -Wextra -fno-exceptions -fno-threadsafe-statics ${ARCH_FLAGS}" CACHE STRING "Common flags for C++ compiler")
