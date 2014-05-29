
For testing:
	Transmitter:
		- make
		- Run generate_parameter.m to create new data to be sent. The needed values for decoding are saved in a file 'tx_data_n.mat'. 
		- Choose between 32 and 64 carriers in the upper part of the code.
		- Check the PAPR before being send. Check also a good amplitude for the training sequences.
		- Run tx_script.m
	
	Receiver:
		- make
		- add Rx and subfolders to path. 
		- Change line 27 to sigma_sqr=13; in rx_main
		- Run rx_script.m. The received data are saved under 'rx_data_n.mat'.
		- Copy the file 'tx_data_n.mat' in the receiver computer.
		- Run rx_main.m. Do not forget to specify the right files to load in the upper part.
		
For simulating:
	Transmitter:
		- Run generate_parameter.m to create new data to be sent. The needed values for decoding are saved in a file 'tx_data_n.mat'. 
		  Choose between 32 and 64 carriers in the upper part of the code.
		  Check the PAPR before being send. Check also a good amplitude for the training sequences.
		- Run sim_channel.m for simulation purpose.
	Receiver:
		- Run rx_main.m
	
Note: 
	- Make sure that you specify the good simulation number (n_sim) each time, in order to not overwrite files.
	- If real transmission, the 100 first samples of the crosscorrelation should be ignored in the sync function.
      This option can be indicated by the boolean  test=0,1;
	- The amplitude of the training and pilots should be wisely chosen.
