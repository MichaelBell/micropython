include ../../py/mkenv.mk

CROSS = 1

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# MicroPython feature configurations
MICROPY_ROM_TEXT_COMPRESSION ?= 1

# include py core make definitions
include $(TOP)/py/py.mk
include $(TOP)/extmod/extmod.mk

ifeq ($(CROSS), 1)
RISCV_TOOLCHAIN ?= /opt/nanoV

CC = $(RISCV_TOOLCHAIN)/bin/riscv32-unknown-elf-gcc
AS = $(RISCV_TOOLCHAIN)/bin/riscv32-unknown-elf-as
AR = $(RISCV_TOOLCHAIN)/bin/riscv32-unknown-elf-ar
LD = $(RISCV_TOOLCHAIN)/bin/riscv32-unknown-elf-gcc
OBJCOPY = $(RISCV_TOOLCHAIN)/bin/riscv32-unknown-elf-objcopy
NANOV_SDK = ../../../nanoV-sdk
endif

INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)

ifeq ($(CROSS), 1)
INC += -I$(NANOV_SDK)
DFU = $(TOP)/tools/dfu.py
PYDFU = $(TOP)/tools/pydfu.py
ARCH = rv32e
CFLAGS_RV32E = -march=$(ARCH) -mabi=ilp32e -mno-strict-align -nostartfiles -ffreestanding -flto
CFLAGS += $(INC) -Wall -Werror -std=c99 -nostdlib $(CFLAGS_RV32E) $(COPT)
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

CSUPEROPT = -Os # save some code space

# Tune for Debugging or Optimization
CFLAGS += -g  # always include debug info in the ELF
ifeq ($(DEBUG), 1)
CFLAGS += -O0
else
CFLAGS += -Os -DNDEBUG
CFLAGS += -fdata-sections -ffunction-sections
endif

# Flags for optional C++ source code
CXXFLAGS += $(filter-out -std=c99,$(CFLAGS))

LIBS =

SRC_C = \
	main.c \
	uart_core.c \
	shared/libc/printf.c \
	shared/readline/readline.c \
	shared/runtime/pyexec.c \
	shared/runtime/stdout_helpers.c \
	$(BUILD)/_frozen_mpy.c \

ifeq ($(CROSS), 1)
SRC_C += shared/libc/string0.c
endif

SRC_QSTR += shared/readline/readline.c shared/runtime/pyexec.c

OBJ += $(PY_O)
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_CXX:.cpp=.o))

ifeq ($(CROSS), 1)
all: $(BUILD)/firmware.bin
else
all: $(BUILD)/firmware.elf
endif

$(BUILD)/_frozen_mpy.c: $(TOP)/tests/frozen/frozentest.mpy $(BUILD)/genhdr/qstrdefs.generated.h
	$(ECHO) "MISC freezing bytecode"
	$(Q)$(TOP)/tools/mpy-tool.py -f -q $(BUILD)/genhdr/qstrdefs.preprocessed.h -mlongint-impl=none $< > $@

$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(LD) $(LDFLAGS) -o $@ $^ $(LIBS) $(NANOV_SDK)/start.o $(RISCV_TOOLCHAIN)/riscv32-unknown-elf/lib/libc.a $(NANOV_SDK)/nanoV.a $(RISCV_TOOLCHAIN)/lib/gcc/riscv32-unknown-elf/*/libgcc.a 
	$(Q)$(SIZE) $@

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(OBJCOPY) $< -O binary $(BUILD)/tmp.rbin
	python3 $(NANOV_SDK)/reverse_bits.py $(BUILD)/tmp.rbin $@

deploy: $(BUILD)/firmware.bin
	$(ECHO) "Execulting $< on the board"
	run_nano_fram $<

include $(TOP)/py/mkrules.mk
