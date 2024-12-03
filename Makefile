
# Compiler and Compiler Flags
CC = g++
CFLAGS = -O3 -std=c++17 -Wno-unused-result

# Source and Object Files
SRC1 = extract_file.cpp
OBJDIR = obj
OBJ1 = $(SRC1:.cpp=.o)

SRC2 = time_reconstruction.cpp
OBJDIR = obj
OBJ2 = $(SRC2:.cpp=.o)

SRC3 = charge_spectrum.cpp
OBJDIR = obj
OBJ3 = $(SRC3:.cpp=.o)

SRC4 = organize_data_by_mpmt.cpp
OBJDIR = obj
OBJ4 = $(SRC4:.cpp=.o)

ROOTFLAGS = -pthread -m64 -I/opt/root/include

INCDIR = -I/opt/WCTEDAQ/DataModel -I/opt/ToolFrameworkCore/include 
EXECUTABLE1 = extract_file
EXECUTABLE2 = time_reconstruction
EXECUTABLE3 = charge_spectrum
EXECUTABLE4 = organize_data_by_mpmt


LIBDIR = -L/opt/ToolFrameworkCore/lib
LIBS = -lStore `root-config --glibs --libs`

all: $(EXECUTABLE1) $(EXECUTABLE2) $(EXECUTABLE3) $(EXECUTABLE4) 

%.o: %.cpp
	$(CC) $(CFLAGS) $(ROOTFLAGS) $(INCDIR) -c $< -o $@

$(EXECUTABLE1): $(OBJ1)
	$(CC) $(OBJ1) -o $@ $(LIBDIR) $(LIBS)

$(EXECUTABLE2): $(OBJ2)
	$(CC) $(OBJ2) -o $@ $(LIBDIR) $(LIBS)

$(EXECUTABLE3): $(OBJ3)
	$(CC) $(OBJ3) -o $@ $(LIBDIR) $(LIBS)

$(EXECUTABLE4): $(OBJ4)
	$(CC) $(OBJ4) -o $@ $(LIBDIR) $(LIBS)
clean:
	rm -f $(OBJ1)$(OBJ2)$(OBJ3)$(OBJ4) $(EXECUTABLE1)$(EXECUTABLE2)$(EXECUTABLE3)$(EXECUTABLE4)
