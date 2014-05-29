%   Wireless Transmission over 60Ghz
%
%   TX SCRIPT TO TRANSMIT DATA PROCESSED IN MATLAB
%

clear all
close all
clc

test=0;
%ask for transmition parameters
while test==0
    modulation_type=input('Modulation type (qpsk, 16-QAM, 32-QAM or 64-BEES)? ');
    nr_carriers=input('Number of carriers (32 or 64)? ');
    try
        file_str=['parameters', modulation_type,num2str(nr_carriers),'Y','.mat'];
        load(file_str)
        test=1;
    catch
        test=0;
        fprinf('These parameters are not valid \n')
    end
end
%generate data frame
readfile
source_coding
pointer=1;
save('to_transmit_per_bloc','pointer')
fprintf(['Number of blocks: ', num2str(nr_blocs)])
fprintf('\nPress any key. \n')
pause()
fprintf('Preparing data, please wait. \n')
nr_OFDM_symbols=floor((nr_data_bits/rate-1)/(nr_used_car*nr_bits_x_symbol))+1;
L_bloc=2*nr_guard_bits+Q*nr_training_bits+nr_OFDM_symbols*(nr_car+length_cpre+length_suf);
to_transmit_per_bloc=zeros(L_bloc,nr_blocs);
%generate the complexe sequence from the data bits for each bloc
for i=1:nr_blocs
    load('data_coded')
    data=data_coded(i,:);
    transmiter
    load('to_transmit_per_bloc')
    rms_value=rms(tx_data);
    const=(9000/rms_value);
    tx_data=const*tx_data;
    to_transmit_per_bloc(:,i)=tx_data;
    save('to_transmit_per_bloc','to_transmit_per_bloc')
end
load('to_transmit')
bloc_to_transmit=(1:nr_blocs);
%transmit data: all encode blocs in a row
while ~isempty(bloc_to_transmit)
    tx_data=reshape(to_transmit_per_bloc(:,bloc_to_transmit),1,L_bloc*length(bloc_to_transmit));
    Nsamples=length(tx_data);
    
    %Set USRP values
    ref_clk=0;
    gain=2;
    tx_rate=25e6;
    LOoffset=0;
    low_res=0;
    tx_60GHz( Nsamples, tx_data, ref_clk , gain, tx_rate, LOoffset, low_res);
    test=0;
    while test==0
    nr_bloc_left=input('How many blocs should be retransmited? ');
    test=1;
    if nr_bloc_left>nr_blocs||nr_bloc_left<0
        test=0;
        fprintf('Please enter a valid number \n')
    end
    end
    bloc_to_transmit=zeros(1,nr_bloc_left);
    for i=1:nr_bloc_left
        test=0;
        while test==0;
            str=['Bloc number which need to be retransmited number ', num2str(i),'? '];
            bloc_to_transmit(i)=input(str);
            test=1;
            if bloc_to_transmit(i)>nr_blocs||bloc_to_transmit(i)<1
                test=0;
                fprintf('Please enter a valid number \n')
            end
        end
    end
    fprintf(['The data rate was: ', num2str(data_rate), 'Mbits/s \n'])
end