#==============================================================
CXXFLAGS = -o3 -Wall -DNDEBUG -std=c++0x -lpthread 
LDLIBS = -luhd -lboost_program_options 

#==============================================================

all:	rx_3

#==============================================================
clean:
	rm -f *.o 
	rm -f rx_3 
#==============================================================

rx_3: rx_3.o 

	$(CXX) -o $@ $^ $(LDLIBS) 
#==============================================================


