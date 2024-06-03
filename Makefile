MPICXX ?= mpic++
CXXFLAGS ?= -std=c++20 -Wall -Wextra -O3
CPPFLAGS ?= -I.
LDLIBS += -L${PACS_ROOT}/lib -lpacs
LDFLAGS ?= -O3
TARGET = main
HEADERS = densemat.hpp jacobi.hpp
SRCS = main.cpp densemat.cpp jacobi.cpp
OBJS = $(SRCS:.cpp=.o)

.PHONY: all clean distclean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(MPICXX) $(CXXFLAGS) $(LDFLAGS) $(OBJS) -o $(TARGET) $(LDLIBS)

$(OBJS): $(SRCS) $(HEADERS)
	$(MPICXX) $(CXXFLAGS) $(CPPFLAGS) -c $<

clean:
	$(RM) *.o
distclean: clean
	$(RM) $(TARGET)

run: $(TARGET)
	mpirun -np 4 ./$(TARGET)