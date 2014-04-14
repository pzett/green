#include <fstream> // PZ
#include <uhd/utils/thread_priority.hpp>
#include <uhd/utils/safe_main.hpp>
#include <uhd/usrp/multi_usrp.hpp>
#include <uhd/device.hpp>
#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include <iostream>
#include <complex>

#include <uhd/types/clock_config.hpp>


#include <thread>         // std::thread, std::this_thread::yield
#include <queue>         // std::vector
#include <mutex>
#include <semaphore.h>

#include <chrono>



//using namespace std;
/*
extern sem_t isReady;
extern queue<short*> bufferQ;
extern mutex mtx;
extern mutex mtxQ;
*/
void storeDataX(uhd::rx_streamer::sptr rx_stream, size_t buffer_size, uint nDetect);

void detection(uint nDetect);
