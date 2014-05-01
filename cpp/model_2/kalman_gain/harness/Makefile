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

kalman_gain_debug.cpp:	kalman_gain.cpp
	cp kalman_gain.cpp kalman_gain_debug.cpp
kalman_gain_debug.o :	CXXFLAGS = -ggdb -Wall
harness.o:				CXXFLAGS = -ggdb -Wall

harness:	harness.o	kalman_gain_debug.o 
	$(CXX) -o $@ $^ $(LDLIBS) 

