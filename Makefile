CXX ?= mpic++
CXXFLAGS ?= -std=c++20 -Wall -O3
CPPFLAGS ?= -I. -I${PACS_ROOT}/src/Matrix -fopenmp
LDLIBS += -L${PACS_ROOT}/lib -lpacs
LDFLAGS ?= -O3
TARGET = main
HEADERS = 
SRCS = main.cpp
OBJS = $(SRCS:.cpp=.o)

.PHONY: all clean distclean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) $(OBJS) -o $(TARGET) $(LDLIBS)

$(OBJS): $(SRCS) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $<

clean:
	$(RM) *.o
distclean: clean
	$(RM) $(TARGET)
