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
tx_60GHz: tx_60GHz.o board_60GHz.o board_60GHz.hpp 
	$(CXX) -o $@ $^ $(LDLIBS) 
#==============================================================
tx_funct_debug.cpp:	tx_funct.cpp
	cp	tx_funct.cpp	tx_funct_debug.cpp
tx_funct_debug.o:    CXXFLAGS = -ggdb -Wall

rx_60GHz: rx_60GHz_1.o board_60GHz.o board_60GHz.hpp
	$(CXX) -o $@ $^ $(LDLIBS) 