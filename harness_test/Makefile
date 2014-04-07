#==============================================================
CXXFLAGS = -o3 -Wall -DNDEBUG
LDLIBS = -luhd -lboost_program_options

#==============================================================
 
all: harness_filt

#==============================================================
clean:
	rm -f *.o 
	rm  harness_filt 

#==============================================================

filter_phase_debug.cpp:	filter_phase.cpp
	cp filter_phase.cpp filter_phase_debug.cpp
filter_phase_debug.o:	CXXFLAGS = -ggdb -Wall
harness_filt.o:				CXXFLAGS = -ggdb -Wall

harness_filt:	harness_filt.o	filter_phase_debug.o 
	$(CXX) -o $@ $^ $(LDLIBS) 

