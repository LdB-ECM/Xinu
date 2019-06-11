##############################################################################
#                                                                            #
#  Makefile for the Embedded Xinu operating system.                          #
#                                                                            #
#  Please see README.compiling.                                              #
#                                                                            #
##############################################################################
# If cross compiling from windows use native GNU-Make 4.2.1
# https://sourceforge.net/projects/ezwinports/files/
# download "make-4.2.1-without-guile-w32-bin.zip" and set it on the enviroment path
# There is no need to install cygwin or any of that sort of rubbish

ifeq ($(OS), Windows_NT)
	#WINDOWS USE THESE DEFINITIONS
	RM = -del /q /f
	CP = copy
	SLASH = \\
else
	#LINUX USE THESE DEFINITIONS
	RM = rm -f
	CP = cp -p
	SLASH = /
endif
#
# Name of the platform to build Embedded Xinu for.  It must be the name of a
# directory in compile/platforms/.  The value below specifies the default
# platform for this Embedded Xinu distribution, but you can override this on the
# command line without modifying this Makefile; e.g. with
#
# $ make PLATFORM=arm-rpi
#
PLATFORM := arm-rpi
IMAGEFILE := kernel.img

# Filename of boot image to create (platform can override it if really needed)
BOOTIMAGE := xinu.boot

# Top-level Embedded Xinu directory
TOPDIR  := ..
# Temp directory for object files to go into
BUILD_DIR ?= $(TOPDIR)/build

# Set the C compilation flags common to all Embedded Xinu platforms.
# platformVars can add more flags if desired.
CFLAGS  :=

# Do not perform linking until the end.
CFLAGS  += -c

# Optimize for size.
# XXX CFLAGS  += -Os
CFLAGS += -Os
# Enable most useful compiler warnings.
CFLAGS  += -Wall

# Require full prototypes for all functions.
CFLAGS  += -Wstrict-prototypes

# Do not search standard system directories such as "/usr/include" for header
# files, since Embedded Xinu is freestanding and only uses its own headers.
CFLAGS  += -nostdinc

# Do not allow gcc to replace function calls with calls to gcc builtins, except
# when explicitly requested through a __builtin prefix.  This ensures that gcc
# does not attempt to replace any of our code with its own.
CFLAGS  += -ffreestanding

# Assume that the memory locations pointed to by any two pointers can alias,
# even if the types of the variables pointed to are not compatible as defined in
# the C standard.  Enabling this option is fairly common, since most programmers
# don't fully understand aliasing in C, and this forces the "expected" behavior.
CFLAGS  += -fno-strict-aliasing

# On ARM7/8 with MMU on unaligned access will raise an exception so we need to stop it
CFLAGS  += -mno-unaligned-access

# Do not allow multiple definitions of uninitialized global variables.
CFLAGS  += -fno-common 

# Place each function in a separate section so that the linker can apply garbage
# collection to remove unused functions (the --gc-sections linker flag).
CFLAGS  += -ffunction-sections

# Do not generate position-independent code.  (This flag may be unneeded, since
# generally you have to specify -fPIC to *get* the compiler to generate
# position-independent code).
CFLAGS  += -fno-pic

# Treat signed overflow as fully defined as per two's complement arithmetic,
# even though the C standard specifies that signed overflow is undefined
# behavior.  Many programmers are not aware of this, so we force the expected
# behavior.
CFLAGS  += -fwrapv

# DEBUG is used internally by `make debug' to add debugging flags.
DEBUG   :=
CFLAGS  += $(DEBUG)

# Set assembler flags common to all platforms.  platformVars can add additional
# flags if needed.  Do not use the "-Wa," prefix, either here or in
# platformVars.
#
# Note: this -I option is not used for C headers; instead it's used in case the
# platform wants to include other files directly from assembly sources.
#ASFLAGS := -I$(TOPDIR)/system/platforms/$(PLATFORM)

# Set linker flags common to all platforms.  platformVars can add additional
# flags if needed.  Do not use the "-Wl," prefix either here on in platformVars.
LDFLAGS := --static --gc-sections

# Set default external libraries.  Embedded Xinu is, of course, stand-alone and
# ordinarily does not need to be linked to any external libraries; however,
# platformVars can add -lgcc to this if needed by the platform.
LDLIBS  := -lc -lm -lgcc

# Set default include directories.  platformVars can add additional directories
# to this if really needed. The build directory is needed for dependency building
INCLUDE := -I$(TOPDIR) -I$(TOPDIR)/include -I$(TOPDIR)/system/platforms/$(PLATFORM)  

# Override this as -DDETAIL to enable certain debugging messages in Embedded
# Xinu.
DETAIL  :=

# Set default additional defines.  platformVars can add extra defines if needed.
DEFS    := $(DETAIL)

# Set default libraries to build into Xinu
#
# Each library LIB is expected to be built from a directory $(LIBDIR)/$(LIB) and
# generate an archive $(LIBDIR)/$(LIB).a.  Unlike other Embedded Xinu
# directories, each library is expected to have an actual 'Makefile' and not
# just 'Makerules'.  However, this 'Makefile' will be executed with variables
# from this one in its environment.
#
# platformVars can add additional libraries to $(LIBS); however the C library
# (libxc) is always included by default.
LIBS    := libxc

###############################################################################

# Include the platform-specific Makefile.
include platforms/$(PLATFORM)/platformVars

###############################################################################

# Finalize ASFLAGS and CFLAGS.
#ASFLAG_PREFIX := -Wa,
#ASFLAGS       := $(addprefix $(ASFLAG_PREFIX),$(ASFLAGS))
#ASFLAGS       += $(INCLUDE) $(DEFS) -c
CFLAGS        += $(INCLUDE) $(DEFS)

# Make the presence of the testsuite available to the C code.
ifneq ($(filter test, $(APPCOMPS)),)
  CFLAGS += -DHAVE_TESTSUITE=1
else
  CFLAGS += -DHAVE_TESTSUITE=0
endif
# Make the presence of the shell available to the C code.
ifneq ($(filter shell, $(APPCOMPS)),)
  CFLAGS += -DHAVE_SHELL=1
else
  CFLAGS += -DHAVE_SHELL=0
endif

# Set linker script.  A script in platforms/ overrides the one in arch/, if
# present.
#ifneq ($(TEMPLATE_ARCH),)
#  ifeq ($(shell if [ -e arch/$(TEMPLATE_ARCH)/ld.script ]; then echo 1; fi),1)
#    LDSCRIPT := arch/$(TEMPLATE_ARCH)/ld.script
#  endif
#endif

#ifeq ($(shell if [ -e platforms/$(PLATFORM)/ld.script ]; then echo 1; fi),1)
#  LDSCRIPT := platforms/$(PLATFORM)/ld.script
#endif
LDSCRIPT := platforms/$(PLATFORM)/ld.script

ifeq ($(LDSCRIPT),)
  $(warning ld.script not found)
else
  LDFLAGS += -T$(LDSCRIPT)
endif

# Set the location of the architecture-specific cross-compiler and binutils.
# This can be overridden on the command line; e.g.
#
# make PLATFORM=arm-rpi COMPILER_ROOT=/opt/arm-dev/bin/arm-none-eabi-
#
COMPILER_ROOT := $(ARCH_ROOT)$(ARCH_PREFIX)

# Set the actual compiler and several binutils programs.
CC       := $(COMPILER_ROOT)gcc
AR       := $(COMPILER_ROOT)ar
AS       := $(COMPILER_ROOT)as
LD       := $(COMPILER_ROOT)ld -Map test.map
STRIP    := $(COMPILER_ROOT)strip
OBJCOPY  := $(COMPILER_ROOT)objcopy

# Sanity check: does 'gcc' actually exist?
#ifneq ($(shell if $(CC) --version &> /dev/null; then echo 1; fi),1)
#  $(warning $(CC) not found; try overriding COMPILER_ROOT \
#	    (see README.compiling))
#endif

# Did platformVars add -lgcc to LDLIBS?  If so, we need to do the linking with
# gcc rather than with ld so that the appropriate libgcc can be found.  However,
# this also requires adjusting LDFLAGS to ensure they are passed directly to the
# linker and adding -nostdlib to prevent gcc from linking in the C runtime
# startup stub.
ifneq ($(filter -lgcc,$(LDLIBS)),)
  KERNEL_LD      := $(CC)
  LDFLAGS_PREFIX := -Wl,
  LDFLAGS        := $(addprefix $(LDFLAGS_PREFIX), $(LDFLAGS))
  LDFLAGS        += -nostdlib
else
  KERNEL_LD     := $(LD)
endif

# Path to platform-specific configuration file
CONFIG := platforms/$(PLATFORM)/xinu.conf

LIBDIR   := $(TOPDIR)/lib

# With GNU make, if you define the magic target .SILENT, `make' will not echo
# recipes as they are executed.  We use this mode by default and allow the
# normal mode to be switched on by overriding VERBOSE to any value.
ifeq ($(VERBOSE),)
.SILENT:
endif

###########################
# Source file information #
###########################

# Major system components to build into Xinu
# Each component is a directory in the source tree and should include a
# Makerules specifying what files to include
# (The loader should always be first, ensuring the image starts with it.)
SYSCOMPS := loader/platforms/$(PLATFORM) \
            system/platforms/$(PLATFORM) \
            system

# List of device driver components to build into Xinu.  Generated from DEVICES,
# which should have been set in platformVars.
DEVCOMPS := $(DEVICES:%=device/%)

# APPCOMPS, the list of application components to build into Xinu, should also
# have been set in platformVars.

# List of all Xinu components to include
COMPS    := $(SYSCOMPS) $(DEVCOMPS) $(APPCOMPS)

# Include expected files, each should add its part to the compile source
COMP_SRC :=
include $(COMPS:%=$(TOPDIR)/%/Makerules)


LIB_ARC  := $(LIBS:%=$(LIBDIR)/%.a)


# Data is relative to the compile directory
DATA     := data

DATA_SRC :=
include $(DATA:%=%/Makerules)

DATA_OBJ := $(patsubst %,%.o,$(DATA_SRC))

# Flags to get GCC to generate dependency information (in .d files) as a side
# effect of compilation or assembly.
DEPFLAGS := -MMD -MP

CFILES := $(strip $(filter-out %.S, $(COMP_SRC)))
SFILES := $(strip $(filter-out %.c, $(COMP_SRC)))
SOFILES := $(patsubst %.S,$(BUILD_DIR)/%.o, $(notdir $(SFILES)))
COFILES := $(patsubst %.c,$(BUILD_DIR)/%.o, $(notdir $(CFILES)))
DEPFILES := $(SOFILES:%.o=%.d) $(COFILES:%.o=%.d)

# Export variables for recursive make calls (such as when building libraries)
export
################
# Make targets #
################

# Note: the default target is actually $(BOOTIMAGE) and is defined in
# "platformVars".  But it will depend on "xinu.elf".

xinu.elf: $(SOFILES) $(COFILES) $(DATA_OBJ) $(LIB_ARC)
	@echo "Linking" ==\> $@
	$(CC) -o $@ $(LDFLAGS) $^ $(LDLIBS)
	@echo "Creating Binary" ==\> $(IMAGEFILE)
	$(COMPILER_ROOT)objcopy xinu.elf -O binary $(IMAGEFILE)
	$(COMPILER_ROOT)nm -n xinu.elf > xinu.map

$(SOFILES): $(SFILES)
	@echo Assembling $(filter %/$(patsubst %.o,%.S,$(notdir $@)), $(SFILES)) ==\> $@
	$(CC) $(INCLUDE) $(CFLAGS) $(DEPFLAGS) $(filter %/$(patsubst %.o,%.S,$(notdir $@)), $(SFILES)) -c -o $@

$(COFILES): $(CFILES)
	@echo Compiling for $(CODE_ARCH) $(filter %/$(patsubst %.o,%.c,$(notdir $@)), $(CFILES)) ==\> $@
	$(CC) $(INCLUDE) $(CFLAGS) $(DEPFLAGS) $(filter %/$(patsubst %.o,%.c,$(notdir $@)), $(CFILES)) -c -o $@

$(SFILES): ;

$(CFILES): ;

clean:
	@echo "Cleaning all objects"
	$(RM) $(subst /,$(SLASH), $(BUILD_DIR)/*.o)
	$(RM) $(subst /,$(SLASH), $(BUILD_DIR)/*.d)
	$(RM) xinu.elf
	$(RM) *.map
.PHONY:	clean

# Include generated dependency information for C and assembly files, if it
# exists.
-include $(DEPFILES)
# A rule for the dependency files just in case
$(BUILD_DIR)/%.d: ;

# XXX: Hack to deal with special device directories.
DEVDOCCOMPS := $(DEVCOMPS)
ifneq ($(filter uart-%,$(DEVICES)),)
  DEVDOCCOMPS += device/uart
endif

debug:
	@echo "Building debug mode"
	$(MAKE) DEBUG="-DDEBUG $(BUGFLAG)"

help:
	$(PAGER) README.compiling

libclean: $(LIBS:%=%-clean)

###################
# Special targets #
###################
$(COMPS):
	@echo "Building" $@
	$(MAKE) COMPS="$@" objects

$(DATA_OBJ):
	@echo "Object Copy" $@
	$(OBJCOPY) $(OCFLAGS) $(@:%.o=%) $@

$(LIBS): $(CONF_OBJ)
	@echo "Building" $@
	$(MAKE) -C $(LIBDIR)/$@

$(LIB_ARC): $(CONF_OBJ)
	@echo "Installing" $@
	$(MAKE) -C $(@:%.a=%) install

$(LIBS:%=%-clean):
	@echo "Cleaning" $@
	$(MAKE) -C $(LIBDIR)/$(@:%-clean=%) clean

# With GNU make, "phony" targets are always rebuilt unconditionally.  We include
# the static library archives here so that Makefiles to build the libraries are
# always executed, since only they know how to build the libraries.  Note that
# they may simply decide that the libraries are already up to date and not
# rebuild them.  We also include the version header because it contains a
# timestamp of the build that must be updated every time the kernel is built.
.PHONY: $(LIB_ARC) $(TOPDIR)/include/version.h

Pi3 Pi2 Pi1: xinu.elf
.PHONY: xinu.elf
Pi1: IMAGEFILE := kernel.img
Pi2: IMAGEFILE := kernel7.img
Pi3: IMAGEFILE := kernel8-32.img
Pi3-64: IMAGEFILE := kernel8.img

# Ensure the default `make' target is set correctly--- it's supposed to be
# $(BOOTIMAGE) and defined in "platformVars".
.DEFAULT_GOAL := xinu.elf
