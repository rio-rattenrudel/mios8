# $Id$

# define the processor here
PROCESSOR = 18f4685

# default flags to GPASM here
GPASM_FLAGS   = 

# include pathes (more will be added by asm_only.mk files)
GPASM_INCLUDE = 

# optional defines that should be passed to GPASM
GPASM_DEFINES = 

# default rule that defines the .hex files which should be created
# note: always require a "cleanall", since dependencies (e.g. on .h and .inc files) are not properly declared
#all: cleanall setup_6581.hex setup_8580.hex setup_sammich_sid.hex setup_tk.hex setup_tk2.hex setup_mb6582.hex sid_filter_table.hex setup_8580_rio.hex setup_6581_rio.hex
all: cleanall sid_filter_table.hex setup_8580_rio.hex setup_6581_rio.hex

# for a separate build
sid_filter_table:
	gpasm -p $(PROCESSOR) sid_filter_table.asm

# include the asm_only.mk file
include $(MIOS_PATH)/include/makefile/asm_only.mk

# include J5 IO driver
include $(MIOS_PATH)/modules/j5_io/j5_io.mk

# include AOUT driver
include $(MIOS_PATH)/modules/aout/aout.mk
