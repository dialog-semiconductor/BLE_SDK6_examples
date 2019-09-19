################################################################################
#
# Copyright (C) 2017 Dialog Semiconductor.
# This computer program includes Confidential, Proprietary Information
# of Dialog Semiconductor. All Rights Reserved.
#
# <bluetooth.support@diasemi.com> and contributors.
#
################################################################################

CC=gcc

STATIC_BUILD?=y

# verbosity switch
V?=0

ifeq ($(STATIC_BUILD),y)
	LDFLAGS+=-static
endif

ifeq ($(V),0)
	V_CC = @echo "  CC    " $@;
	V_LINK = @echo "  LINK  " $@;
	V_CLEAN = @echo "  CLEAN ";
	V_CLEAN_TEMP_FILES = @echo "  CLEAN_TEMP_FILES ";
	V_STRIP = @echo "  STRIP " $@;
else
	V_OPT = '-v'
endif

CFLAGS+=-std=gnu99 -Wall -O2 -Wl,-Map,$@.map

ifeq ($(V),2)
	CFLAGS+=--verbose --save-temps -fverbose-asm
	LDFLAGS+=-Wl,--verbose
endif

vpath %.c ../../../third_party/crc32
vpath %.c ..

EXEC=mkimage.exe
OBJS=crc32.o mkimage.o sw_aes.o

# how to compile C files
%.o : %.c
	$(V_CC)$(CC) $(CFLAGS) -c $< -o $@

all: $(EXEC)

$(EXEC): $(OBJS)
	$(V_LINK)$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)
	$(V_STRIP)strip -s $@
	$(V_CLEAN_TEMP_FILES)rm -f $(OBJS)
	
clean:
	$(V_CLEAN)rm -f $(V_OPT) $(EXEC) *.[ois]
