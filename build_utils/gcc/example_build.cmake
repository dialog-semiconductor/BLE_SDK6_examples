
include(${DIALOG_EXAMPLE_PATH}/build_utils/gcc/dialog-sdk.cmake)
# Include Dialog SDK
include(${DIALOG_EXAMPLE_PATH}/build_utils/gcc/dialog-sdk.cmake)
include_directories(src/config src src/custom_profile)
FILE(GLOB_RECURSE sourcesFiles src/*.c)
LIST(APPEND userSourceFiles ${sourcesFiles})

add_executable(${PROJECT_NAME}_531
    ${DIALOG_SDK_SOURCES_SHARED}
    ${DIALOG_SDK_SOURCES_531}
    # Source
    ${userSourceFiles}
)

add_executable(${PROJECT_NAME}_585
    ${DIALOG_SDK_SOURCES_SHARED}
    ${DIALOG_SDK_SOURCES_58x}
    # Source
    ${userSourceFiles}
)

set(GLOBAL_DEBUG_OPTIONS_531 -mthumb -mcpu=cortex-m0plus -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -flto -Wall)
set(GLOBAL_DEBUG_OPTIONS_58x -mthumb -mcpu=cortex-m0 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -flto -Wall)

target_compile_definitions(${PROJECT_NAME}_531 PRIVATE
    $<$<COMPILE_LANGUAGE:C,CXX,ASM>:__DA14531__>
)

target_compile_definitions(${PROJECT_NAME}_585 PRIVATE
    $<$<COMPILE_LANGUAGE:C,CXX,ASM>:__DA14585__>
)


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

target_compile_options(${PROJECT_NAME}_531 PRIVATE
# C, C++, Assembly
    $<$<COMPILE_LANGUAGE:C,CXX,ASM>:${GLOBAL_DEBUG_OPTIONS_531}>
    ${SHARED_COMPILE_OPTIONS}
)

target_compile_options(${PROJECT_NAME}_585 PRIVATE
# C, C++, Assembly
    $<$<COMPILE_LANGUAGE:C,CXX,ASM>:${GLOBAL_DEBUG_OPTIONS_58x}>
    ${SHARED_COMPILE_OPTIONS}
)

set(USER_INCLUDES
    ${CMAKE_SOURCE_DIR}/include
    # GCC includes
    # ${GCC_TOOLCHAIN_PATH}/arm-none-eabi/include/
)

target_include_directories(${PROJECT_NAME}_531 PRIVATE
    $<$<COMPILE_LANGUAGE:C,CXX>:${DIALOG_SDK_INCLUDES}>
    $<$<COMPILE_LANGUAGE:C,CXX>:${USER_INCLUDES}>
)

target_include_directories(${PROJECT_NAME}_585 PRIVATE
    $<$<COMPILE_LANGUAGE:C,CXX>:${DIALOG_SDK_INCLUDES}>
    $<$<COMPILE_LANGUAGE:C,CXX>:${USER_INCLUDES}>
)

# Linker file needs to be preprocessed
set(LINKER_SCRIPT_IN_531 "${DIALOG_EXAMPLE_PATH}/build_utils/gcc/ldscript_DA14531.lds.S")
set(LINKER_SCRIPT_OUT_531 "ldscript_531.lds")
# Linker file needs to be preprocessed
set(LINKER_SCRIPT_IN_58x "${DIALOG_EXAMPLE_PATH}/build_utils/gcc/ldscript_DA14585_586.lds.S")
set(LINKER_SCRIPT_OUT_58x "ldscript_585_586.lds")

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

generate_linker_script(${PROJECT_NAME}_531 "${LINKER_SCRIPT_INCLUDES}" "-D__DA14531__" "${LINKER_SCRIPT_IN_531}" "${LINKER_SCRIPT_OUT_531}")
generate_linker_script(${PROJECT_NAME}_585 "${LINKER_SCRIPT_INCLUDES}" "-D__DA14585__" "${LINKER_SCRIPT_IN_58x}" "${LINKER_SCRIPT_OUT_58x}")

set(TARGET_LINK_DIRECTORIES_COMMON
    ${DIALOG_SDK_PATH}/sdk/gcc
    ${DIALOG_SDK_PATH}/sdk/platform/system_library/output/Keil_5/
    ${DIALOG_SDK_PATH}/sdk/common_project_files/scatterfiles/
    ${DIALOG_SDK_PATH}/sdk/common_project_files/misc/
)

target_link_directories(${PROJECT_NAME}_531 PRIVATE
    ${TARGET_LINK_DIRECTORIES_COMMON}
)

target_link_directories(${PROJECT_NAME}_585 PRIVATE
    ${TARGET_LINK_DIRECTORIES_COMMON}
)

target_link_libraries(${PROJECT_NAME}_531 PRIVATE
    :da14531.lib
)

target_link_libraries(${PROJECT_NAME}_585 PRIVATE
    :da14585_586.lib
)

set(TARGET_LINK_OPTIONS_COMMON
    "-Xlinker"
    "--gc-sections"
    "--specs=nano.specs"
    "--specs=nosys.specs"
#    "-v"
    "-Wl,--no-wchar-size-warning" # Suppress the warning from linking Dialog's system library
)

target_link_options(${PROJECT_NAME}_531 PRIVATE
    ${GLOBAL_DEBUG_OPTIONS_531}
    "-T${LINKER_SCRIPT_OUT_531}"
    ${TARGET_LINK_OPTIONS_COMMON}
    "-Wl,-Map,${PROJECT_NAME}_531.map" # Produce map file
)

target_link_options(${PROJECT_NAME}_585 PRIVATE
    ${GLOBAL_DEBUG_OPTIONS_58x}
    "-T${LINKER_SCRIPT_OUT_58x}"
    ${TARGET_LINK_OPTIONS_COMMON}
    "-Wl,-Map,${PROJECT_NAME}_585.map" # Produce map file
)

# Post build actions

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

# Post build actions

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
