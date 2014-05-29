%   Wireless Transmission over 60Ghz
%
%   RX SCRIPT TO TRANSMIT DATA PROCESSED IN MATLAB
%

clear
close all

test_n=31;

Nsamples=98039*2;


%Set USRP values 


gain=15;


%Receive sequence

rx_data=rx_60GHz(Nsamples,  gain);

rx_data=rx_data/4400;

figure(1)
subplot(1,2,1);
plot(rx_data,'o');
%axis([-0.1 0.1 -0.1 0.1])
title('Data Received from USRP');

subplot(1,2,2)
pwelch(rx_data);


%Save data

rx_data_1=rx_data;

file_str=['rx_data_', num2str(test_n),'.mat'];
save(file_str);
rx_main

