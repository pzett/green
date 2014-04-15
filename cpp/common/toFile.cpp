  // Save data to file
     std::ofstream ofs( "sentx_downsamp.dat" , std::ifstream::out );
     ofs.write((char * ) buffer, nAll*sizeof(short));
     ofs.close();
