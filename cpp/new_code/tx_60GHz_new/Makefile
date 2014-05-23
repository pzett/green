#==============================================================
CXXFLAGS = -o3 -Wall -DNDEBUG	`itpp-config --cflags`
LDLIBS = -luhd -lboost_program_options	`itpp-config --libs`

#==============================================================
 
all:	tx_60GHz

#==============================================================
clean:
	rm -f *.o 
	rm -f tx_60GHz
#==============================================================
tx_60GHz: tx_60GHz.o board_60GHz.o board_60GHz.hpp  tx_funct.o tx_funct.hpp
	$(CXX) -o $@ $^ $(LDLIBS) 
#==============================================================
