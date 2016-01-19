#-----------------------------------------------------------------------------
# Makefile
#
# For building dynamic-surface-water-extent.
#-----------------------------------------------------------------------------
.PHONY: all install clean

# Inherit from upper-level make.config
TOP = ../..
include $(TOP)/make.config

#-----------------------------------------------------------------------------
# Set up compile options
CC = gcc
RM = rm
EXTRA = -Wall $(EXTRA_OPTIONS)

# Define the include files
INC = get_args.h cfmask_water_detection.h utilities.h input.h

# Define the source code and object files
SRC = \
      get_args.c \
      utilities.c \
      input.c \
      cfmask_water_detection.c
OBJ = $(SRC:.c=.o)

# Define include paths
INCDIR  = -I. -I$(ESPAINC) -I$(XML2INC)
NCFLAGS = $(EXTRA) $(INCDIR)

# Define the object libraries and paths
EXLIB = -L$(ESPALIB) -l_espa_raw_binary -l_espa_common \
        -L$(XML2LIB) -lxml2 \
        -L$(LZMALIB) -llzma \
        -L$(ZLIBLIB) -lz
MATHLIB = -lm
LOADLIB = $(EXLIB) $(MATHLIB)

# Define the executable
EXE = cfmask_water_detection

#-----------------------------------------------------------------------------
all: $(EXE)

$(EXE): $(OBJ) $(INC)
	$(CC) $(EXTRA) -o $(EXE) $(OBJ) $(LOADLIB)

#-----------------------------------------------------------------------------
install:
	install -d $(link_path)
	install -d $(cfwd_bin_install_path)
	install -m 755 $(EXE) $(cfwd_bin_install_path)
	ln -sf $(cfwd_link_source_path)/$(EXE) $(link_path)/$(EXE)

#-----------------------------------------------------------------------------
clean:
	$(RM) -f *.o $(EXE)

#-----------------------------------------------------------------------------
$(OBJ): $(INC)

.c.o:
	$(CC) $(NCFLAGS) -c $<

