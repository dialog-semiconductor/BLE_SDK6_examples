set(CMAKE_SYSTEM_NAME "Cortex-M0plus")
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)

SET (CMAKE_C_COMPILER   "clang")
SET (CMAKE_CXX_COMPILER "clang++")
SET (CMAKE_ASM_COMPILER "clang")
SET (CMAKE_AR           "llvm-ar")
SET (CMAKE_LINKER       "llvm-ld")
SET (CMAKE_NM           "llvm-nm")
SET (CMAKE_OBJDUMP      "llvm-objdump")
SET (CMAKE_RANLIB       "llvm-ranlib")

set(CMAKE_C_COMPILER_ID Clang)
set(CMAKE_CXX_COMPILER_ID Clang)

set(CMAKE_EXECUTABLE_SUFFIX_C   .elf)
set(CMAKE_EXECUTABLE_SUFFIX_CXX .elf)
set(CMAKE_EXECUTABLE_SUFFIX_ASM .elf)

# C/C++ toolchain
# SET(TOOLCHAIN "/usr/local/opt/llvm/bin")
SET(CMAKE_C_COMPILER_EXTERNAL_TOOLCHAIN ${TOOLCHAIN})
SET(CMAKE_CXX_COMPILER_EXTERNAL_TOOLCHAIN ${TOOLCHAIN})

SET(ARCH_FLAGS "--target=arm-unknown-none-eabi -march=armv6-m -mcpu=cortex-m0plus -mhwdiv=thumb -mfloat-abi=soft --sysroot=/usr/lib/gcc/arm-none-eabi -nostdlib -nodefaultlibs -v")
SET(CMAKE_C_FLAGS "-Wall -Wextra ${COMPILER_FLAGS}" CACHE STRING "Common flags for C compiler")
SET(CMAKE_CXX_FLAGS "-Wall -Wextra -fno-exceptions -fno-threadsafe-statics ${COMPILER_FLAGS}" CACHE STRING "Common flags for C++ compiler")
