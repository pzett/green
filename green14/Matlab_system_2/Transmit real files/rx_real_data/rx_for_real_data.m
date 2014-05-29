%   RX SCRIPT TO TRANSMIT DATA PROCESSED IN MATLAB
clc
clear all
close all
test=0;
%ask for transmission informations
while test==0
modulation_type=input('modulation type (qpsk, 16-QAM, 32-QAM or 64-BEES)? ');
nr_carriers=input('Number of carriers (32 or 64)? ');
try
  file_str=['parameters', modulation_type,num2str(nr_carriers),'Y','.mat'];
  load(file_str)
  test=1;
catch
    test=0;
    fprintf('These parameters are not valid \n')
end
end
pointer=1;
test=0;
while test==0
nr_blocs=input('Number of expected blocks (Number given by the transmiter)?');
test=1;
if nr_blocs>16||nr_blocs<1
    test=0;
    fprintf('Enter a valid number of blocks \n')
end
end
%initialization parameters
Nsamples_per_bloc=floor(1.1*(2*10+12*2+46/21*100000/(nr_bits_x_symbol*rate)));
Nsamples_tx=(Nsamples_per_bloc+1000)*(nr_blocs);
Nsamples=floor(Nsamples_per_bloc*(nr_blocs+2)+1);
ref_clock=0;
gain=18;
rx_rate=25e6;
LOoffset=0;
gain_t=0;
%Wait for receiving
test=1;
fprintf('Press any key when transmitting \n')
pause()
%receive
rx_data_tot=rx_60GHz(Nsamples, ref_clock, gain, rx_rate, LOoffset);
tic
number_decoding=0;
%iterate over the blocks
while test
    number_decoding=number_decoding+1;
    if pointer+Nsamples_per_bloc*2>length(rx_data_tot)
    fprintf('Not all the blocks have been received: Error!!!!! \n')
    str='Please retransmit the blocks: \n';
    a=find(received_blocs==0);
    str=[str, num2str(a(1))];
    for k=2:length(a)
       str=[str,', ',num2str(a(k))];
    end
    str=[str,'.'];
    fprintf(str)
    fprintf('Press any key when transmitting \n')
    pause()
    Nsamples=floor(Nsamples_per_bloc*(length(a)+2)+1);
    Nsamples_tx=(Nsamples_per_bloc+1000)*(length(a));
    number_decoding=1;
    pointer=1;
    rx_data_tot=rx_60GHz(Nsamples, ref_clock, gain, rx_rate, LOoffset);
    end
    if number_decoding==1
       rx_data=rx_data_tot(pointer+1:pointer+Nsamples_per_bloc*2);
    else
       rx_data=rx_data_tot(pointer+1:pointer+Nsamples_per_bloc+4000);
    end

%Save data

rx_data=(rx_data.*exp(2*pi*(1:length(rx_data))*1i*0.01)).';

save('rx_data','rx_data');

%decoder of the datas
reciever
pointer=pointer+t_end-2000;
load('rx_info_dec')
if iteration<20||count>1
if ~exist('rx_info')
nr_blocs=sum(rx_info_dec(1:4).*2.^(3:-1:0))+1;
rx_info=zeros(nr_blocs,100000-4*2);
received_blocs=zeros(1,nr_blocs);
end
nr_current_bloc=sum(rx_info_dec(5:8).*2.^(3:-1:0))+1;
rx_info(nr_current_bloc,:)=rx_info_dec(9:end);
received_blocs(nr_current_bloc)=1;
save('rx_info_dec','rx_info','received_blocs','nr_blocs');
test=sum(received_blocs)<nr_blocs;
% test if there still information in what have been received
if test
    str='Blocks still to be decoded: \n';
    a=find(received_blocs==0);
    str=[str, num2str(a(1))];
    for k=2:length(a)
       str=[str,', ',num2str(a(k))];
    end
    str=[str,'.', '\nDecoding the next block. \n'];
    fprintf(str)
end
else
    fprintf('Error in the received bloc, continue decoding \n')
end
end
%when every blocks have been received: decode to regenerate the file
fprintf('All blocks well received, decoding data \n')
source_decoding
writefile
toc
fprintf('Reception well done \n')
try
I = imread(str2);
imshow(I);
catch
end
delete('rx_info_dec.mat');
delete('rx_data.mat');
clear all
