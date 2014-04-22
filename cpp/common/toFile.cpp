  // Save data to file
     std::ofstream ofs( "sentx_downsamp.dat" , std::ifstream::out );
if (!ofs.is_open()){std::cerr << "File not open";}
     ofs.write((char * ) buffer, nAll*sizeof(short));
     ofs.close();
