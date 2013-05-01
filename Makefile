#------------------------------------------------------------------------------
# Makefile for sample HDF code
#------------------------------------------------------------------------------

# Set up compile options
CC = gcc
RM = rm -f
EXTRA = -Wall -g

# Define the include files
INC = bool.h const.h date.h error_handler.h input.h myhdf.h mystring.h \
myproj.h myproj_const.h output.h space.h swe.h
INCDIR  = -I. -I$(HDFINC) -I$(HDFEOS_INC) -I$(HDFEOS_GCTPINC) -I$(JPEGINC) 
NCFLAGS = $(EXTRA) $(INCDIR)

# Define the source code and object files
SRC = calculate_slope.c         \
      date.c                    \
      error_handler.c           \
      get_args.c                \
      input.c                   \
      myhdf.c                   \
      mystring.c                \
      output.c                  \
      surface_water_extent.c    \
      ias_misc_split_filename.c \
      space.c                   \
      write_envi_hdr.c          \
      scene_based_swe.c
OBJ = $(SRC:.c=.o)

# Define the object libraries
LIB   = -L$(HDFLIB) -lmfhdf -ldf -lxdr -L$(JPEGLIB) -ljpeg -lz -lm -lstdc++
EOSLIB = -L$(HDFEOS_LIB) -lhdfeos -L$(HDFEOS_GCTPLIB) -lGctp

# Define the executable
EXE = scene_based_swe

# Target for the executable
all: $(EXE)

scene_based_swe: $(OBJ) $(INC)
	$(CC) $(EXTRA) -o $(EXE) $(OBJ) $(EOSLIB) $(LIB)

install:
	cp $(EXE) $(BIN)

clean:
	$(RM) *.o $(EXE)

$(OBJ): $(INC)

.c.o:
	$(CC) $(NCFLAGS) -c $<

