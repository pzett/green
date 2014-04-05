%   Wireless Transmission over 60Ghz
%
%   TX SCRIPT TO TRANSMIT DATA PROCESSED IN MATLAB
%

clear all
close all


% Load data previous processed

test_n=1;

file_str=['data/tx_data_', num2str(test_n),'.mat'];

load(file_str)


Nsamples=length(tx_data);

rms_value=rms(tx_dat);

if (rms_value > 5000 && rms_value<4000)  %Set proper RMS value
    const=5000/rms_value;
    tx_data=const*tx_data;
end


%Plot the data to send:
figure(1,2,1)
plot(tx_data, 'o');
title('Data to USRP');


figure (1,2,2);
pwelch(tx_data);

drawnow;


%Set USRP values 

RF_freq=5.5e9; 
ref_clk=0;
gain=20;
tx_rate=25e6;
LOoffset=0;
low_res=0;

tx( Nsamples,RF_freq, tx_seq, ref_clk , gain, tx_rate, LOoffset, low_res);







