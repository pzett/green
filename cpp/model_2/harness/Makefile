#==============================================================
CXXFLAGS = -o3 -Wall -DNDEBUG -std=c++0x -lpthread  `itpp-config --cflags`
LDLIBS = -luhd -lboost_program_options  `itpp-config --libs`

#==============================================================
 
all: harness_tx

#==============================================================
clean:
	rm -f *.o 
	rm  harness_tx

#==============================================================

tx_funct_debug.cpp:	tx_funct.cpp
	cp tx_funct.cpp tx_funct_debug.cpp
tx_funct_debug.o:	CXXFLAGS = -ggdb -Wall
harness_tx.o:				CXXFLAGS = -ggdb -Wall

harness_tx:	harness_tx.o	tx_funct_debug.o 
	$(CXX) -o $@ $^ $(LDLIBS) 

