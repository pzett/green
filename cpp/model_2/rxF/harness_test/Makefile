#==============================================================
CXXFLAGS = -o3 -Wall -DNDEBUG	`itpp-config --cflags`
LDLIBS = -luhd -lboost_program_options `itpp-config --libs`

#==============================================================
 
all: harness

#==============================================================
clean:
	rm -f *.o 
	rm  harness 

#==============================================================

rx_funct_debug.cpp:	rx_funct.cpp
	cp rx_funct.cpp rx_funct_debug.cpp
rx_funct_debug.o :	CXXFLAGS = -ggdb -Wall
harness.o:				CXXFLAGS = -ggdb -Wall

harness:	harness.o	rx_funct_debug.o 
	$(CXX) -o $@ $^ $(LDLIBS) 

