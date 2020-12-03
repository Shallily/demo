SHELL=/bin/bash

# **********************************************
# Toolchains
# **********************************************
CC          = gcc
LD          = ld
CXX         = g++
OBJCOPY     = objcopy
OBJDUMP     = objdump
SIZE        = size
NM          = nm
AR          = ar
STRIP       = strip
READ_ELF    = readelf

# **********************************************
# Directories
# **********************************************
#cpp source files
SRCCPP =    ./core/common       \
			./core/demo         \
            ./sample            \
            ./app

# Header files
INCDIR    = ./core/common	    \
			./core/common/inc   \
			./core/demo/inc

BINPATH   = bin
OBJPATH   = obj
IMAGENAME = main

VPATH     = $(SRCCPP)

# **********************************************
# Image file names
# **********************************************
IMAGE_ELF   = $(BINPATH)/$(IMAGENAME)

# **********************************************
#configures
# **********************************************
#static/dynamic
LINK_TYPE = static

# **********************************************
# Files to be compiled
# **********************************************
SRC_CPP   += $(foreach dir,$(SRCCPP),$(wildcard $(dir)/*.cpp))
OBJ_CPP_0 = $(notdir $(patsubst %.cpp, %.o,$(SRC_CPP)))
OBJ_CPP   = $(foreach file, $(OBJ_CPP_0),$(OBJPATH)/$(file))

# **********************************************
# Compiler and linker options
# **********************************************
INCLUDE     += $(foreach dir, $(INCDIR), -I$(dir))

CC_OPTS     = -Wall -Wpointer-arith -Wstrict-prototypes -Winline -Wundef -fno-strict-aliasing -fno-optimize-sibling-calls -ffunction-sections -fdata-sections -fno-exceptions -c -fPIC
CC_OPTS     += $(INCLUDE) $(CFG_CC_OPTS) $(OTT_LIBS_INCLUDE)

CXX_OPTS    = -Wall -Wno-unused-but-set-variable -Wpointer-arith -Winline -Wundef -fno-strict-aliasing -fno-optimize-sibling-calls -ffunction-sections -fdata-sections -fno-exceptions -c -fPIC
CXX_OPTS    += $(INCLUDE) $(CFG_CC_OPTS) $(OTT_LIBS_INCLUDE)

CXX_DEFS    = -D'LINUX = 1'

CXX_DEFS    += -D'SH_SUPPORT_LONGLONG' -D'SH_SUPPORT_64BIT'

CC_DEFS     = $(CXX_DEFS)

THIRD_LIB   += -L$(LIB_PATH)
THIRD_LIB   += $(foreach dir,$(LIB_PATH),$(wildcard $(dir)/*.a))
ifeq ($(CURL),enable)
THIRD_LIB += -lssl -lcrypto
endif
THIRD_LIB_DYNAMIC_PATH = -L$(LIB_PATH)
THIRD_LIB_DYNAMIC      = $(foreach dir, $(LIB_PATH), $(basename $(notdir $(wildcard $(dir)/*.so))))
THIRD_LIB_DYNAMIC_OPT  = $(THIRD_LIB_DYNAMIC_PATH) $(THIRD_LIB_DYNAMIC:lib%=-l%)
# **********************************************
# Rules
# **********************************************
all: setup $(IMAGE_ELF) 

setup:
	-@rm $(IMAGE_ELF)
	-@mkdir -p $(OBJPATH) 2> /dev/null;
	-@mkdir -p $(BINPATH) 2> /dev/null;
	
$(IMAGE_ELF) : $(OBJ_C) $(OBJ_CPP)
ifeq ($(LINK_TYPE),dynamic)
	@echo $(OBJ_CPP) $(OBJ_C) $(THIRD_LIB_DYNAMIC_OPT)
	@$(CXX) -Wl,--gc-sections -mhard-float -lrt -lm -pthread -lc -ldl -o $(IMAGE_ELF) $(OBJ_CPP) -Wl,-Map=$(IMAGE_ELF).map
else
	@echo $(OBJ_CPP) $(OBJ_C) $(THIRD_LIB)
	@$(CXX) -Wl,--gc-sections -mhard-float -lrt -lm -pthread -lc -ldl -o $(IMAGE_ELF) $(OBJ_CPP) -Wl,-Map=$(IMAGE_ELF).map
endif
	@ls -l $(IMAGE_ELF);
	@$(STRIP) $(IMAGE_ELF)

# Project Source Build
$(OBJ_CPP) : $(OBJPATH)/%.o : %.cpp
	@echo  $@
	@$(CXX) $(CXX_OPTS) $(CXX_DEFS) -o $@ $<;

clean:
	@rm -rf $(OBJPATH)
	@rm -rf $(BINPATH)

