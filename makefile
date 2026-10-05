# 项目编译脚本 单文件版
# 项目名称
PROJECT := NTailOS

# 定义路径
PROJECT_ROOT := $(CURDIR)
SCRIPTS_DIR := scripts
SOURCE_DIR := source
INCLUDE_DIR := $(SOURCE_DIR) include
BUILD_DIR := build
LIB_DIR := $(BUILD_DIR)/lib

# 目标
TARGET_NAME := $(PROJECT).elf
TARGET := $(BUILD_DIR)/$(TARGET_NAME)

# 模块
DRIVERS := serial apic # vga timer
# FS := fat32

SUB_MODULES := boot core cpu interrupt clib mem
SUB_MODULES += drivers $(addprefix drivers/,$(DRIVERS))
# SUB_MODULES += fs $(addprefix fs/,$(FS))

# 定义工具链
CROSS_COMPAIR_PATH := /home/linuxbrew/.linuxbrew/bin

CC  := $(CROSS_COMPAIR_PATH)/x86_64-elf-gcc
CXX := $(CROSS_COMPAIR_PATH)/x86_64-elf-g++
CPP := $(CROSS_COMPAIR_PATH)/x86_64-elf-cpp
AS  := nasm
AR  := $(CROSS_COMPAIR_PATH)/x86_64-elf-ar
LD  := $(CROSS_COMPAIR_PATH)/x86_64-elf-ld

RM    := rm -rf
MKDIR := @mkdir -p

# 调试/发布模式
MODE ?= debug

# 构建标志
DEPFLAGS := -MMD -MP
  # 语言标准和ABI
CFLAGS := -std=gnu23 -m64 -mcmodel=kernel -masm=intel
  # 代码生成
CFLAGS += -ffreestanding -fno-stack-protector -fno-pic -fno-exceptions -fno-asynchronous-unwind-tables
  # 优化选项
CFLAGS += -mpopcnt -flto
  # 警告
CFLAGS += -Wall -Winline
  # 调试信息
CFLAGS += -g
  # 输出
CFLAGS += -c $(DEPFLAGS) -save-temps #-flto-report
  # 包含
CFLAGS += $(addprefix -I ,$(INCLUDE_DIR)) -include compiler.h

ASMFLAGS := -f elf64 -g -F dwarf

ifeq ($(MODE),debug)
CFLAGS    += -Og -DDEBUG
ASMFLAGS  +=
QEMUFLAGS += -s -S
else
CFLAGS    += -O2 -DNDEBUG
ASMFLAGS  +=
QEMUFLAGS +=
endif

CXXFLAGS := -std=gnu++23 $(filter-out -std=gnu23,$(CFLAGS))
CPPFLAGS := $(addprefix -I ,$(INCLUDE_DIR)) -E -P $(DEPFLAGS)
ARFLAGS  := rcs
LDFLAGS  := $(filter-out -c $(DEPFLAGS),$(CFLAGS)) -nostdlib -static -no-pie -Wl,--build-id=none # -Wl,-Map=$(BUILD_DIR)/output.map

LD_SRCS   :=lds/kernel.LD
C_SRCS    :=$(foreach module,$(SUB_MODULES), $(wildcard $(SOURCE_DIR)/$(module)/*.c))
ASM_SRCS  :=$(foreach module,$(SUB_MODULES), $(wildcard $(SOURCE_DIR)/$(module)/*.asm))
ASM_PSRCS :=$(foreach module,$(SUB_MODULES), $(wildcard $(SOURCE_DIR)/$(module)/*.ASM))

LDS  := $(BUILD_DIR)/$(LD_SRCS:.LD=.ld)
OBJS := $(subst $(SOURCE_DIR),$(BUILD_DIR),$(C_SRCS:.c=.o) $(ASM_SRCS:.asm=.o) $(ASM_PSRCS:.ASM=.o))
DEPS := $(OBJS:.o=.d) $(LDS:.ld=.d)

# 导入功能定义
-include $(SCRIPTS_DIR)/func.mk

# 声明虚目标
.PHONY: prepare print
.PHONY: all $(ALLS)
.PHONY: clean $(CLEANS)

all: prepare $(TARGET)

prepare: $(sort $(dir $(DEPS)))

$(foreach dir,$(sort $(dir $(DEPS))),$(eval $(call check_and_create_folder,$(dir))))

-include $(DEPS)

# $(TARGET): CFLAGS := $(filter-out -mcmodel=kernel,$(CFLAGS))

$(TARGET): $(OBJS) $(LDS)
	$(CC) $(LDFLAGS) -Wl,-T,$(LDS) -Wl,--start-group $(OBJS) -Wl,--end-group -o $@

$(LDS): $(LD_SRCS) lds/lds.h.in
	sed 's/# *define VPAGE_SIZE *\S*/#define VPAGE_SIZE $(call get_size,$(SOURCE_DIR)/mem/buddy.h,PageFrame)/' lds/lds.h.in > $(BUILD_DIR)/lds/lds.h
	$(CPP) $(CPPFLAGS) -DLD_FILE -include $(BUILD_DIR)/lds/lds.h $< -o $@

$(BUILD_DIR)/%_early.o: CFLAGS := $(filter-out -mcmodel=kernel,$(CFLAGS)) -mcmodel=large

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.asm
	$(AS) $(ASMFLAGS) $< -o $@

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.ASM
	$(CPP) $(CPPFLAGS) $< -o $(BUILD_DIR)/$*.asm
	$(AS) $(ASMFLAGS) $(BUILD_DIR)/$*.asm -o $@

clean:
	$(RM) $(BUILD_DIR)/*

test_size:
	echo $(call get_size,mem/buddy.h,PageFrame)

include $(SCRIPTS_DIR)/run.mk