# This target will get run if just `make` is used. Be sure to set the BUILD_CONFIG to one of BUILD_CONFIG_TYPES
default: all

-include ../../build_utils/clang/makefile.init

SOURCEROOT ?= ../../../../..
SDKROOT := $(SOURCEROOT)/sdk
APP_SOURCE_DIR ?= $(SOURCEROOT)/projects/target_apps/ble_examples/prox_reporter_makefile/src 
CONFIG_SOURCE_DIR ?= $(addsuffix /config, $(APP_SOURCE_DIR))

APP_CONFIG_H := "$(CONFIG_SOURCE_DIR)/da1458x_config_basic.h"
APP_CONFIG_H += "$(CONFIG_SOURCE_DIR)/da1458x_config_advanced.h"
APP_CONFIG_H += "$(CONFIG_SOURCE_DIR)/user_config.h"

BUILD_CONFIG ?=  # one of BUILD_CONFIG_TYPES below

BUILD_CONFIG_TYPES := da14531 
BUILD_CONFIG_TYPES += da14585 
BUILD_CONFIG_TYPES += da14586 

BUILD_OUTPUT_DIR := $(BUILD_CONFIG)

CC   ?= clang
OPT_FLAGS ?=-Oz
ENABLE_LTO ?=1

ifeq ($(ENABLE_LTO),1)
OPT_FLAGS += -flto	
endif

OBJCOPY :=llvm-objcopy
OBJSIZE  :=llvm-size

BUILD_GCC_OBJS:= $(SOURCEROOT)/crt_objects/crt0.o \
                 $(SOURCEROOT)/crt_objects/crti.o \
                 $(SOURCEROOT)/crt_objects/crtn.o

RM := rm -rf

# Here all the sources are included. The auto-generated makefile does this
# By creating makefiles for each subdirectory, and then including them all.
# But in our case, all these subdirectory makefiles are essentially identical,
# just with different source files. So, instead, we just pull in all the 
# source files here.

ELF_SRCS := 
OBJ_SRCS := 
ASM_SRCS := 
#C_SRCS :=
PROJECT_SRCS := 
S_UPPER_SRCS := 
O_SRCS := 
OBJS :=
SECONDARY_FLASH :=
SECONDARY_SIZE := 
ASM_DEPS := 
S_UPPER_DEPS := 
C_DEPS :=

################################################################################
# C Source Files
################################################################################

# Header include directories
# This order used to match eclipse 
C_INCLUDE_DIRS := \
$(SDKROOT)/app_modules/api \
$(SDKROOT)/platform/include/CMSIS/5.6.0/Include \
$(SDKROOT)/ble_stack/controller/em \
$(SDKROOT)/ble_stack/controller/llc \
$(SDKROOT)/ble_stack/controller/lld \
$(SDKROOT)/ble_stack/controller/llm \
$(SDKROOT)/ble_stack/ea/api \
$(SDKROOT)/ble_stack/em/api \
$(SDKROOT)/ble_stack/hci/api \
$(SDKROOT)/ble_stack/hci/src \
$(SDKROOT)/ble_stack/host/att \
$(SDKROOT)/ble_stack/host/att/attc \
$(SDKROOT)/ble_stack/host/att/attm \
$(SDKROOT)/ble_stack/host/att/atts \
$(SDKROOT)/ble_stack/host/gap \
$(SDKROOT)/ble_stack/host/gap/gapc \
$(SDKROOT)/ble_stack/host/gap/gapm \
$(SDKROOT)/ble_stack/host/gatt \
$(SDKROOT)/ble_stack/host/gatt/gattc \
$(SDKROOT)/ble_stack/host/gatt/gattm \
$(SDKROOT)/ble_stack/host/l2c/l2cc \
$(SDKROOT)/ble_stack/host/l2c/l2cm \
$(SDKROOT)/ble_stack/host/smp \
$(SDKROOT)/ble_stack/host/smp/smpc \
$(SDKROOT)/ble_stack/host/smp/smpm 

# Add BLE profile includes
# Top level profile directory
SDK_PROFILES := ble_stack/profiles
# Get the top level directory for each profile
SDK_PROFILE_DIRS += $(wildcard $(addprefix $(SDKROOT)/, $(SDK_PROFILES))/*/) 
# Some, but not all, top level profile directories have h files. Get only those with h files
SDK_PROFILE_DIRS_WITH_H_FILE := $(subst //,,$(foreach d,$(SDK_PROFILE_DIRS),$(dir $(wildcard  $d/*.h))))
# Get the api directory for each profile
SDK_PROFILE_API_DIRS += $(subst //,/,$(foreach d,$(SDK_PROFILE_DIRS),$(wildcard  $d/*/api)))
# Add profile directories to the include directories
C_INCLUDE_DIRS += $(addprefix $(SDKROOT)/, $(SDK_PROFILES))
C_INCLUDE_DIRS += $(sort $(SDK_PROFILE_DIRS_WITH_H_FILE) $(SDK_PROFILE_API_DIRS))

C_INCLUDE_DIRS += \
$(SDKROOT)/ble_stack/rwble \
$(SDKROOT)/ble_stack/rwble_hl \
$(SDKROOT)/common_project_files \
$(SDKROOT)/platform/arch \
$(SDKROOT)/platform/arch/boot \
$(SDKROOT)/platform/arch/boot/ARM \
$(SDKROOT)/platform/arch/boot/GCC \
$(SDKROOT)/platform/arch/boot/CLANG \
$(SDKROOT)/platform/arch/compiler \
$(SDKROOT)/platform/arch/compiler/ARM \
$(SDKROOT)/platform/arch/compiler/GCC \
$(SDKROOT)/platform/arch/ll \
$(SDKROOT)/platform/arch/main \
$(SDKROOT)/platform/core_modules/arch_console \
$(SDKROOT)/platform/core_modules/common/api \
$(SDKROOT)/platform/core_modules/crypto \
$(SDKROOT)/platform/core_modules/dbg/api \
$(SDKROOT)/platform/core_modules/gtl/api \
$(SDKROOT)/platform/core_modules/gtl/src \
$(SDKROOT)/platform/core_modules/h4tl/api \
$(SDKROOT)/platform/core_modules/ke/api \
$(SDKROOT)/platform/core_modules/ke/src \
$(SDKROOT)/platform/core_modules/nvds/api \
$(SDKROOT)/platform/core_modules/rf/api \
$(SDKROOT)/platform/core_modules/rwip/api \
$(GCC_LIB_DIR)/arm-none-eabi/include

# Add driver includes
# Top level driver directory
SDK_DRIVERS := platform/driver
# Each driver directory has an h file. Get each driver directory
SDK_DRIVER_DIRS = $(wildcard $(addprefix $(SDKROOT)/, $(SDK_DRIVERS))/*)
C_INCLUDE_DIRS += $(SDK_DRIVER_DIRS)

C_INCLUDE_DIRS += \
$(SDKROOT)/platform/include \
$(SDKROOT)/platform/system_library/include \
$(SOURCEROOT)/third_party/hash \
$(SOURCEROOT)/third_party/irng \
$(SOURCEROOT)/third_party/rand \
$(SDKROOT)/platform/utilities/otp_cs \
$(SDKROOT)/platform/utilities/otp_hdr \

# Add User includes
# Get each directory in source directory
USER_DIRS := $(wildcard $(addsuffix /, $(APP_SOURCE_DIR))*/) # Have issue when attempt to use APP_SOURCE_DIR directly, using addsuffix as a work around
# Some, but not all, directories have h files. Get only those with h files
USER_DIRS_WITH_H_FILE := $(subst //,,$(foreach d,$(USER_DIRS),$(dir $(wildcard  $d/*.h))))
USER_INCLUDES := $(sort $(USER_DIRS_WITH_H_FILE)) 
USER_INCLUDES += $(APP_SOURCE_DIR)
# Add user includes
C_INCLUDE_DIRS += $(USER_INCLUDES)

##################################################################
# Sources: .c and .S files 

# The Eclipse project uses virtual folders to organize SDK files into logical groupings. 
# Here we mimic those groupings with the corresponding "VIRTUAL_" variable. 
# The order these files are built affects the order they are added to the OBJS variable, which ultimately affects linking order.  
# We use these groupings to maintain consistency with Eclipse project and produce an identical binary to the Eclipse project

VIRTUAL_SDK_PROFILE_SRCS :=
VIRTUAL_SDK_DRIVER_SRCS :=
VIRTUAL_SDK_BOOT_SRCS := 
VIRTUAL_SDK_BLE_SRCS :=
VIRTUAL_SDK_ARCH_SRCS :=
VIRTUAL_SDK_APP_SRCS := 

# sdk_profiles
# Add additional profiles as necessary. Make sure for any profile you add, you add the corresponding app_module to VIRTUAL_SDK_APP_SUBDIRS below
VIRTUAL_SDK_PROFILE_SUBDIRS := ble_stack/profiles/bas/bass/src
VIRTUAL_SDK_PROFILE_SUBDIRS += ble_stack/profiles/dis/diss/src
VIRTUAL_SDK_PROFILE_SUBDIRS += ble_stack/profiles
VIRTUAL_SDK_PROFILE_SUBDIRS += ble_stack/profiles/prox/proxr/src
VIRTUAL_SDK_PROFILE_SUBDIRS += ble_stack/profiles/suota/suotar/src
# Get all .c files for the sdk_profiles virtrual folder
SDK_PROFILE_UNSORTED += $(wildcard $(addsuffix /*.c, $(addprefix $(SDKROOT)/, $(VIRTUAL_SDK_PROFILE_SUBDIRS))))
# SORTING: To maintain the same build/linking order with the Eclipse project build, we need to sort the files based on file name only (diregarding path)
# Here we remove the path to each file, sort the files, then re-apply to the path to each file maintaining sorted oreder
SDK_PROFILE_SORTED_WITHOUT_DIR := $(sort $(notdir $(SDK_PROFILE_UNSORTED)))
SDK_PROFILE_SORTED_BY_FILE_NAME := $(foreach src,$(SDK_PROFILE_SORTED_WITHOUT_DIR),$(filter %$(src),$(SDK_PROFILE_UNSORTED)))
VIRTUAL_SDK_PROFILE_SRCS += $(SDK_PROFILE_SORTED_BY_FILE_NAME)

# sdk_driver
# Add additional drivers as necessary
VIRTUAL_SDK_DRIVER_SUBDIRS := platform/driver/adc
VIRTUAL_SDK_DRIVER_SUBDIRS += platform/driver/battery
VIRTUAL_SDK_DRIVER_SUBDIRS += platform/driver/dma
VIRTUAL_SDK_DRIVER_SUBDIRS += platform/driver/gpio
VIRTUAL_SDK_DRIVER_SUBDIRS += platform/driver/hw_otpc
VIRTUAL_SDK_DRIVER_SUBDIRS += platform/driver/i2c
VIRTUAL_SDK_DRIVER_SUBDIRS += platform/driver/i2c_eeprom
VIRTUAL_SDK_DRIVER_SUBDIRS += platform/driver/spi
VIRTUAL_SDK_DRIVER_SUBDIRS += platform/driver/spi_flash
VIRTUAL_SDK_DRIVER_SUBDIRS += platform/driver/syscntl
VIRTUAL_SDK_DRIVER_SUBDIRS += platform/driver/trng
VIRTUAL_SDK_DRIVER_SUBDIRS += platform/driver/uart
VIRTUAL_SDK_DRIVER_SUBDIRS += platform/driver/wkupct_quadec
# Get all .c files for the sdk_driver virtrual folder
SDK_DRIVER_UNSORTED += $(wildcard $(addsuffix /*.c, $(addprefix $(SDKROOT)/, $(VIRTUAL_SDK_DRIVER_SUBDIRS))))
# See the SORTING comment above
SDK_DRIVER_SORTED_WITHOUT_DIR := $(sort $(notdir $(SDK_DRIVER_UNSORTED)))
SDK_DRIVER_SORTED_BY_FILE_NAME := $(foreach src,$(SDK_DRIVER_SORTED_WITHOUT_DIR),$(filter %$(src),$(SDK_DRIVER_UNSORTED)))
VIRTUAL_SDK_DRIVER_SRCS += $(SDK_DRIVER_SORTED_BY_FILE_NAME)

# sdk_boot 
#VIRTUAL_SDK_BOOT_SUBDIRS := platform/arch/boot/GCC 
#VIRTUAL_SDK_BOOT_SUBDIRS += platform/arch/boot 
VIRTUAL_SDK_BOOT_SUBDIRS = platform/arch/boot/CLANG
# Get all .c and .S files for the sdk_boot virtual folder 
SDK_BOOT_UNSORTED := $(wildcard $(addsuffix /*.c, $(addprefix $(SDKROOT)/, $(VIRTUAL_SDK_BOOT_SUBDIRS))))
SDK_BOOT_UNSORTED += $(wildcard $(addsuffix /*.S, $(addprefix $(SDKROOT)/, $(VIRTUAL_SDK_BOOT_SUBDIRS))))
# The handfault_handler.c and nmi_handler.c are located in the platform/arch/main along with a number of other .c files that we do not want in the sdk_boot grouping
# This filter takes only the handfault_handler.c and nmi_handler.c files from this directory. 
SDK_BOOT_UNSORTED += $(filter %hardfault_handler.c %nmi_handler.c,$(wildcard $(addsuffix /*.c, $(addprefix $(SDKROOT)/, platform/arch/main))))
# See the SORTING comment above
SDK_BOOT_SORTED_WITHOUT_DIR := $(sort $(notdir $(SDK_BOOT_UNSORTED)))
SDK_BOOT_SORTED_BY_FILE_NAME := $(foreach src,$(SDK_BOOT_SORTED_WITHOUT_DIR),$(filter %$(src),$(SDK_BOOT_UNSORTED)))
VIRTUAL_SDK_BOOT_SRCS += $(SDK_BOOT_SORTED_BY_FILE_NAME)

# sdk_ble
VIRTUAL_SDK_BLE_SUBDIRS := platform/core_modules/rf/src 
VIRTUAL_SDK_BLE_SUBDIRS += ble_stack/rwble 
VIRTUAL_SDK_BLE_SUBDIRS += platform/core_modules/rwip/src 
# Get all .c and files for the sdk_ble virtual folder 
SDK_BLE_UNSORTED += $(wildcard $(addsuffix /*.c, $(addprefix $(SDKROOT)/, $(VIRTUAL_SDK_BLE_SUBDIRS))))
# See the SORTING comment above
SDK_BLE_SORTED_WITHOUT_DIR := $(sort $(notdir $(SDK_BLE_UNSORTED)))
SDK_BLE_SORTED_BY_FILE_NAME := $(foreach src,$(SDK_BLE_SORTED_WITHOUT_DIR),$(filter %$(src),$(SDK_BLE_UNSORTED)))
VIRTUAL_SDK_BLE_SRCS += $(SDK_BLE_SORTED_BY_FILE_NAME)

# sdk_arch
VIRTUAL_SDK_ARCH_SUBDIRS += platform/core_modules/arch_console 
VIRTUAL_SDK_ARCH_SUBDIRS += platform/arch/main
VIRTUAL_SDK_ARCH_SUBDIRS += platform/core_modules/nvds/src 
VIRTUAL_SDK_ARCH_SUBDIRS += platform/utilities/otp_cs 
VIRTUAL_SDK_ARCH_SUBDIRS += platform/utilities/otp_hdr 
# Add additional thrid party libraries as necessary
THIRD_PARTY_SUBDIRS := third_party/rand
THIRD_PARTY_SUBDIRS += third_party/hash
# Get all .c files for the sdk_arch virtual folder 
SDK_ARCH_UNSORTED := $(wildcard $(addsuffix /*.c, $(addprefix $(SDKROOT)/, $(VIRTUAL_SDK_ARCH_SUBDIRS))))
SDK_ARCH_UNSORTED += $(wildcard $(addsuffix /*.c, $(addprefix $(SOURCEROOT)/, $(THIRD_PARTY_SUBDIRS))))
# See the SORTING comment above
SDK_ARCH_SORTED_WITHOUT_DIR := $(sort $(notdir $(SDK_ARCH_UNSORTED)))
SDK_ARCH_SORTED_BY_FILE_NAME := $(foreach src,$(SDK_ARCH_SORTED_WITHOUT_DIR),$(filter %$(src),$(SDK_ARCH_UNSORTED)))
# The handfault_handler.c and nmi_handler.c are part of the sdk_boot grouping but are incldued in SDK_ARCH_SORTED_BY_FILE_NAME. Here we filter them out of the sdk_arch grouping
VIRTUAL_SDK_ARCH_SRCS += $(filter-out %hardfault_handler.c %nmi_handler.c, $(SDK_ARCH_SORTED_BY_FILE_NAME))

# sdk_app
VIRTUAL_SDK_APP_SUBDIRS += app_modules/src/app_common
VIRTUAL_SDK_APP_SUBDIRS += app_modules/src/app_bond_db
VIRTUAL_SDK_APP_SUBDIRS += app_modules/src/app_default_hnd
VIRTUAL_SDK_APP_SUBDIRS += app_modules/src/app_easy
VIRTUAL_SDK_APP_SUBDIRS += app_modules/src/app_entry
VIRTUAL_SDK_APP_SUBDIRS += app_modules/src/app_sec
# For any additional ble profiles added, make sure you add the corresponding app_module here 
VIRTUAL_SDK_APP_SUBDIRS += app_modules/src/app_bass
VIRTUAL_SDK_APP_SUBDIRS += app_modules/src/app_diss
VIRTUAL_SDK_APP_SUBDIRS += app_modules/src/app_proxr
VIRTUAL_SDK_APP_SUBDIRS += app_modules/src/app_suotar
# Get all .c files for the sdk_app virtual folder 
SDK_APP_UNSORTED := $(wildcard $(addsuffix /*.c, $(addprefix $(SDKROOT)/, $(VIRTUAL_SDK_APP_SUBDIRS))))
# See the SORTING comment above
SDK_APP_SORTED_WITHOUT_DIR := $(sort $(notdir $(SDK_APP_UNSORTED)))
SDK_APP_SORTED_BY_FILE_NAME := $(foreach src,$(SDK_APP_SORTED_WITHOUT_DIR),$(filter %$(src),$(SDK_APP_UNSORTED)))
VIRTUAL_SDK_APP_SRCS += $(SDK_APP_SORTED_BY_FILE_NAME)

# Recursive wildcard:  
# https://stackoverflow.com/questions/2483182/recursive-wildcards-in-gnu-make/18258352#18258352
rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d)) 

# Go through all the files and subdirectories in APP_SOURCE_DIR and pull in all c source files.
APP_SRCS := $(strip $(call rwildcard,$(APP_SOURCE_DIR),*.c))

# Combine all source files
SDK_SRCS_COMBINED := \
$(VIRTUAL_SDK_PROFILE_SRCS) \
$(VIRTUAL_SDK_DRIVER_SRCS) \
$(VIRTUAL_SDK_BOOT_SRCS) \
$(VIRTUAL_SDK_BLE_SRCS) \
$(VIRTUAL_SDK_ARCH_SRCS) \
$(VIRTUAL_SDK_APP_SRCS)  

ifeq (da14531, $(BUILD_CONFIG))
#  remove 585/586 related files
SDK_SRCS := $(filter-out %586.S %586.c %585.c %58x.c, $(strip $(SDK_SRCS_COMBINED))) 
else
# remove 531 related files
SDK_SRCS := $(filter-out %531.S %531.c, $(strip  $(SDK_SRCS_COMBINED))) 
endif

##################################################################
# Flags

# Optimization flags
#OPT += -gdwarf-4 -Oz
#OPT += --flto
OPT += -gdwarf-4 $(OPT_FLAGS)

# Compilation flags
COMMON_FLAGS += -c --target=arm-none-eabi -march=armv6-m -mcpu=cortex-m0plus
COMMON_FLAGS += -mthumb

ifeq (da14531, $(BUILD_CONFIG))
DEVICE := __DA14531__
else ifeq (da14585, $(BUILD_CONFIG))
DEVICE := __DA14585__
else
DEVICE := __DA14586__
endif

CFLAGS += $(OPT) $(COMMON_FLAGS)
CFLAGS += -D$(DEVICE)
CFLAGS += -MMD -MP

ifeq (da14531, $(BUILD_CONFIG))
LDSCRIPT := ldscript_DA14531.lds
else
LDSCRIPT := ldscript_DA14585_586.lds
endif

# Linker Flags
LDFLAGS += --target=arm-unknown-none-eabi -march=armv6-m -mcpu=cortex-m0plus -nostdlib
LDFLAGS += $(OPT_FLAGS)
LDFLAGS += -gdwarf-4 -ffunction-sections -fdata-sections -Wall -Werror
LDFLAGS_1 += -T"$(LDSCRIPT)"
LDFLAGS_2 += -L"$(SDKROOT)/platform/system_library/output/Keil_5"
LDFLAGS_2 += -L"$(SDKROOT)/common_project_files/scatterfiles"
LDFLAGS_2 += -L"$(SDKROOT)/common_project_files/misc"
LDFLAGS_2 += -L"$(GCC_LIB_DIR)/arm-none-eabi/include"
LDFLAGS_2 += -L"$(GCC_LIB_DIR)/arm-none-eabi/lib/thumb/v6-m"
LDFLAGS_2 += -L"$(GCC_LIB_DIR)/lib/gcc/arm-none-eabi/7.3.1/thumb/v6-m"

################################################################################
# .S Assembly Source Files
################################################################################
# This section is analogous to the one below, but for assembly files.

# Generate a rule to build a .o and a .d file for every .S file. See the `C_DEPENDENCY_template` 
# definition and documentation below.
define S_UPPER_DEPENDENCY_template
$(2): $(1) 
	@echo 'Building file: $(1)'
	@echo 'Invoking: Cross ARM GNU Assembler'

# Create the appropriate folder inside the build folder, if it doesn't exist already.
	@mkdir -p $(dir $(2))

	@$(CC) $(CFLAGS) -x assembler-with-cpp \
	-MF"$(2:%.o=%.d)" \
	-MT"$(2)" \
	-c -o "$(2)" "$(1)"

	@echo 'Finished building: $(1)'
	@echo ' '

OBJS += $(2)
S_UPPER_DEPS += $(patsubst %.o,%.d,$(2))
endef

# This macro takes a c file as input and generates a rule for building it. Not only does
# the rule build the associated .o file (which is the target of the rule), but it also 
# builds a _makefile_ that lists all the _header_ dependencies of the .o file. 
# This file has the extension ".d". When the makefile is run for the first time, no .o
# files exist, so this rule gets run for every .c file and generates a .o and a .d file for each.
# On subsequent executions, the makefile imports all the .d files. This means that the .o
# files will rebuilt if _either_ their .c file _or any header dependency_ is changed.
#
# The input c file is passed in as $(1). The .o and .d files are generated from $(1). 
define C_DEPENDENCY_template
$(2): $(1) 
	@echo 'Building file: $(1)'
	@echo 'Invoking: Cross ARM C Compiler'

# Create the appropriate folder inside the build folder, if it doesn't exist already.
	@mkdir -p $(dir $(2))

	@$(CC) $(CFLAGS) $(addprefix -I, $(C_INCLUDE_DIRS)) \
	$(addprefix -include,$(APP_CONFIG_H)) \
	-std=gnu11 \
	-Wno-int-conversion -Wno-unused-variable \
	-MF"$(2:%.o=%.d)" \
	-MT"$(2)" \
	-c -o "$(2)" "$(1)"

	@echo 'Finished building: $(1)'
	@echo ' '

OBJS += $(2)
C_DEPS += $(patsubst %.o,%.d,$(2))
endef

# Convienent mechanisms to call above dependency templates
s_template=$(eval $(call S_UPPER_DEPENDENCY_template,$(1),$(subst $(SOURCEROOT)/,$(BUILD_OUTPUT_DIR)/,$(patsubst %.S,%.o,$(1)))))
c_sdk_template=$(eval $(call C_DEPENDENCY_template,$(1),$(subst $(SOURCEROOT)/,$(BUILD_OUTPUT_DIR)/,$(patsubst %.c,%.o,$(1))))) 
# This is used to specify a different output folder for artifacts produced from building user sources
c_user_src_template=$(eval $(call C_DEPENDENCY_template,$(1),$(subst $(addsuffix /, $(APP_SOURCE_DIR)),$(BUILD_OUTPUT_DIR)/user/,$(patsubst %.c,%.o,$(1))))) 

# Build app sources
$(foreach src, $(APP_SRCS),$(call c_user_src_template,$(src)))

# Build SDK sources
# SDK_SRCS contains both .c and .S files. Having a list with both .c and .S files allows us link in the same order as the Eclipse project and produce an identical binary. 
# Here we must call the appropriate template based on the file type.
$(foreach src, $(SDK_SRCS), $(if $(strip $(filter %.S,$(src)) ),$(call s_template,$(src)),$(call c_sdk_template,$(src))))


################################################################################
# Import Dependencies
################################################################################

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

USER_OBJS :=

ifeq (da14531, $(BUILD_CONFIG))
#LIBS := -l:da14531.lib
LIBS := -l:libgcc.a -l:libg_nano.a -l:libnosys.a -l:da14531.lib
else
LIBS := -l:da14585_586.lib
endif

# Templates for cleaning 
define clean_extensions_template
	@find . -wholename */$(strip $(1))/*.elf -type f -delete
	@find . -wholename */$(strip $(1))/*.lds -type f -delete
	@find . -wholename */$(strip $(1))/*.map -type f -delete
	@find . -wholename */$(strip $(1))/*.img -type f -delete
	@find . -wholename */$(strip $(1))/*.bin -type f -delete
	@find . -wholename */$(strip $(1))/*.hex -type f -delete
	@find . -wholename */$(strip $(1))/*.prev -type f -delete
endef

define clean_template

	@echo 'Cleaning $(strip $(1))'
	$(call clean_extensions_template, $(1))
	-@$(RM) \
	$(addprefix $(strip $(1)),$(OBJS)) \
	$(SECONDARY_FLASH) \
	$(SECONDARY_SIZE) \
	$(addprefix $(strip $(1)),$(ASM_DEPS)) \
	$(addprefix $(strip $(1)),$(S_UPPER_DEPS)) \
	$(addprefix $(strip $(1)),$(C_DEPS)) 
	@echo 'Finished cleaning $(strip $(1))'
	@echo ' '

endef

################################################################################
# Rules
################################################################################

# Allow extra symbols to be defined before compilation. Optional file.
-include ../../build_utils/clang/makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../../build_utils/clang/makefile.defs) \
$(wildcard ../../build_utils/clang/makefile.init) \
$(wildcard ../../build_utils/clang/makefile.targets) \

BUILD_ARTIFACT_NAME ?= pxp_reporter
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

SECONDARY_FLASH += \
$(BUILD_ARTIFACT_NAME).hex \

SECONDARY_BIN += \
$(BUILD_ARTIFACT_NAME).bin \

SECONDARY_SIZE += \
$(BUILD_ARTIFACT_NAME).siz \


all: pre-build main-build

# Main-build Target
main-build: check-build-config $(BUILD_ARTIFACT) secondary-outputs 
 
# Tool invocations

OBJS_ = user_proxr.o

# Generate .elf 
$(BUILD_ARTIFACT): $(OBJS) $(USER_OBJS) $(OPTIONAL_TOOL_DEPS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross ARM C Linker'
	$(CC) $(LDFLAGS) $(OBJS) $(BUILD_GCC_OBJS) $(LDFLAGS_2) $(LDFLAGS_1) -Wl,--gc-sections,-Map,$(BUILD_OUTPUT_DIR)/$(BUILD_ARTIFACT_NAME).map -o "$(BUILD_OUTPUT_DIR)/$@" $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Generate .hex 
$(SECONDARY_FLASH): $(BUILD_ARTIFACT) $(OPTIONAL_TOOL_DEPS)
	@echo 'Invoking: Cross LLVM Create Flash Image'
	@$(OBJCOPY) -O ihex "$(BUILD_OUTPUT_DIR)/$<"  "$(BUILD_OUTPUT_DIR)/$@" 
	@echo 'Finished building: $@'
	@echo ' '

# Generate .bin 
$(SECONDARY_BIN): $(BUILD_ARTIFACT) $(OPTIONAL_TOOL_DEPS)
	@echo 'Invoking: Cross LLVM Create Flash Image'
	@$(OBJCOPY) -O binary "$(BUILD_OUTPUT_DIR)/$<"  "$(BUILD_OUTPUT_DIR)/$@" 
	@echo 'Finished building: $@'
	@echo ' '


# Print size
$(SECONDARY_SIZE): $(BUILD_ARTIFACT) $(OPTIONAL_TOOL_DEPS)
	@echo 'Invoking: Cross LLVM Print Size'
	@$(OBJSIZE) --format=berkeley "$(BUILD_OUTPUT_DIR)/$<"
	@echo 'Finished building: $@'
	@echo ' '


# make clean will clean ALL build configurations {da14531, da14585, da14586}
# setting BUILD_CONFIG when calling clean will clean only that configuration
# For example:
# make BUILD_CONFIG=da14531 clean 
# will clean only the da14531 build
clean:
ifeq (,$(BUILD_CONFIG))
	@echo 'Cleaning all build configuration outputs'
	@echo ' '
	$(foreach config,$(BUILD_CONFIG_TYPES),$(call clean_template,$(config)))
	@echo 'Finished cleaning all build configurations'
	@echo ' '

else
	@echo 'Cleaning $(BUILD_CONFIG) build ouputs'
	$(call clean_extensions_template, $(BUILD_CONFIG))
	-@$(RM) $(OBJS) $(SECONDARY_FLASH) $(SECONDARY_SIZE) $(ASM_DEPS) $(S_UPPER_DEPS) $(C_DEPS)
	@echo 'Finished cleaning $(BUILD_CONFIG)'
	@echo ' '

endif

# Check that a valid build configuration has been selected
check-build-config:
ifeq (,$(findstring $(BUILD_CONFIG), $(BUILD_CONFIG_TYPES)))
	$(error Invalid build configuration. Please set BUILD_CONFIG to one of {$(BUILD_CONFIG_TYPES)})
endif

# Generate linker scripts TODO need to change LDSCRIPT_FILES for 14585/86
pre-build: check-build-config
	-@echo 'Generate linker scripts.'
	-@echo 'SDKROOT: $(SDKROOT)'
	-@echo 'CONFIG_SOURCE_DIR: $(CONFIG_SOURCE_DIR)'
	-@echo 'DEVICE: "-D$(DEVICE)"'
	-@echo 'LDSCRIPT: $(LDSCRIPT)'
	-@echo 'CC: $(CC)'
	-@mkdir -p $(BUILD_OUTPUT_DIR)
	@make generate_ldscripts PRE_BUILD_EXTRA_DEFS="-D$(DEVICE)" \
	CC="$(CC)" \
	LDSCRIPT_INCLUDE_DIR="$(CONFIG_SOURCE_DIR) $(SDKROOT)/common_project_files" \
	LDSCRIPT_PATH="../../build_utils/clang/clang_ldscripts" \
	LDSCRIPT_FILES="$(LDSCRIPT)"
	-@echo ' '


secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_SIZE) $(SECONDARY_BIN)

.PHONY: all clean dependents main-build pre-build post-build

# Define custom build targets. For this application, this imports a rule to build the linker scripts.
-include ../../build_utils/clang/makefile.targets
