#==============================================================
CXXFLAGS = -o3 -Wall -DNDEBUG -std=c++0x -lpthread  `itpp-config --cflags`
LDLIBS = -luhd -lboost_program_options `itpp-config --libs`

#==============================================================

all:	rx

#==============================================================
clean:
	rm -f *.o 
	rm -f rx 
#==============================================================

rx: rx.o # rx_funct.o rx_funct.hpp

	$(CXX) -o $@ $^ $(LDLIBS) 
#==============================================================


