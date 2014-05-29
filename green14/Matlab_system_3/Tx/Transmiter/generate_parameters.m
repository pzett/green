% Script to generate data to be transmitted
clear 
close all
clc

nr_car=32;
n_sim=31;

switch(nr_car)
    case 32
%With 32 subcarriers
nr_training_bits = 12;                 %Number of training bits in the begining of the frame (no OFDM)
length_cpre = 10;                       %Prefix length
length_suf = 5;                        %Suffix to cope with the influence of the future samples on the estimate
train_seq=golay_generator(nr_training_bits);                 %Training sequenceSNR = 0;                             %Eb/N0 values to simulate (in dB)
nr_data_bits =100000;                    %Number of data bits
nr_guard_bits=10;                      %Size of guard sequence, where we don't transmit anything (in symbols) attached at the begining and at the end of the whole frame.
ispilot=0;                             %Flag for knowing if we transmit pilots
Q = 2;                                 %Upsampling rate for the training sequence
pilot_pattern=[13 20];                   %Carriers with pilot, where the 1 means DC and so on until f~1
car_pattern=[(1:12) (21:32)];     
car_pattern_merged=[(1:13) (20:32)]; 
c=3.5;
d=0.8;
Ap=0.4;
    case 64
% With 64 subcarriers
nr_training_bits = 12;                 %Number of training bits in the begining of the frame (no OFDM)
length_cpre = 24;                       %Prefix length
length_suf = 10;                        %Suffix to cope with the influence of the future samples on the estimate
train_seq=golay_generator(nr_training_bits);                 %Training sequenceSNR = 0;                             %Eb/N0 values to simulate (in dB)
nr_data_bits =50048;                    %Number of data bits
nr_guard_bits=10;                      %Size of guard sequence, where we don't transmit anything (in symbols) attached at the begining and at the end of the whole frame.
ispilot=0;                             %Flag for knowing if we transmit pilots
Q = 2;                                 %Upsampling rate for the training sequence
pilot_pattern=[27 38];                   %Carriers with pilot, where the 1 means DC and so on until f~1
car_pattern=[(1:26) (39:64)];     
car_pattern_merged=[(1:27) (38:64)]; 
c=5;
d=0.55;
Ap=0.5
    otherwise
        disp('Error in choosing the number of carriers')
end

nr_car_used=length(car_pattern);
length_pilot=length(pilot_pattern);    %Length of the pilot
nr_blocks=1;                           %Number of data blocks to transmit
modulation_type='16-QAM';                %Modulation for data
pilot_type='random_phase';             %Choose from 'random', 'modulated', or 'random_phase'

[nr_bits_x_symbol]=test_constellation(modulation_type);                       %Compute the bits/symbols of the constellation
[nr_used_car,nr_pilot_car]=check_pattern(car_pattern, pilot_pattern, nr_car); %This function checks the carrier assignment in order to avoid collisions.
k=nr_used_car*nr_bits_x_symbol-(mod(nr_data_bits-1,nr_used_car*nr_bits_x_symbol)+1);  %Number of zeros added in order to fill the OFDM matrix

train_OFDM=LFSR(6,[6 5 4 1],3);
train_OFDM=[train_OFDM' 0];
data=generate_data(nr_data_bits);
nr_OFDM_symbols = floor((length(data)-1)/(nr_used_car*nr_bits_x_symbol))+1;
data=[data rand(1,k)];

%Modulations
data_mapped=modulation(data, modulation_type);

%DFT precoded
data_dft=DFT_precode( data_mapped, nr_car_used, 'encode' );
data_reshaped=reshape(data_dft, nr_used_car, length(data_mapped)/nr_used_car); %Modify data to allocate it for each frequency band. Col=frequency, Row=time.

%Pilots
pilot=Ap*generate_pilot(size(data_reshaped,2), pilot_type, pilot_pattern);     %Get pilots
data_dft_p=zeros(size(data_reshaped, 1)+length(pilot_pattern), size(data_reshaped, 2));
data_dft_p(1, :)=pilot(2, :);
data_dft_p(end, :)=pilot(1, :);
data_dft_p(2:end-1, :)=data_reshaped(:,:);
new=data_dft_p(pilot_pattern(1,1)+1:end, :);
new(pilot_pattern(1,1)+1:nr_car_used+2,:)=data_dft_p(1:pilot_pattern(1,1), :);

%Training for joint estimation
train_OFDM=modulation([train_OFDM(1:nr_car)],'qpsk');
nr_OFDM_symbols=nr_OFDM_symbols+1;
data_preOFDM=zeros(nr_car, nr_OFDM_symbols);
train_OFDM=c*train_OFDM;
data_preOFDM([1:2:end],1)=train_OFDM;
data_preOFDM(car_pattern_merged,2:end)=new; 

%OFDM. 
data_OFDM=OFDM(data_preOFDM,length_cpre,length_suf);

%TS
pulse_shape=ones(1,Q);
train_seq_upsamp=upfirdn(train_seq,ones(Q,1),Q,1);
serial_data=[d*train_seq_upsamp;data_OFDM];

%Sequence to send
tx_data=[zeros(nr_guard_bits,1); serial_data; zeros(nr_guard_bits,1)];

 figure(1);
 pwelch(tx_data);
 figure(2);
 plot(abs(tx_data))

tx_datab=tx_data(150:end);
PAPR=max(abs(tx_datab))^2/(tx_datab'*tx_datab/length(tx_datab))

%Save parameters and datas.
file_str=['tx_data_', num2str(n_sim),'.mat'];
file_strF=fullfile(fileparts('/data'),file_str);
save(file_str)
