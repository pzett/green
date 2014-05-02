%   Wireless Transmission over 60Ghz
%
%   TX SCRIPT TO TRANSMIT DATA PROCESSED IN MATLAB
%

clear all
close all


% Load data previous processed

test_n=1;

file_str=['tx_data_', num2str(test_n),'.mat'];

load(file_str)

tx_data=tx_data_2;


Im=imread('LogoMain2013.png');
image(Im)
[data] = generate_data_from_picture(Im);
source_coding
data=data_coded;
length(data)
transmiter


Nsamples=length(tx_data);

rms_value=rms(tx_data);

if (rms_value > 500 || rms_value<400)  %Set proper RMS value
    const=400/rms_value;
    tx_data=const*tx_data;
end

rms(tx_data)


 %tx_data = cw( 2.5e6 , Nsamples);

%Plot the data to send:
figure(1)
subplot(1,2,1)
plot(tx_data, 'o');
title('Data to USRP');


subplot (1,2,2);
pwelch(tx_data);

drawnow;


%Set USRP values 

RF_freq=5.5e9; 
ref_clk=0;
gain=10;
tx_rate=25e6;
LOoffset=10e6;
low_res=0;

tx( Nsamples,RF_freq, tx_data, ref_clk , gain, tx_rate, LOoffset, low_res);







