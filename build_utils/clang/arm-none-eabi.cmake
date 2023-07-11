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
SET (CMAKE_OBJCOPY      "llvm-objcopy-15")
SET (CMAKE_SIZE         "llvm-size-15")

# set(CMAKE_LINKER        "arm-none-eabi-ld")
# set(CMAKE_AR            "arm-none-eabi-ar")

# SET(CMAKE_CXX_LINK_EXECUTABLE "<CMAKE_LINKER> <FLAGS> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
# SET(CMAKE_C_LINK_EXECUTABLE "<CMAKE_LINKER> <FLAGS> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
# SET(CMAKE_ASM_LINK_EXECUTABLE "<CMAKE_LINKER> <FLAGS> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")

# SET (CMAKE_AR           "clang")
# SET (CMAKE_NM           "clang")
# SET (CMAKE_OBJDUMP      "clang")
# SET (CMAKE_RANLIB       "clang")

set(CMAKE_EXECUTABLE_SUFFIX_C   .elf)
set(CMAKE_EXECUTABLE_SUFFIX_CXX .elf)
set(CMAKE_EXECUTABLE_SUFFIX_ASM .elf)

SET(CMAKE_SYSROOT "/lib/arm-none-eabi")
SET(CMAKE_SYSROOT_COMPILE "/lib/arm-none-eabi")
SET(CMAKE_SYSROOT_LINK "/lib/arm-none-eabi")

SET(ARCH_FLAGS "-v --target=arm-arm-none-eabi -nodefaultlibs -ccc-gcc-name arm-none-eabi-g++ -isystem /lib/arm-none-eabi -isystem /lib/arm-none-eabi/include")

SET(CMAKE_C_FLAGS "${ARCH_FLAGS}" CACHE STRING "Common flags for C compiler")
SET(CMAKE_ASM_FLAGS "${ARCH_FLAGS}" CACHE STRING "Common flags for assembler")
SET(CMAKE_CXX_FLAGS "${ARCH_FLAGS}" CACHE STRING "Common flags for C++ compiler")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -B/lib/arm-none-eabi/bin --specs=nosys.specs")


set(DIALOG_SDK_SOURCES_531
    ${DDIALOG_EXAMPLE_PATH}/build_utils/clang/startup_DA14531.S
)
