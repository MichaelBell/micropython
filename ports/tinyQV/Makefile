include ../../py/mkenv.mk

CROSS = 1

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# MicroPython feature configurations
MICROPY_ROM_TEXT_COMPRESSION ?= 1

FROZEN_MANIFEST = manifest.py

# include py core make definitions
include $(TOP)/py/py.mk
include $(TOP)/extmod/extmod.mk

ifeq ($(CROSS), 1)
RISCV_TOOLCHAIN ?= /opt/tinyQV

CC = $(RISCV_TOOLCHAIN)/bin/riscv32-unknown-elf-gcc
AS = $(RISCV_TOOLCHAIN)/bin/riscv32-unknown-elf-as
AR = $(RISCV_TOOLCHAIN)/bin/riscv32-unknown-elf-ar
LD = $(RISCV_TOOLCHAIN)/bin/riscv32-unknown-elf-gcc
OBJCOPY = $(RISCV_TOOLCHAIN)/bin/riscv32-unknown-elf-objcopy
TINYQV_SDK = ../../../tinyQV-sdk
endif

INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)

ifeq ($(CROSS), 1)
INC += -I$(TINYQV_SDK)
DFU = $(TOP)/tools/dfu.py
PYDFU = $(TOP)/tools/pydfu.py
ARCH = rv32ec_zcb_zicond
CFLAGS_RV32E = -march=$(ARCH) -mabi=ilp32e -mno-strict-align -nostartfiles -ffreestanding -flto
CFLAGS += $(INC) -Wall -Werror -std=gnu99 -nostdlib $(CFLAGS_RV32E) $(COPT)
LDFLAGS += -march=$(ARCH) -mabi=ilp32e -flto=auto -ffreestanding -nostartfiles --specs=nano.specs -Wl,-Bstatic,-T,memmap,--gc-sections
else
UNAME_S := $(shell uname -s)
LD = $(CC)
CFLAGS += $(INC) -Wall -Werror -Wdouble-promotion -Wfloat-conversion -std=c99 $(COPT)
ifeq ($(UNAME_S),Linux)
LDFLAGS += -Wl,-Map=$@.map,--cref -Wl,--gc-sections
else ifeq ($(UNAME_S),Darwin)
LDFLAGS += -Wl,-map,$@.map -Wl,-dead_strip
endif
endif

# Tune for Debugging or Optimization
CFLAGS += -g  # always include debug info in the ELF
ifeq ($(DEBUG), 1)
CFLAGS += -O0
else
CFLAGS += -O2 -DNDEBUG
CFLAGS += -fdata-sections -ffunction-sections
endif

CFLAGS += -DLFS2_NO_MALLOC -DLFS2_NO_DEBUG -DLFS2_NO_WARN -DLFS2_NO_ERROR -DLFS2_NO_ASSERT

# Flags for optional C++ source code
CXXFLAGS += $(filter-out -std=c99,$(CFLAGS))

LIBS =

SRC_C = \
	main.c \
	fatfs_port.c \
	uart_core.c \
	mphalport.c \
	shared/libc/printf.c \
	shared/readline/readline.c \
	shared/runtime/pyexec.c \
	shared/runtime/stdout_helpers.c \
	shared/timeutils/timeutils.c \
	lib/littlefs/lfs2.c \
	lib/littlefs/lfs2_util.c \
	lib/oofatfs/ff.c \
	lib/oofatfs/ffunicode.c \

ifeq ($(CROSS), 1)
SRC_C += shared/libc/string0.c
endif

SRC_QSTR += shared/readline/readline.c shared/runtime/pyexec.c main.c

OBJ += $(PY_O)
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_CXX:.cpp=.o))

ifeq ($(CROSS), 1)
all: $(BUILD)/firmware.bin
else
all: $(BUILD)/firmware.elf
endif

$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(LD) $(LDFLAGS) -o $@ $^ $(LIBS) $(TINYQV_SDK)/start.o $(TINYQV_SDK)/tinyQV.a $(RISCV_TOOLCHAIN)/riscv32-unknown-elf/lib/libc.a $(RISCV_TOOLCHAIN)/riscv32-unknown-elf/lib/libm.a $(RISCV_TOOLCHAIN)/lib/gcc/riscv32-unknown-elf/*/libgcc.a
	$(Q)$(SIZE) $@

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(OBJCOPY) $< -O binary $@

deploy: $(BUILD)/firmware.bin
	$(ECHO) "Executing $< on the board"
	./run_prog.sh $<

include $(TOP)/py/mkrules.mk
