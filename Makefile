MPICXX ?= mpic++
CXXFLAGS ?= -std=c++20 -Wall -Wextra
CPPFLAGS ?= -I.
LDLIBS += -L${PACS_ROOT}/lib -lpacs
LDFLAGS ?= -O3
TARGET = main
HEADERS = densemat.hpp jacobi.hpp
SRCS = main.cpp
OBJS = $(SRCS:.cpp=.o)

.PHONY: all clean distclean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(MPICXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(OBJS): $(SRCS) $(HDRS)
	$(MPICXX) $(CXXFLAGS) -c $< -o $@



clean:
	$(RM) *.o
distclean: clean
	$(RM) $(TARGET)

run: $(TARGET)
	mpiexec -np 4 ./$(TARGET)