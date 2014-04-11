clear all
close all
clc

%Comments:
%     1. We should take care of the fact that we don't know when the TX
%     will transmitt, we assume that the first sample of rx_bb is correct

%% ----------------------Initialization------------------------------------
filter=load('filt.mat');
%load('good_train_10');
%train=data_qpsk;
%train=randn(100,1);
SNR = 1:5;                                                                 % Eb/N0 values to simulate (in dB)
nr_bits_x_symbol = 2;                                                      % Depends on the modulation (Binary, quaternary, etc...)
nr_carriers =256;
nr_used_car = 128;
nr_training_bits = 10;                                                     % Size of training sequence (in bits).
length_cp = 5;
nr_data_bits = 1024*4;                                                       % Size of each data sequence (in bits).
nr_guard_bits=10;
nr_blocks=10;
%     nr_blk_bits=1000;                                                    % Number of bits per block (in symbols)
%     length_blk_sym=nr_blk_bits/nr_bits_x_symbol;                         % Number of bits per block (in bits)
guard_symbols = 5;                                                        % Size of guard sequence, where we don't transmit anything (in symbols).
% Guard bits are appended to transmitted bits so
% that the transients in the beginning and end
% of received sequence do not affect the samples
% which contain the training and data symbols.
Q = 4;                                                                     % Number of samples per symbol in baseband.
%     nr_blocks=nr_data_bits/nr_bits_x_symbol/length_blk;  % The number of blocks to simulate, how to block the data.

nr_pilots=4;
vLO_TX=@(p) exp(1i*2*pi*0.57*(p-1));
vLO_RX=@(p) exp(1i*2*pi*0.6*(p-1));
G_RX=1;
G_TX=1;

nr_err_t_samp=zeros(1,length(SNR));
for m=1:length(SNR)
    m
%% ----------------------------Transmiter----------------------------------
%Design for a single frame each transmission for the moment.
for p=1:nr_blocks

%First step
data=generate_data(nr_data_bits);  %Generate data
%C=0.25;
%[train_seq]=C*train; %C*(randn(nr_training_bits,1)+j*randn(nr_training_bits,1)); %LFSR(6,[6 5],4);  %Generate the training sequence

%Modulations
data_qpsk=qpsk(data);
%train_qpsk=qpsk(train)

[data2, pattern]=car_pattern(data_qpsk, nr_carriers, nr_used_car, nr_pilots, 2);

% data1=reshape(data_qpsk, nr_used_car, length(data_qpsk)/nr_used_car);
% data2=zeros(nr_carriers, length(data_qpsk)/nr_used_car);
% pattern1 = [42 43 44 45 46 47 48 49  51 52 53 54 55 56 57 58  60 61 62 63 64 1 2 3  5 6 7 8 9 10 11 12  ];  %vectors to define the pattern of the frequency allocation
% pattern2 = [ 2 3 4 5 6 7 8 9  13 14 15 16 17 18 19 20    62 61 60 59 58 57 56 55  51 50 49 48 47 46 45 44 ];
% data2(pattern1,:)=data1;

% for l=1:nr_used_car
% 
%     data2(pattern1(l),:)=data1(l,:);
% 
% end

%pilot signals
% pilot=LFSR(5, [5 3],1);
% pilot=[pilot;0];
% pilot=qpsk(pilot);
% pilot_pattern = [50 59 4 13];             %which frequencies we use for pilots
% pil=zeros(59,16);
% for l=1:59
%     pil(l,:)=pilot;
% end
% data2(pilot_pattern,:)= pil(1);
data_OFDM=OFDM(data2,length_cp);

%Define pulse shape and sampling frequency. Remember that our Fs=25 MHz, so
%in the end sampling frequency would be Fs/Q. (only for training seauence)
pulse_shape=ones(1,Q);

%upsample trainnning
load('good_train_20_Q4.mat');
train=upfirdn(train_best,ones(Q,1),Q,1);

%Group data
serial_data=[train;data_OFDM];


%Sending: $$$$$$$$
tx=serial_data;
tx=[zeros(nr_guard_bits,1); tx; zeros(nr_guard_bits,1)];
%Model of LO
tx=G_TX*tx;
%% ----------------------------Channel-------------------------------------

%Try to model a shift on the recieved vector.
w0=(serial_data'*serial_data)/length(serial_data)*10^(-SNR(m)/10);
w=w0*(randn(2*length(tx),1)+1i*randn(2*length(tx),1))/sqrt(2);
rx=[tx;tx]+w;


%% ----------------------------Reciever------------------------------------
%gain of the receiver
% Model of the LO
k=(1:length(rx));
%plot(abs(fft(rx)))
%figure
%hold on
rx=G_RX*rx.*conj((vLO_TX(k).*conj(vLO_RX(k)))')+0.01*conj(vLO_RX(k)')+0.01*conj(vLO_TX(k)');
%plot(abs(fft(rx)))
% % Frequency estimation
% rx_f=abs(fft(rx,ceil(2^log2(length(rx)))));
% v_offs=get_fOffset(rx_f);
% 
% % Frequency correction & filtering
% k=0:1:length(rx)-1;
% v_corr=exp(1i*2*pi*v_offs*k);
% rx_bb=rx.*v_corr';
% rx_bb=rx_bb-conj(vLO_RX(k)')+conj(vLO_TX(k)');

rx_bb=rx;
%plot(abs(fft(conj((vLO_TX(k).*conj(vLO_RX(k)))'))),'red')
% Sampling time synchronization
[t_samp,phi_hat]=sync_catch(rx_bb, train); %revise it

t_start=t_samp+Q*nr_training_bits;
t_end=t_start+(nr_carriers+length_cp)/nr_used_car*nr_data_bits/nr_bits_x_symbol;
rx_data=rx_bb(t_start+1:t_end);

rx_data_reshaped=reshape(rx_data,nr_carriers+length_cp,length(rx_data)/(nr_carriers+length_cp));
%Demodulation
%should be modified in order to integrate the freauency offset: use an
%estimation using cyclic prefix estimation
[rx_data] = OFDM_inv(rx_data_reshaped,nr_carriers, length_cp);

%Estimation of the frequency response using  pilot


%Removing the estimate the frequency response from data (deviding by it)
if t_samp~=10
    nr_err_t_samp(m)=nr_err_t_samp(m)+1;
end

%Detection
end
end

rx_data=detect(rx_mod);

%plot(rx_mod,'rx_mod');
