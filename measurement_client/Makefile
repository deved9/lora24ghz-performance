# compiler
CXX = arm-none-linux-gnueabihf-g++

# compiler flags
CXXFLAGS = -Wall -Wextra -std=c++20 --static

# executable name
TARGET = main

# source directories
SRCDIR = .
WIMODLRDIR = WiMODLR

# source files
SRCS = $(SRCDIR)/main.cpp \
       $(WIMODLRDIR)/ComSlip.cpp \
       $(WIMODLRDIR)/CRC16.cpp \
       $(WIMODLRDIR)/SerialDevice.cpp \
       $(WIMODLRDIR)/WiMODLRHCI.cpp

# object files
OBJS = $(SRCS:.cpp=.o)

# header files
DEPS = $(WIMODLRDIR)/ComSlip.h \
       $(WIMODLRDIR)/CRC16.h \
       $(WIMODLRDIR)/SerialDevice.h \
       $(WIMODLRDIR)/WiMODLRHCI.h \
       $(WIMODLRDIR)/WiMODLRHCI_IDs.h \
       $(WIMODLRDIR)/WMDefs.h

# build target
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# clean target
.PHONY: clean
clean:
	rm $(TARGET) $(OBJS)

# compile object files
%.o: %.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) -c $< -o $@