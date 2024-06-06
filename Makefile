# Variables
MPICXX ?= mpic++
CXXFLAGS ?= -std=c++20 -O3
CPPFLAGS ?= -fopenmp -I. -I${PACS_ROOT}/include -I.${PACS_ROOT}/src/muParserInterface/ -DNDEBUG
LDLIBS ?= 
LDFLAGS ?=
TARGET = main.exe
SOURCES = main.cpp jacobi.cpp densemat.cpp
HEADERS = jacobi.hpp densemat.hpp writeVTK.hpp
OBJECTS = $(SOURCES:.cpp=.o)
# Default input parameters, if you want to directly "make run" (without bash script)
ARGS="51 5000 1e-5"


.PHONY: all clean distclean run

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(MPICXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) $(SOURCES) $(LDLIBS) -o $@

clean:
	$(RM) *.o
distclean: clean
	$(RM) $(TARGET) test/*.vtk test/data.txt

run: $(TARGET)
	mpiexec -np 4 -x OMP_NUM_THREADS=2 ./$(TARGET) $(ARGS)