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

sync_catch_channel_debug.cpp:	sync_catch_channel.cpp
	cp sync_catch_channel.cpp sync_catch_channel_debug.cpp
sync_catch_channel_debug.o :	CXXFLAGS = -ggdb -Wall
harness.o:				CXXFLAGS = -ggdb -Wall

harness:	harness.o	sync_catch_channel_debug.o 
	$(CXX) -o $@ $^ $(LDLIBS) 

