First use in a computer:
"make clean" in the "tx_real_data" and "rx_real_data" folders
"make tx_60GHz" in the "tx_real_data" folder
"make rx_60GHz" in the "rx_real_data" folder

In both computer:
Connect the USRP.
uhd_find_devices command in order for the computer to find the USRPs.
Open matlab

On the transmiter computer:
Open tx_real_data folder
read the script tx_real_data.m
Enter wanted constellation (as string, ex: '32-QAM')
Enter number carriers frequencies (as number, ex: 32)
Choose the file you want to transmit (it should not be bigger than 1.6Mbits)
Look at the number of blocs
clik on any key

On the receiver computer:
Open rx_real_data folder
read the script rx_real_data.m
Enter wanted constellation (the same one as the in the transmitter side)
Enter number carriers frequencies (the same one as the onthe transmitter side)
Enter number of blocs given by the transmitter
Once the transmitter has strted to transmit, clik on any key.

Once the receiver write "done", you can enter ctr+C in the transmiter side.

Wait for the decoding,
Once the receiver has finished, is there were any frame error, it will ask for retransmitting some blocs:

if there is no error and the receiver has finished, enter 0 in the transmiter;
else enter the number of blocks which should be retransmited, then enter the number of the blocks which should be retransmited.



Once the reception is ok and finished, the file appear in the folder:
rx_real_data

Whenever the transmitter or the receiver ask for the sudo code, enter your sudo code.
