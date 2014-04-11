#==============================================================
CXXFLAGS = -o3 -Wall -DNDEBUG `itpp-config --cflags`
LDLIBS = -luhd -lboost_program_options`itpp-config --libs` 

#==============================================================
 
all: harness_imp1all

#==============================================================
clean:
	rm -f *.o 
	rm  harness_imp1all

#==============================================================

rx_funct_2_debug.cpp:	rx_funct_2.cpp
	cp 	rx_funct_2.cpp 	rx_funct_2_debug.cpp
rx_funct_2_debug.o:	CXXFLAGS = -ggdb -Wall 
harness_imp1all.o:	CXXFLAGS = -ggdb -Wall 

harness_imp1all:	harness_imp1all.o	rx_funct_2_debug.o 
	$(CXX) -o $@ $^ $(LDLIBS) 

