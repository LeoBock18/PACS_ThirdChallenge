MPICXX ?= mpicxx
CXXFLAGS ?= -std=c++20 -Wall -Wextra
CPPFLAGS ?= -I. -I./usr/lib/openmpi/include
LDLIBS += -L${PACS_ROOT}/lib -lpacs
LDFLAGS ?= -O3
TARGET = main
HEADERS = densemat.hpp jacobi.hpp
SRCS = main.cpp densemat.cpp densemat.cpp
OBJS = $(SRCS:.cpp=.o)

.PHONY: all clean distclean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(MPICXX) $(CXXFLAGS) -o $(TARGET) $(SRCS) $(LDLIBS)

clean:
	$(RM) *.o
distclean: clean
	$(RM) $(TARGET)

run: $(TARGET)
	mpiexec -np 4 ./$(TARGET)