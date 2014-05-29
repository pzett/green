%   TX SCRIPT TO TRANSMIT DATA PROCESSED IN MATLAB
clc
clear all
close all

%ask for transmission information
modulation_type=input('Choose modulation type: qpsk, 16-QAM, 32-QAM, 64-BEES:');  %to be set the same as at the transmitter
nr_carriers=input('Enter the number of carriers, 32 or 64:');
nr_carriers=num2str(nr_carriers);
coding=input('With or without coding:Y/N?');
L_range=input('How many receptions do you want to do?');
test_range=(1:L_range);
%initialize parameters
Nsamples_tx=200000;
Nsamples=2*Nsamples_tx;
ref_clock=0;
gain=18;
rx_rate=25e6;
LOoffset=0;
gain_t=0;
for k=1:length(test_range)
test_n=test_range(k);


%Receive sequence
rx_data=rx_60GHz(Nsamples, ref_clock, gain, rx_rate, LOoffset);

rx_data=rx_data;

figure(1)
subplot(1,2,1);
plot(rx_data,'.');
title('Data Received from USRP');

subplot(1,2,2)
pwelch(rx_data);


%Save data


rx_data=(rx_data.*exp(2*pi*(1:length(rx_data))*1i*0.01)).';


save('rx_data','rx_data');
%decode data
reciever

%plot some results
title(['Received constellation: G_r=',num2str(gain),'; G_t=',num2str(gain_t), ' with ', modulation_type, ' and ', nr_carriers, ' carriers.'])

   
   load('tx_info')
   load('rx_info_dec')
   err=rx_info_dec~=tx_info;
   figure(3)
   plot(err, 'o')
   title('Pattern of errors')
   drawnow
   BER=sum(err)
%pause()
end