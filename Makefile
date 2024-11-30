
# Compiler and Compiler Flags
CC = g++
CFLAGS = -O3 -std=c++17 -Wno-unused-result

# Source and Object Files
#SRC = src/demo.cpp
#SRC = extract_file.cpp
SRC = charge_spectrum.cpp
OBJDIR = obj
OBJ = $(SRC:.cpp=.o)

ROOTFLAGS = -pthread -m64 -I/opt/root/include

INCDIR = -I/opt/WCTEDAQ/DataModel -I/opt/ToolFrameworkCore/include 
#EXECUTABLE = extract_file
EXECUTABLE = charge_spectrum

LIBDIR = -L/opt/ToolFrameworkCore/lib
LIBS = -lStore `root-config --glibs --libs`

all: $(EXECUTABLE)

%.o: %.cpp
	$(CC) $(CFLAGS) $(ROOTFLAGS) $(INCDIR) -c $< -o $@

$(oEXECUTABLE): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LIBDIR) $(LIBS)

clean:
	rm -f $(OBJ) $(EXECUTABLE)
