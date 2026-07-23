#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>/devkitpro")
endif

TOPDIR ?= $(CURDIR)
include $(DEVKITPRO)/libnx/switch_rules

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# DATA is a list of directories containing data files
# INCLUDES is a list of directories containing header files
# EXEFS_SRC is the optional input directory containing data copied into exefs, if anything this normally should only contain "main.npdm".
#---------------------------------------------------------------------------------
TARGET		:=	hoc-clk
CONFIG_DIR	:=	horizon-oc
BUILD		:=	build
OUTDIR		:=	out
RESOURCES	:=	res
SOURCES		:=	src src/hos src/i2c src/ipc src/board src/display src/tsensor src/mgr src/file src/soc src/mapping src/pwr src/util
DATA		:=	data
INCLUDES	:=	../common/include src/hos src/soc src/i2c src/util src/pwr src/ipc
EXEFS_SRC	:=	exefs_src
LIBNAMES	:=	minIni
# major minor patch
TARGET_VERSION	:= 2.5.0
KIP_VERSION :=  250
CUST_REV    :=  6

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
DEFINES	:=	-DDISABLE_IPC -DKIP_VERSION=$(KIP_VERSION) -DCUST_REV=$(CUST_REV) -DTARGET="\"$(TARGET)\"" -DTARGET_VERSION="\"$(TARGET_VERSION)\"" -DCONFIG_DIR="\"$(CONFIG_DIR)\""

ARCH	:=	-march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE

CFLAGS	:=	-g -Wall -Os \
            -ffast-math -ffunction-sections -fdata-sections -fno-asynchronous-unwind-tables -fno-unwind-tables \
            $(ARCH) $(DEFINES)

CFLAGS	+=	$(INCLUDE) -D__SWITCH__

CXXFLAGS    := $(CFLAGS) -fno-rtti -fno-exceptions -std=gnu++23

ASFLAGS	:=	-g $(ARCH)
LDFLAGS    =    -specs=$(DEVKITPRO)/libnx/switch.specs -g $(ARCH) -Wl,-Map,$(notdir $*.map) -Wl,-wrap,__cxa_throw -Wl,-wrap,_Unwind_Resume -Wl,-wrap,__gxx_personality_v0

LIBS	:= $(foreach lib,$(LIBNAMES),-l$(lib)) -lnx

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:= $(PORTLIBS) $(LIBNX) $(foreach lib,$(LIBNAMES),$(TOPDIR)/lib/$(lib))

#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(OUTDIR)/$(TARGET)
export TOPDIR	:=	$(CURDIR)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
			$(foreach dir,$(DATA),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
#---------------------------------------------------------------------------------
	export LD	:=	$(CC)
#---------------------------------------------------------------------------------
else
#---------------------------------------------------------------------------------
	export LD	:=	$(CXX)
#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------

export OFILES	:=	$(addsuffix .o,$(BINFILES)) \
			$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
			$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
			-I$(CURDIR)/$(BUILD)

export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)

export BUILD_EXEFS_SRC := $(TOPDIR)/$(EXEFS_SRC)

export APP_JSON := $(TOPDIR)/perms.json

.PHONY: $(BUILD) clean clean-libs clean-build all libs

#---------------------------------------------------------------------------------
all: $(BUILD)

libs:
	@$(foreach lib,$(LIBNAMES),$(MAKE) --no-print-directory -C $(TOPDIR)/lib/$(lib) && ) true

$(LIBNAMES):
	@echo $@

$(BUILD): libs
	@[ -d $@ ] || mkdir -p $@
	@[ -d $(OUTDIR) ] || mkdir -p $(OUTDIR)
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
clean-libs:
	@echo clean libs $(LIBNAMES) ...
	@$(foreach lib,$(LIBNAMES),$(MAKE) -C $(TOPDIR)/lib/$(lib) clean && ) true

clean-build:
	@echo clean build ...
	@rm -fr $(BUILD) $(TARGET).kip $(TARGET).nsp $(TARGET).npdm $(TARGET).nso $(TARGET).elf $(OUTDIR)

clean: clean-libs clean-build


#---------------------------------------------------------------------------------
else
.PHONY:	all $(LIBFILES)

LIBFILES	:= $(foreach lib,$(LIBNAMES),$(TOPDIR)/lib/$(lib)/lib/lib$(lib).a)
DEPENDS		:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------

all: $(OUTPUT).nsp

$(OUTPUT).nsp: $(OUTPUT).nso $(OUTPUT).npdm

$(OUTPUT).elf: $(OFILES) $(LIBFILES)

#---------------------------------------------------------------------------------
# you need a rule like this for each extension you use as binary data
#---------------------------------------------------------------------------------
%.bin.o	:	%.bin
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

-include $(DEPENDS)

#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------
