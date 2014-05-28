#==============================================================
CXXFLAGS = -o3 -Wall -DNDEBUG -std=c++0x -lpthread  `itpp-config --cflags`
LDLIBS = -luhd -lboost_program_options `itpp-config --libs`

#==============================================================
 
all:	rx_60GHz

#==============================================================
clean:
	rm -f *.o 
	rm -f rx_60GHz

#==============================================================
rx_60GHz: rx_60GHz.o board_60GHz.o board_60GHz.hpp rx_funct.o rx_funct.hpp
	$(CXX) -o $@ $^ $(LDLIBS) 



