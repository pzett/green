%   Wireless Transmission over 60Ghz
%
%   TX SCRIPT TO TRANSMIT DATA PROCESSED IN MATLAB
%

clear all
close all

test_n=4;

Nsamples_tx=100600;


%Set USRP values 

Nsamples=2*Nsamples_tx;
RF_freq=5.5e9; 
ref_clock=0;
gain=15;
rx_rate=25e6;
LOoffset=10e6;

%Receive sequence

rx_data=rx(Nsamples, RF_freq, ref_clock, gain, rx_rate, LOoffset);

rx_data=rx_data/4000;

figure(1)
subplot(1,2,1);
plot(rx_data,'o');
axis([-0.1 0.1 -0.1 0.1])
title('Data Received from USRP');

subplot(1,2,2)
pwelch(rx_data);


%Save data

rx_data_4=rx_data;

file_str=['data/rx_data_', num2str(test_n),'.mat'];

%save(file_str,'rx_data_4');
