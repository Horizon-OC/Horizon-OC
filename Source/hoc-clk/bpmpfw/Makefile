ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif
include $(DEVKITARM)/base_rules

TARGET     := bpmpfw
BUILDDIR   := build
OUTPUTDIR  := out
SOURCEDIR  := source

OBJS := $(BUILDDIR)/start.o $(BUILDDIR)/main.o

ARCH   := -march=armv4t -mtune=arm7tdmi -mthumb-interwork
CXXFLAGS := $(ARCH) -O2 -g -ffreestanding -nostdlib -fno-exceptions -fno-rtti \
            -fno-unwind-tables -fno-threadsafe-statics -ffunction-sections -fdata-sections \
            -fomit-frame-pointer -std=gnu++20 -Wall -Wextra $(CUSTOMDEFINES)
ASFLAGS := $(ARCH) -g
LDFLAGS := $(ARCH) -nostartfiles -nodefaultlibs -Wl,--nmagic,--gc-sections \
           -T $(SOURCEDIR)/../bpmpfw.ld -lgcc

BIN_TARGET := $(OUTPUTDIR)/$(TARGET).bin

.PHONY: all clean

all: $(BIN_TARGET)

$(BIN_TARGET): $(BUILDDIR)/$(TARGET).elf
	@mkdir -p $(OUTPUTDIR)
	$(OBJCOPY) -S -O binary $< $@

$(BUILDDIR)/$(TARGET).elf: $(OBJS)
	@$(CXX) $(LDFLAGS) $(OBJS) -o $@

$(BUILDDIR)/%.o: $(SOURCEDIR)/%.cpp | $(BUILDDIR)
	@$(CXX) $(CXXFLAGS) -I$(SOURCEDIR) -c $< -o $@

$(BUILDDIR)/%.o: $(SOURCEDIR)/%.s | $(BUILDDIR)
	@$(CXX) $(ASFLAGS) -I$(SOURCEDIR) -c $< -o $@

$(BUILDDIR):
	@mkdir -p $(BUILDDIR)

clean:
	@rm -rf $(BUILDDIR) $(OUTPUTDIR)
