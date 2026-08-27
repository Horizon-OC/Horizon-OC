ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif
include $(DEVKITARM)/base_rules

TARGET     := bpmpfw
BUILDDIR   := build
OUTPUTDIR  := out
SOURCEDIR  := source

LIBCDIR := $(SOURCEDIR)/libc

CPP_SOURCES := $(wildcard $(SOURCEDIR)/*.cpp)
C_SOURCES   := $(wildcard $(LIBCDIR)/src/*.c)
S_SOURCES   := $(wildcard $(SOURCEDIR)/*.s)

OBJS := $(patsubst $(SOURCEDIR)/%.cpp,$(BUILDDIR)/%.o,$(CPP_SOURCES)) \
        $(patsubst $(LIBCDIR)/src/%.c,$(BUILDDIR)/libc/%.o,$(C_SOURCES)) \
        $(patsubst $(SOURCEDIR)/%.s,$(BUILDDIR)/%.o,$(S_SOURCES))

ARCH     := -march=armv4t -mtune=arm7tdmi -mthumb-interwork
COMMONDIR := ../common/include
INCLUDE  := -I$(SOURCEDIR) -I$(LIBCDIR)/include -I$(LIBCDIR)/include/fslc -I$(COMMONDIR)
CFLAGS   := $(ARCH) -O2 -g -ffreestanding -nostdlib -ffunction-sections -fdata-sections \
            -fomit-frame-pointer -std=gnu11 -Wall -Wextra $(CUSTOMDEFINES)
CXXFLAGS := $(ARCH) -O2 -g -ffreestanding -nostdlib -fno-exceptions -fno-rtti \
            -fno-unwind-tables -fno-threadsafe-statics -ffunction-sections -fdata-sections \
            -fomit-frame-pointer -std=gnu++20 -Wall -Wextra $(CUSTOMDEFINES)
ASFLAGS  := $(ARCH) -g
LDFLAGS  := $(ARCH) -nostartfiles -nodefaultlibs -Wl,--nmagic,--gc-sections \
            -T $(SOURCEDIR)/../bpmpfw.ld
LDLIBS   := -lgcc

BIN_TARGET := $(OUTPUTDIR)/$(TARGET).bin

.PHONY: all clean

all: $(BIN_TARGET)

$(BIN_TARGET): $(BUILDDIR)/$(TARGET).elf
	@mkdir -p $(OUTPUTDIR)
	@echo "bpmpfw.bin"
	@$(OBJCOPY) -S -O binary $< $@

$(BUILDDIR)/$(TARGET).elf: $(OBJS)
	@echo "linking $(TARGET).elf"
	@$(CXX) $(LDFLAGS) $(OBJS) $(LDLIBS) -o $@

$(BUILDDIR)/%.o: $(SOURCEDIR)/%.cpp | $(BUILDDIR)
	@echo "$(notdir $<)"
	@$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

$(BUILDDIR)/libc/%.o: $(LIBCDIR)/src/%.c | $(BUILDDIR)/libc
	@echo "$(notdir $<)"
	@$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(BUILDDIR)/%.o: $(SOURCEDIR)/%.s | $(BUILDDIR)
	@echo "$(notdir $<)"
	@$(CXX) $(ASFLAGS) $(INCLUDE) -c $< -o $@

$(BUILDDIR):
	@mkdir -p $(BUILDDIR)

$(BUILDDIR)/libc: | $(BUILDDIR)
	@mkdir -p $(BUILDDIR)/libc

clean:
	@rm -rf $(BUILDDIR) $(OUTPUTDIR)
