#==============================================================
CXXFLAGS = -o3 -Wall -DNDEBUG  `itpp-config --cflags`
LDLIBS = -luhd -lboost_program_options `itpp-config --libs`

#==============================================================

all:	tx_3

#==============================================================
clean:
	rm -f *.o 
	rm -f tx_3 
#==============================================================


tx_funct_debug.cpp:	tx_funct.cpp
	cp tx_funct.cpp tx_funct_debug.cpp
tx_funct_debug.o:	CXXFLAGS = -ggdb -Wall
tx_3.o:				CXXFLAGS = -ggdb -Wall

tx_3:	tx_3.o	tx_funct_debug.o 
	$(CXX) -o $@ $^ $(LDLIBS) 

#==============================================================

