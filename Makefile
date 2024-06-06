# Variables
MPICXX ?= mpic++
CXXFLAGS ?= -std=c++20 -O3
CPPFLAGS ?= -fopenmp -I. -I${PACS_ROOT}/include -I.${PACS_ROOT}/src/muParserInterface/ -DNDEBUG
LDLIBS ?= 
LDFLAGS ?=
TARGET = main

.PHONY: all clean distclean run

all: $(TARGET)

$(TARGET): %: %.cpp
	$(MPICXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) $< $(LDLIBS) -o $@

clean:
	$(RM) *.o
distclean: clean
	$(RM) $(TARGET) *.vtk

run: $(TARGET)
	mpiexec -np 4 -x OMP_NUM_THREADS=2 ./$(TARGET) $(ARGS)