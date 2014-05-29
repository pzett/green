The transmitter code is used for 60 GHz

1. Check that the USRPs are connected by uhd_find_devices
2. Open matlab in the directory, where the folder is located
AT THE TRANSMITTER SIDE:
1. Open MATLAB in the directory with the transmitter folder
2. launch the tx_main function
3. choose the modulation
4 choose the number of carriers (32 or 64)
5 choose coding or not ('Y' for coding and  'N' for data without coding)



AT THE RECEIVER SIDE:
1. Open rx_main script. Follow the following steps to run the program:
   - set the number of samples to be the same or greater than the length of the transmitted sequence 
   - the following parameters are to be set by the user
     ref_clock=0;
     gain can be chosen, commonly used value is 15-18, depends on attenuaters
     rx_rate=25e6; 
     LOoffset=0; offset for the Local Oscillator
     gain_t=0;   set to zero 
2. Set the following parameters that should be the same as at the transmitter:
    modulation_type
    coding
    nr_carriers
rx_main calls the receiver function, which processes the data. 

