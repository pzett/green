%   Wireless Transmission over 60Ghz
%
%   TX SCRIPT TO TRANSMIT DATA PROCESSED IN MATLAB
%

clear all
close all

test_n=1;

Nsamples_tx=100000;


%Set USRP values 

Nsamples=2*Nsamples_tx;
RF_freq=5.5e9; 
ref_clk=0;
gain=15;
tx_rate=25e6;
LOoffset=0;
low_res=0;

%Receive sequence

rx_data=rx(Nsamples, RF_freq, ref_clock, rx_rate, LOoffset);

figure(1,2,1);
plot(rx_data);
title('Data Received from USRP');

figure(1,2,2)
pwelch(rx_data);


%Save data

file_str=['data/tx_data_', num2str(test_n),'.mat'];

save('data/rx_data.mat','');
