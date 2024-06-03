MPICXX ?= mpic++
CXXFLAGS ?= -std=c++20 -O3
CPPFLAGS ?= -fopenmp -I. -I${PACS_ROOT}/include -DNDEBUG
LDLIBS += -L${PACS_ROOT}/lib -lpacs
LDFLAGS ?=
TARGET = main

.PHONY: all clean distclean run

all: $(TARGET)

$(TARGET): %: %.cpp
	$(MPICXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) $< $(LDLIBS) -o $@

clean:
	$(RM) *.o
distclean: clean
	$(RM) $(TARGET)

run: $(TARGET)
	mpiexec -np 4 ./$(TARGET)