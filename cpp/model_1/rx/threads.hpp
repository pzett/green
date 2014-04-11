#include <thread>         // std::thread, std::this_thread::yield
#include <queue>         // std::vector
#include <iostream>
#include <mutex>

#include "threads.cpp"

void storeData2(uhd::rx_streamer::sptr rx_stream, size_t buffer_size, int nDetect);

void detection();
