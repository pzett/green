
clear 
close all
clc
nr_car =64;                            %Number of total carriers
nr_training_bits = 12;                 %Number of training bits in the begining of the frame (no OFDM)
length_cpre = 10;                       %Prefix length
length_suf = 4;                        %Suffix to cope with the influence of the future samples on the estimate
train_seq=golay_generator(nr_training_bits);                 %Training sequenceSNR = 0;                             %Eb/N0 values to simulate (in dB)
nr_data_bits =100000;                    %Number of data bits
nr_guard_bits=10;                      %Size of guard sequence, where we don't transmit anything (in symbols) attached at the begining and at the end of the whole frame.
ispilot=0;                             %Flag for knowing if we transmit pilots
Q = 2;                                 %Upsampling rate for the training sequence
pilot_ampl=9.6;    %for 16_QAM
car_pattern_merged=[(1:24) (39:64)]; 
%Carriers with data
pilot_pattern=[24 39];
car_pattern=[(1:23) (40:64)];
dft_window=48;
nr_car_used=length(car_pattern);
length_pilot=length(pilot_pattern);    %Length of the pilot
nr_blocks=1;                           %Number of data blocks to transmit
modulation_type='qpsk';                %Modulation for data
pilot_type='random_phase';             %Choose from 'random', 'modulated', or 'random_phase'
decoding_method='LLH_est_av';          %For the moment just one option
[nr_bits_x_symbol]=test_constellation(modulation_type);                       %Compute the bits/symbols of the constellation
[nr_used_car,nr_pilot_car]=check_pattern(car_pattern, pilot_pattern, nr_car); %This function checks the carrier assignment in order to avoid collisions.
k=nr_used_car*nr_bits_x_symbol-(mod(nr_data_bits-1,nr_used_car*nr_bits_x_symbol)+1);  %Number of zeros added in order to fill the OFDM matrix

%----------Channel Parameters----------


frequency_offset=[0.01 0.00001];      %First parameter=mean, second=variance(uniforme distribution). Can handle means lower than 5e-3
channel_filter=[1 0.1];               %Impulse response of the channel
channel_gain = 1;                 %Overall channel gain (it will be over each channel coefficient)
%Flags
phase_offset=1;   %Add uniformly distributed random phase offset
LO_leak=0;        %Add LO leakage
phase_noise=0;    %Add phase noise in the receiver
length_past=4;
length_fut=1;
train2=LFSR(6,[6 5],3);
train2=[train2' 0];
data=generate_data(nr_data_bits);
nr_OFDM_symbols = floor((length(data)-1)/(nr_used_car*nr_bits_x_symbol))+1;
data=[data rand(1,k)];

%Modulations
data_mapped=modulation(data, modulation_type);
data_dft = DFT_precode( data_mapped, dft_window, 'encode' ); for 16_QAM ne
data_reshaped=reshape(data_dft, nr_used_car, length(data_dft)/nr_used_car); %Modify data to allocate it for each frequency band. Col=frequency, Row=time.
%Pilots
pilot=pilot_ampl*generate_pilot(size(data_reshaped,2), pilot_type, pilot_pattern);     %Get pilots
data_dft_p=zeros(size(data_reshaped, 1)+length(pilot_pattern), size(data_reshaped, 2));
data_dft_p(1, :)=pilot(1, :);
data_dft_p(end, :)=pilot(2, :);
data_dft_p(2:end-1, :)=data_reshaped(:,:);
data_preOFDM=zeros((nr_car+length(pilot_pattern)), nr_OFDM_symbols);
new=zeros(nr_car_used+length(pilot_pattern),nr_OFDM_symbols);
new(1:pilot_pattern(1,1), :)=data_dft_p(pilot_pattern(1,1)+length(pilot_pattern)+1:end, :);
new(pilot_pattern(1,1)+length(pilot_pattern):nr_car_used+2,:)=data_dft_p(1:pilot_pattern(1,1)+1, :);
%Training for joint estimation
train2=modulation([train2(1:nr_car)],'qpsk');
nr_OFDM_symbols=nr_OFDM_symbols+1;
data_preOFDM=zeros(nr_car, nr_OFDM_symbols);
data_preOFDM([1:2:end],1)=10*train2;
data_preOFDM(car_pattern_merged,2:end)=new; 

%OFDM. 
data_OFDM=OFDM(data_preOFDM,length_cpre,length_suf);

%TS
pulse_shape=ones(1,Q);
%Upsample training
train_seq_upsamp=upfirdn(train_seq,ones(Q,1),Q,1);
%Group data
serial_data=[0.7*train_seq_upsamp;data_OFDM];
%Sending: $$$$$$$$
tx_data=[zeros(nr_guard_bits,1); serial_data; zeros(nr_guard_bits,1)];
figure(1)

plot((abs(fft(tx_data))))
figure();
pwelch(tx_data);
PAPR=max(abs(tx_data))^2/(tx_data'*tx_data/length(tx_data))
file_str=['constant_parameters', modulation_type,num2str(nr_car),'.mat'];

%save(file_str)