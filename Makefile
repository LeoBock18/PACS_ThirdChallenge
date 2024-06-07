# Variables
MPICXX ?= mpic++
CXXFLAGS ?= -std=c++20 -O3
CPPFLAGS ?= -fopenmp -I. -I${PACS_ROOT}/include -I.${PACS_ROOT}/src/muParserInterface/ -DNDEBUG
LDLIBS ?= -L${PACS_ROOT}/lib -lmuparser
LDFLAGS ?=
TARGET = main.exe
SOURCES = main.cpp jacobi.cpp densemat.cpp ${PACS_ROOT}/src/muParserInterface/muParserInterface.cpp
HEADERS = jacobi.hpp densemat.hpp writeVTK.hpp ${PACS_ROOT}/src/muParserInterface/muParserInterface.hpp
OBJECTS = $(SOURCES:.cpp=.o)


.PHONY: all clean distclean run

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(MPICXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) $(SOURCES) -o $@ $(LDLIBS)

clean:
	$(RM) *.o
distclean: clean
	$(RM) $(TARGET) test/*.vtk test/data.txt

run: $(TARGET)
	mpiexec -np 4 -x OMP_NUM_THREADS=2 ./$(TARGET)