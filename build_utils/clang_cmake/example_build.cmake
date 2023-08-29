
# Include Dialog SDK

include(${DIALOG_EXAMPLE_PATH}/../../build_utils/clang_cmake/dialog-sdk.cmake)
include_directories(src/config src src/custom_profile)
FILE(GLOB_RECURSE sourcesFiles src/*.c)
LIST(APPEND userSourceFiles ${sourcesFiles})

if((BUILD_FOR_531 OR BUILD_FOR_585 OR BUILD_FOR_586) AND (BUILD_FOR_531_PERIPHERAL_EXAMPLE OR BUILD_FOR_585_PERIPHERAL_EXAMPLE OR BUILD_FOR_586_PERIPHERAL_EXAMPLE))
    message( FATAL_ERROR "You can't have the normal build path and the Peripheral build path enabled" )
endif()

if(BUILD_FOR_531)
    add_executable(${PROJECT_NAME}_531
                    # Source
                    ${userSourceFiles}
                    ${DIALOG_SDK_SOURCES_SHARED}
                    ${DIALOG_SDK_SOURCES_531}
                   )
endif()

# Common compile and link flags
set(COMMON_FLAGS
    -gdwarf-4
    -Oz
    -flto
    )

# comile flags
set(CFLAGS
    ${COMMON_FLAGS} 
    --target=arm-none-eabi 
    -march=armv6s-m 
    -mcpu=cortex-m0plus
    -mthumb
    -MMD
    -MP
    )

# linker flags
set(LDFLAGS 
    --target=arm-unknown-none-eabi 
    -march=armv6-m 
    -mcpu=cortex-m0plus 
    -nostdlib
    ${COMMON_FLAGS}
    -ffunction-sections
    -fdata-sections
    -Wall 
    -Werror
    )

# GCC crt objects to build 
set(BUILD_GCC_OBJS
    ${DIALOG_SDK_PATH}/crt_objects/crt0.o
    ${DIALOG_SDK_PATH}/crt_objects/crti.o
    ${DIALOG_SDK_PATH}/crt_objects/crtn.o
    )

if(BUILD_FOR_531)
    # add compule definitions
    target_compile_definitions(${PROJECT_NAME}_531 PRIVATE
    $<$<COMPILE_LANGUAGE:C,CXX,ASM>:__DA14531__>
    )
endif()

if(BUILD_FOR_531 OR BUILD_FOR_585 OR BUILD_FOR_586)
    # compile options
    set(SHARED_COMPILE_OPTIONS
    # C, C++
        # .h files in src/config directory 
        $<$<COMPILE_LANGUAGE:C,CXX>:-include${CMAKE_SOURCE_DIR}/src/config/da1458x_config_basic.h>
        $<$<COMPILE_LANGUAGE:C,CXX>:-include${CMAKE_SOURCE_DIR}/src/config/da1458x_config_advanced.h>
        $<$<COMPILE_LANGUAGE:C,CXX>:-include${CMAKE_SOURCE_DIR}/src/config/user_config.h>
    # C only
        $<$<COMPILE_LANGUAGE:C>:-std=gnu11>
        $<$<COMPILE_LANGUAGE:C>:-Wno-int-conversion>
        $<$<COMPILE_LANGUAGE:C>:-Wno-unused-variable>
    # C++ only
        $<$<COMPILE_LANGUAGE:CXX>:-std=c++11>
        $<$<COMPILE_LANGUAGE:CXX>:-fms-extensions>
        $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
        $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
        $<$<COMPILE_LANGUAGE:CXX>:-fno-use-cxa-atexit>
        $<$<COMPILE_LANGUAGE:CXX>:-fno-threadsafe-statics>
        $<$<COMPILE_LANGUAGE:CXX>:-Wold-style-cast>
        $<$<COMPILE_LANGUAGE:CXX>:-Wsuggest-override>
    # Assembly
        $<$<COMPILE_LANGUAGE:ASM>:-xassembler-with-cpp>
    )
endif()


if(BUILD_FOR_531 OR BUILD_FOR_531_PERIPHERAL_EXAMPLE)
    target_compile_options(${PROJECT_NAME}_531 PRIVATE
    # C, C++, Assembly
       ${SHARED_COMPILE_OPTIONS}
       ${CFLAGS}
    )

endif()

set(USER_INCLUDES
    ${CMAKE_SOURCE_DIR}/include
    # GCC includes
    ${GCC_TOOLCHAIN_PATH}/arm-none-eabi/include/
)

if(BUILD_FOR_531 OR BUILD_FOR_531_PERIPHERAL_EXAMPLE)
    target_include_directories(${PROJECT_NAME}_531 PRIVATE
        $<$<COMPILE_LANGUAGE:C,CXX>:${DIALOG_SDK_INCLUDES}>
        $<$<COMPILE_LANGUAGE:C,CXX>:${USER_INCLUDES}>
    )
endif()

# Linker file needs to be preprocessed
set(LINKER_SCRIPT_IN_531 "${DIALOG_SDK_PATH}/clang_ldscripts/ldscript_DA14531.lds.S")
set(LINKER_SCRIPT_OUT_531 "ldscript_DA14531.lds")


# Linker script includes
set(LINKER_SCRIPT_INCLUDES
    -I${DIALOG_SDK_PATH}/sdk/common_project_files
    -I${CMAKE_SOURCE_DIR}/src/config
    -I${CMAKE_SOURCE_DIR}/src
)

function(generate_linker_script target_name linker_script_includes linker_script_defs linker_script_input linker_script_output)
    # Check the input file
    if(NOT EXISTS ${linker_script_input})
        message(FATAL_ERROR "Linker input file not found.")
    else()
        message(STATUS "Found linker input file: ${linker_script_input}")
    endif()

    add_custom_command(OUTPUT ${linker_script_output}
                    COMMAND ${CMAKE_C_COMPILER} ${linker_script_includes} ${linker_script_defs} -E -P -c ${linker_script_input} -o ${linker_script_output}
                    DEPENDS ${linker_script_input}
                    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
                    COMMENT "Generating linker script: ${linker_script_output}"
                    VERBATIM)

    add_custom_target(${target_name}-linkerscript DEPENDS ${linker_script_output} VERBATIM)

    # Add linker preprocessed file as dependency
    add_dependencies(${target_name} ${target_name}-linkerscript)
    # Add dependency on linker input file
    set_target_properties(${target_name} PROPERTIES LINK_DEPENDS ${linker_script_input})
endfunction(generate_linker_script)

if(BUILD_FOR_531)
    generate_linker_script(${PROJECT_NAME}_531 "${LINKER_SCRIPT_INCLUDES}" "-D__DA14531__" "${LINKER_SCRIPT_IN_531}" "${LINKER_SCRIPT_OUT_531}")
endif()

message("TARGET_LINK_DIRECTORIES_COMMON=${TARGET_LINK_DIRECTORIES_COMMON}")

if(BUILD_FOR_531 OR BUILD_FOR_531_PERIPHERAL_EXAMPLE)
    target_link_directories(${PROJECT_NAME}_531 PRIVATE
        ${DIALOG_SDK_PATH}/sdk/platform/system_library/output/Keil_5/
        ${DIALOG_SDK_PATH}/sdk/common_project_files/scatterfiles/
        ${DIALOG_SDK_PATH}/sdk/common_project_files/misc/
        ${GCC_TOOLCHAIN_PATH}/arm-none-eabi/lib/thumb/v6-m/
        ${GCC_TOOLCHAIN_PATH}/lib/gcc/arm-none-eabi/7.3.1/thumb/v6-m/   
    )
endif()

if(BUILD_FOR_531 OR BUILD_FOR_531_PERIPHERAL_EXAMPLE)
    target_link_libraries(${PROJECT_NAME}_531 PRIVATE
        :libg_nano.a
        :libnosys.a
        :libgcc.a
        :da14531.lib
    )
endif()

if(BUILD_FOR_531)
    target_link_options(${PROJECT_NAME}_531 PRIVATE
        ${LDFLAGS}
        ${BUILD_GCC_OBJS}
        "-T${LINKER_SCRIPT_OUT_531}"
        "-Wl,--gc-sections,-Map,${PROJECT_NAME}_531.map" # Produce map file          
    )
endif()

# Post build actions

if(BUILD_FOR_531 OR BUILD_FOR_531_PERIPHERAL_EXAMPLE)
    add_custom_command(TARGET ${PROJECT_NAME}_531 POST_BUILD
        COMMAND ${CMAKE_SIZE} --format=berkeley "$<TARGET_FILE:${PROJECT_NAME}_531>"
        COMMENT "Print output application size"
    )

    add_custom_command(TARGET ${PROJECT_NAME}_531 POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O ihex "$<TARGET_FILE:${PROJECT_NAME}_531>" ${PROJECT_NAME}_531.hex
        COMMENT "Convert output to hex"
    )

    add_custom_command(TARGET ${PROJECT_NAME}_531 POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O binary "$<TARGET_FILE:${PROJECT_NAME}_531>" ${PROJECT_NAME}_531.bin
        COMMENT "Convert output to binary"
    )
endif()
