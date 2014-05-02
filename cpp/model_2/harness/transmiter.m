function [tx_data, data,data_OFDM] = transmiter

load('parameters')
modulation_type='qpsk';
nr_bits_x_symbol=2;


%First step: generate data
if (~exist('data'))
data=generate_data(nr_data_bits);
else
data=[data generate_data(nr_data_bits-length(data))];   
end
%%Coding the data
% data_coded_1=data;
% data_coded_2=Convolute(data,M_generatrice,memory,number_output,number_input);
% data_coded_3=Convolute(data(PI),M_generatrice,memory,number_output,number_input);
% data_coded_1_ponc=data_coded_1(logical(mask1));
% data_coded_2_ponc=data_coded_2(logical(mask2));
% data_coded_3_ponc=data_coded_3(logical(mask3));
% data_coded_1_ponc_reshaped=reshape(data_coded_1_ponc,m_p1,length(data_coded_1_ponc)/leng_p);
% data_coded_2_ponc_reshaped=reshape(data_coded_2_ponc,m_p2,length(data_coded_1_ponc)/leng_p);
% data_coded_3_ponc_reshaped=reshape(data_coded_3_ponc,m_p3,length(data_coded_1_ponc)/leng_p);
% data_coded_shaped=[data_coded_1_ponc_reshaped; data_coded_2_ponc_reshaped; data_coded_3_ponc_reshaped];
% data_coded=reshape(data_coded_shaped,1,size(data_coded_shaped,1)*size(data_coded_shaped,2));



%% add zeros in order to
k=nr_used_car-(mod(nr_data_bits/nr_bits_x_symbol-1,nr_used_car)+1);
data_coded=data;
nr_OFDM_symbols = floor((length(data_coded)-1)/(nr_used_car*nr_bits_x_symbol))+1;


%Modulations
data_mapped=modulation(data_coded, modulation_type);
data_mapped=[conj(data_mapped') zeros(1,k)];
data_reshaped=reshape(data_mapped, nr_used_car, length(data_mapped)/nr_used_car); %Modify data to allocate it for each frequency band. Col=frequency, Row=time.
data_preOFDM=zeros(nr_car, length(data_mapped)/nr_used_car);                      %Prepare the OFDM matrix. Col=frequency, Row=time.
data_preOFDM(car_pattern,:)=data_reshaped;                                        %Allocate data in the defined subcarriers.

%Pilot signals
data_preOFDM(pilot_pattern,:)= pilot(:,1:nr_OFDM_symbols);                                      %Allocate pilots in the respective subcarriers

%OFDM. We get data in a columnwise vector
data_OFDM=OFDM(data_preOFDM,length_cpre,length_suf);

%Define pulse shape and sampling frequency FOR THE TRAINING SEQUENCE.
%Remember that our Fs=25 MHz, so in the end sampling frequency would be Fs/Q.
%The Fs for data is 25MHz.
pulse_shape=ones(1,Q);

%Upsample training
train_seq_upsamp=upfirdn(train_seq,ones(Q,1),Q,1);

%Group data
serial_data=[train_seq_upsamp;data_OFDM];

%Sending: $$$$$$$$
tx_data=[zeros(nr_guard_bits,1); serial_data; zeros(nr_guard_bits,1)];
data_rate=nr_data_bits/length(tx_data)*25;
['Data rate: ', num2str(data_rate), 'Mbits/s'];
tx_info=data;
save('tx_info','tx_info');
save('tx_data','tx_data');