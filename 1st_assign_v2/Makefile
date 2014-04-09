#==============================================================
CXXFLAGS = -o3 -Wall -DNDEBUG -std=c++0x -pthread  #`itpp-config --cflags`
LDLIBS = -luhd -lboost_program_options #`itpp-config --libs`

#==============================================================

all:	tx rx 

#==============================================================
clean:
	rm -f *.o 
	rm -f tx rx 
#==============================================================
rx: rx.o rx_funct.o rx_funct.hpp  funct_general.o funct_general.hpp
	$(CXX) -o $@ $^ $(LDLIBS) 
#==============================================================
tx: tx.o tx_funct.o tx_funct.cpp  funct_general.o funct_general.hpp

	$(CXX) -o $@ $^ $(LDLIBS) 
#==============================================================


