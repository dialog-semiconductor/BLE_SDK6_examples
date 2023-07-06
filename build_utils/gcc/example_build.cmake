
include(${DIALOG_EXAMPLE_PATH}/build_utils/gcc/dialog-sdk.cmake)
# Include Dialog SDK
include(${DIALOG_EXAMPLE_PATH}/build_utils/gcc/dialog-sdk.cmake)
include_directories(src/config src src/custom_profile)
FILE(GLOB_RECURSE sourcesFiles src/*.c)
LIST(APPEND userSourceFiles ${sourcesFiles})

if((BUILD_FOR_531 OR BUILD_FOR_585 OR BUILD_FOR_586) AND (BUILD_FOR_531_PERIPHERAL_EXAMPLE OR BUILD_FOR_585_PERIPHERAL_EXAMPLE OR BUILD_FOR_586_PERIPHERAL_EXAMPLE))
    message( FATAL_ERROR "You can't have the normal build path and the Peripheral build path enabled" )
endif()

if(BUILD_FOR_585)
    add_executable(${PROJECT_NAME}_585
        # Source
        ${userSourceFiles}
        ${DIALOG_SDK_SOURCES_SHARED}
        ${DIALOG_SDK_SOURCES_58x_BLE}
        ${DIALOG_SDK_SOURCES_58x}
    )
endif()

if(BUILD_FOR_586)
add_executable(${PROJECT_NAME}_586
# Source
${userSourceFiles}
${DIALOG_SDK_SOURCES_SHARED}
${DIALOG_SDK_SOURCES_58x_BLE}
${DIALOG_SDK_SOURCES_58x}
)
endif()

if(BUILD_FOR_531)
add_executable(${PROJECT_NAME}_531
# Source
${userSourceFiles}
${DIALOG_SDK_SOURCES_SHARED}
${DIALOG_SDK_SOURCES_531}
)
endif()

if(BUILD_FOR_585_PERIPHERAL_EXAMPLE)
add_executable(${PROJECT_NAME}_585
# Source
${userSourceFiles}
${DIALOG_SDK_SOURCES_PERIPH_EXAMPLE}
${DIALOG_SDK_SOURCES_58x}
)
endif()

if(BUILD_FOR_586_PERIPHERAL_EXAMPLE)
add_executable(${PROJECT_NAME}_586
# Source
${userSourceFiles}
${DIALOG_SDK_SOURCES_PERIPH_EXAMPLE}
${DIALOG_SDK_SOURCES_58x}
)
endif()

if(BUILD_FOR_531_PERIPHERAL_EXAMPLE)
add_executable(${PROJECT_NAME}_531
# Source
${userSourceFiles}
${DIALOG_SDK_SOURCES_PERIPH_EXAMPLE}
${DIALOG_SDK_SOURCES_531}
)
endif()

set(GLOBAL_DEBUG_OPTIONS_531 -mthumb -mcpu=cortex-m0plus -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -flto -Wall  --specs=nano.specs )
set(GLOBAL_DEBUG_OPTIONS_58x -mthumb -mcpu=cortex-m0 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -flto -Wall  --specs=nano.specs )


if(BUILD_FOR_531)
target_compile_definitions(${PROJECT_NAME}_531 PRIVATE
$<$<COMPILE_LANGUAGE:C,CXX,ASM>:__DA14531__>
)
endif()

if(BUILD_FOR_585)
    target_compile_definitions(${PROJECT_NAME}_585 PRIVATE
        $<$<COMPILE_LANGUAGE:C,CXX,ASM>:__DA14585__>
    )
endif()

if(BUILD_FOR_586)
    target_compile_definitions(${PROJECT_NAME}_586 PRIVATE
        $<$<COMPILE_LANGUAGE:C,CXX,ASM>:__DA14586__>
    )
endif()

if(BUILD_FOR_531_PERIPHERAL_EXAMPLE)
    target_compile_definitions(${PROJECT_NAME}_531 PRIVATE
        $<$<COMPILE_LANGUAGE:C,CXX,ASM>:__DA14531__>
        $<$<COMPILE_LANGUAGE:C,CXX,ASM>:__NON_BLE_EXAMPLE__>
    )
endif()

if(BUILD_FOR_585_PERIPHERAL_EXAMPLE)
    target_compile_definitions(${PROJECT_NAME}_585 PRIVATE
        $<$<COMPILE_LANGUAGE:C,CXX,ASM>:__DA14585__>
        $<$<COMPILE_LANGUAGE:C,CXX,ASM>:__NON_BLE_EXAMPLE__>
    )
endif()

if(BUILD_FOR_586_PERIPHERAL_EXAMPLE)
    target_compile_definitions(${PROJECT_NAME}_586 PRIVATE
        $<$<COMPILE_LANGUAGE:C,CXX,ASM>:__DA14586__>
        $<$<COMPILE_LANGUAGE:C,CXX,ASM>:__NON_BLE_EXAMPLE__>
    )
endif()

if(BUILD_FOR_531 OR BUILD_FOR_585 OR BUILD_FOR_586)
    set(SHARED_COMPILE_OPTIONS
    # C, C++
        $<$<COMPILE_LANGUAGE:C,CXX>:-include${CMAKE_SOURCE_DIR}/src/config/da1458x_config_basic.h>
        $<$<COMPILE_LANGUAGE:C,CXX>:-include${CMAKE_SOURCE_DIR}/src/config/da1458x_config_advanced.h>
        $<$<COMPILE_LANGUAGE:C,CXX>:-include${CMAKE_SOURCE_DIR}/src/config/user_config.h>
    # C only
        $<$<COMPILE_LANGUAGE:C>:-std=gnu11>
        $<$<COMPILE_LANGUAGE:C>:-Wno-int-conversion>
        $<$<COMPILE_LANGUAGE:C>:-Wno-unused-variable>
        $<$<COMPILE_LANGUAGE:C>:-Wno-address>
        # -fstrict-volatile-bitfields
        # -Werror
        # -Wextra
        # -Wcast-align
        # -Wconversion
        # -Wsign-conversion
        # -Wshadow
        # -Wlogical-op
        # -Wsuggest-final-types
        # -Wsuggest-final-methods
        # -pedantic
        # -Wno-expansion-to-defined
        # -Wno-unused-parameter
        # -Wno-shadow
        # -Wno-sign-compare
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

if(BUILD_FOR_531_PERIPHERAL_EXAMPLE OR BUILD_FOR_585_PERIPHERAL_EXAMPLE OR BUILD_FOR_586_PERIPHERAL_EXAMPLE)
    set(SHARED_COMPILE_OPTIONS
    # C only
        $<$<COMPILE_LANGUAGE:C>:-std=gnu11>
        $<$<COMPILE_LANGUAGE:C>:-Wno-int-conversion>
        $<$<COMPILE_LANGUAGE:C>:-Wno-unused-variable>
        $<$<COMPILE_LANGUAGE:C>:-Wno-address>
        # -fstrict-volatile-bitfields
        # -Werror
        # -Wextra
        # -Wcast-align
        # -Wconversion
        # -Wsign-conversion
        # -Wshadow
        # -Wlogical-op
        # -Wsuggest-final-types
        # -Wsuggest-final-methods
        # -pedantic
        # -Wno-expansion-to-defined
        # -Wno-unused-parameter
        # -Wno-shadow
        # -Wno-sign-compare
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
        $<$<COMPILE_LANGUAGE:C,CXX,ASM>:${GLOBAL_DEBUG_OPTIONS_531}>
        ${SHARED_COMPILE_OPTIONS}
    )
endif()

if(BUILD_FOR_585 OR BUILD_FOR_585_PERIPHERAL_EXAMPLE)
    target_compile_options(${PROJECT_NAME}_585 PRIVATE
    # C, C++, Assembly
        $<$<COMPILE_LANGUAGE:C,CXX,ASM>:${GLOBAL_DEBUG_OPTIONS_58x}>
        ${SHARED_COMPILE_OPTIONS}
    )
endif()

if(BUILD_FOR_586 OR BUILD_FOR_586_PERIPHERAL_EXAMPLE)
    target_compile_options(${PROJECT_NAME}_586 PRIVATE
    # C, C++, Assembly
        $<$<COMPILE_LANGUAGE:C,CXX,ASM>:${GLOBAL_DEBUG_OPTIONS_58x}>
        ${SHARED_COMPILE_OPTIONS}
    )
endif()

set(USER_INCLUDES
    ${CMAKE_SOURCE_DIR}/include
    # GCC includes
    # ${GCC_TOOLCHAIN_PATH}/arm-none-eabi/include/
)

if(BUILD_FOR_531 OR BUILD_FOR_531_PERIPHERAL_EXAMPLE)
    target_include_directories(${PROJECT_NAME}_531 PRIVATE
        $<$<COMPILE_LANGUAGE:C,CXX>:${DIALOG_SDK_INCLUDES}>
        $<$<COMPILE_LANGUAGE:C,CXX>:${USER_INCLUDES}>
    )
endif()

if(BUILD_FOR_585 OR BUILD_FOR_585_PERIPHERAL_EXAMPLE)
    target_include_directories(${PROJECT_NAME}_585 PRIVATE
        $<$<COMPILE_LANGUAGE:C,CXX>:${DIALOG_SDK_INCLUDES}>
        $<$<COMPILE_LANGUAGE:C,CXX>:${USER_INCLUDES}>
    )
endif()

if(BUILD_FOR_586 OR BUILD_FOR_586_PERIPHERAL_EXAMPLE)
    target_include_directories(${PROJECT_NAME}_586 PRIVATE
        $<$<COMPILE_LANGUAGE:C,CXX>:${DIALOG_SDK_INCLUDES}>
        $<$<COMPILE_LANGUAGE:C,CXX>:${USER_INCLUDES}>
    )
endif()

# Linker file needs to be preprocessed
set(LINKER_SCRIPT_IN_531 "${DIALOG_EXAMPLE_PATH}/build_utils/gcc/ldscript_DA14531.lds.S")
set(LINKER_SCRIPT_OUT_531 "ldscript_531.lds")
# Linker file needs to be preprocessed
set(LINKER_SCRIPT_IN_58x "${DIALOG_EXAMPLE_PATH}/build_utils/gcc/ldscript_DA14585_586.lds.S")
set(LINKER_SCRIPT_OUT_585 "ldscript_585.lds")
set(LINKER_SCRIPT_OUT_586 "ldscript_586.lds")
# Linker file needs to be preprocessed
set(LINKER_SCRIPT_IN_58x_PERIPHERAL_EXAMPLE "${DIALOG_EXAMPLE_PATH}/build_utils/gcc/ldscript_peripheral_examples.lds.S")
set(LINKER_SCRIPT_OUT_531_PERIPHERAL_EXAMPLE "ldscript_531_PERIPHERAL_EXAMPLE.lds")
set(LINKER_SCRIPT_OUT_585_PERIPHERAL_EXAMPLE "ldscript_585_PERIPHERAL_EXAMPLE.lds")
set(LINKER_SCRIPT_OUT_586_PERIPHERAL_EXAMPLE "ldscript_586_PERIPHERAL_EXAMPLE.lds")

# Linker script includes
set(LINKER_SCRIPT_INCLUDES
    -I ${DIALOG_SDK_PATH}/sdk/common_project_files
    -I ${DIALOG_EXAMPLE_PATH}/build_utils/gcc/
    -I ${CMAKE_SOURCE_DIR}/src/config
    -I ${CMAKE_SOURCE_DIR}/src
)

link_directories(
    ${CMAKE_SOURCE_DIR}/gcc
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

if(BUILD_FOR_585)
    generate_linker_script(${PROJECT_NAME}_585 "${LINKER_SCRIPT_INCLUDES}" "-D__DA14585__" "${LINKER_SCRIPT_IN_58x}" "${LINKER_SCRIPT_OUT_585}")
endif()

if(BUILD_FOR_586)
    generate_linker_script(${PROJECT_NAME}_586 "${LINKER_SCRIPT_INCLUDES}" "-D__DA14586__" "${LINKER_SCRIPT_IN_58x}" "${LINKER_SCRIPT_OUT_586}")
endif()

if(BUILD_FOR_531_PERIPHERAL_EXAMPLE)
    generate_linker_script(${PROJECT_NAME}_531 "${LINKER_SCRIPT_INCLUDES}" "-D__DA14531__ -D__NON_BLE_EXAMPLE__" "${LINKER_SCRIPT_IN_58x_PERIPHERAL_EXAMPLE}" "${LINKER_SCRIPT_OUT_531_PERIPHERAL_EXAMPLE}")
endif()

if(BUILD_FOR_585_PERIPHERAL_EXAMPLE)
    generate_linker_script(${PROJECT_NAME}_585 "${LINKER_SCRIPT_INCLUDES}" "-D__DA14585__ -D__NON_BLE_EXAMPLE__" "${LINKER_SCRIPT_IN_58x_PERIPHERAL_EXAMPLE}" "${LINKER_SCRIPT_OUT_585_PERIPHERAL_EXAMPLE}")
endif()

if(BUILD_FOR_586_PERIPHERAL_EXAMPLE)
    generate_linker_script(${PROJECT_NAME}_586 "${LINKER_SCRIPT_INCLUDES}" "-D__DA14586__ -D__NON_BLE_EXAMPLE__" "${LINKER_SCRIPT_IN_58x_PERIPHERAL_EXAMPLE}" "${LINKER_SCRIPT_OUT_586_PERIPHERAL_EXAMPLE}")
endif()

set(TARGET_LINK_DIRECTORIES_COMMON
    ${DIALOG_SDK_PATH}/sdk/gcc
    ${DIALOG_SDK_PATH}/sdk/platform/system_library/output/Keil_5/
    ${DIALOG_SDK_PATH}/sdk/common_project_files/scatterfiles/
    ${DIALOG_SDK_PATH}/sdk/common_project_files/misc/
)

if(BUILD_FOR_531 OR BUILD_FOR_531_PERIPHERAL_EXAMPLE)
    target_link_directories(${PROJECT_NAME}_531 PRIVATE
        ${TARGET_LINK_DIRECTORIES_COMMON}
    )
endif()

if(BUILD_FOR_585 OR BUILD_FOR_585_PERIPHERAL_EXAMPLE)
    target_link_directories(${PROJECT_NAME}_585 PRIVATE
        ${TARGET_LINK_DIRECTORIES_COMMON}
    )
endif()

if(BUILD_FOR_586 OR BUILD_FOR_586_PERIPHERAL_EXAMPLE)
    target_link_directories(${PROJECT_NAME}_586 PRIVATE
        ${TARGET_LINK_DIRECTORIES_COMMON}
    )
endif()

if(BUILD_FOR_531 OR BUILD_FOR_531_PERIPHERAL_EXAMPLE)
    target_link_libraries(${PROJECT_NAME}_531 PRIVATE
        :da14531.lib
    )
endif()

if(BUILD_FOR_585 OR BUILD_FOR_585_PERIPHERAL_EXAMPLE)
    target_link_libraries(${PROJECT_NAME}_585 PRIVATE
        :da14585_586.lib
    )
endif()

if(BUILD_FOR_586 OR BUILD_FOR_586_PERIPHERAL_EXAMPLE)
    target_link_libraries(${PROJECT_NAME}_586 PRIVATE
        :da14585_586.lib
    )
endif()

set(TARGET_LINK_OPTIONS_COMMON
    "-Xlinker"
    "--gc-sections"
    #"--specs=nosys.specs"
    #"-Wl,--no-wchar-size-warning" # Suppress the warning from linking Dialog's system library
)

if(BUILD_FOR_531)
    target_link_options(${PROJECT_NAME}_531 PRIVATE
        ${GLOBAL_DEBUG_OPTIONS_531}
        "-T${LINKER_SCRIPT_OUT_531}"
        ${TARGET_LINK_OPTIONS_COMMON}
        "-Wl,-Map,${PROJECT_NAME}_531.map" # Produce map file
    )
endif()

if(BUILD_FOR_585)
    target_link_options(${PROJECT_NAME}_585 PRIVATE
        ${GLOBAL_DEBUG_OPTIONS_58x}
        "-T${LINKER_SCRIPT_OUT_585}"
        ${TARGET_LINK_OPTIONS_COMMON}
        "-Wl,-Map,${PROJECT_NAME}_585.map" # Produce map file
    )
endif()

if(BUILD_FOR_586)
    target_link_options(${PROJECT_NAME}_586 PRIVATE
        ${GLOBAL_DEBUG_OPTIONS_58x}
        "-T${LINKER_SCRIPT_OUT_586}"
        ${TARGET_LINK_OPTIONS_COMMON}
        "-Wl,-Map,${PROJECT_NAME}_586.map" # Produce map file
    )
endif()

if(BUILD_FOR_531_PERIPHERAL_EXAMPLE)
    target_link_options(${PROJECT_NAME}_531 PRIVATE
        ${GLOBAL_DEBUG_OPTIONS_531}
        "-T${LINKER_SCRIPT_OUT_531_PERIPHERAL_EXAMPLE}"
        ${TARGET_LINK_OPTIONS_COMMON}
        "-Wl,-Map,${PROJECT_NAME}_531.map" # Produce map file
    )
endif()

if(BUILD_FOR_585_PERIPHERAL_EXAMPLE)
    target_link_options(${PROJECT_NAME}_585 PRIVATE
        ${GLOBAL_DEBUG_OPTIONS_58x}
        "-T${LINKER_SCRIPT_OUT_585_PERIPHERAL_EXAMPLE}"
        ${TARGET_LINK_OPTIONS_COMMON}
        "-Wl,-Map,${PROJECT_NAME}_585.map" # Produce map file
    )
endif()

if(BUILD_FOR_586_PERIPHERAL_EXAMPLE)
    target_link_options(${PROJECT_NAME}_586 PRIVATE
        ${GLOBAL_DEBUG_OPTIONS_58x}
        "-T${LINKER_SCRIPT_OUT_586_PERIPHERAL_EXAMPLE}"
        ${TARGET_LINK_OPTIONS_COMMON}
        "-Wl,-Map,${PROJECT_NAME}_586.map" # Produce map file
    )
endif()

# Post build actions

if(BUILD_FOR_531 OR BUILD_FOR_531_PERIPHERAL_EXAMPLE)
    add_custom_command(TARGET ${PROJECT_NAME}_531 POST_BUILD
        COMMAND ${CMAKE_SIZE} --format=berkeley "$<TARGET_FILE:${PROJECT_NAME}_531>"
        COMMENT "Print output application size"
    )

    add_custom_command(TARGET ${PROJECT_NAME}_531 POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex "$<TARGET_FILE:${PROJECT_NAME}_531>" ${PROJECT_NAME}_531.hex
        COMMENT "Convert output to hex"
    )

    add_custom_command(TARGET ${PROJECT_NAME}_531 POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Obinary "$<TARGET_FILE:${PROJECT_NAME}_531>" ${PROJECT_NAME}_531.bin
        COMMENT "Convert output to binary"
    )
endif()

# Post build actions

if(BUILD_FOR_585 OR BUILD_FOR_585_PERIPHERAL_EXAMPLE)
add_custom_command(TARGET ${PROJECT_NAME}_585 POST_BUILD
    COMMAND ${CMAKE_SIZE} --format=berkeley "$<TARGET_FILE:${PROJECT_NAME}_585>"
    COMMENT "Print output application size"
)

add_custom_command(TARGET ${PROJECT_NAME}_585 POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -Oihex "$<TARGET_FILE:${PROJECT_NAME}_585>" ${PROJECT_NAME}_585.hex
    COMMENT "Convert output to hex"
)

add_custom_command(TARGET ${PROJECT_NAME}_585 POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -Obinary "$<TARGET_FILE:${PROJECT_NAME}_585>" ${PROJECT_NAME}_585.bin
    COMMENT "Convert output to binary"
)
endif()

# Post build actions

if(BUILD_FOR_586 OR BUILD_FOR_586_PERIPHERAL_EXAMPLE)
    add_custom_command(TARGET ${PROJECT_NAME}_586 POST_BUILD
        COMMAND ${CMAKE_SIZE} --format=berkeley "$<TARGET_FILE:${PROJECT_NAME}_586>"
        COMMENT "Print output application size"
    )

    add_custom_command(TARGET ${PROJECT_NAME}_586 POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex "$<TARGET_FILE:${PROJECT_NAME}_586>" ${PROJECT_NAME}_586.hex
        COMMENT "Convert output to hex"
    )

    add_custom_command(TARGET ${PROJECT_NAME}_586 POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Obinary "$<TARGET_FILE:${PROJECT_NAME}_586>" ${PROJECT_NAME}_586.bin
        COMMENT "Convert output to binary"
    )
endif()
