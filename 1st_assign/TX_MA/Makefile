#==============================================================
CXXFLAGS = -o3 -Wall -DNDEBUG  #`itpp-config --cflags`
LDLIBS = -luhd -lboost_program_options #`itpp-config --libs`

#==============================================================

all:	tx_1_MA rx 

#==============================================================
clean:
	rm -f *.o 
	rm -f tx_1_MA rx 
#==============================================================
rx: rx.o rx_funct.o rx_funct.hpp  funct_general.o funct_general.hpp
	$(CXX) -o $@ $^ $(LDLIBS) 
#==============================================================
tx: tx_1_MA.o tx_funct_MN.o tx_funct_MN.hpp  funct_general.o funct_general.hpp

	$(CXX) -o $@ $^ $(LDLIBS) 
#==============================================================


