clear all
close all
clc

%Comments:
%     1. We should take care of the fact that we don't know when the TX
%     will transmitt, we assume that the first sample of rx_bb is correct
%     2. The v_offs is not numerically perfect, that's why we have a
%     rotation in the plot.
%% ----------------------Initialization------------------------------------
    filter=load('filt.mat');
    EbN0_db = 0:20;                               % Eb/N0 values to simulate (in dB)
    nr_bits_x_symbol = 2;                         % Depends on the modulation (Binary, quaternary, etc...)

    nr_training_bits = 63;                        % Size of training sequence (in bits).
    nr_data_bits = 1000;                          % Size of each data sequence (in bits).
    nr_guard_bits=0;
%     nr_blk_bits=1000;                             % Number of bits per block (in symbols)
%     length_blk_sym=nr_blk_bits/nr_bits_x_symbol;  % Number of bits per block (in bits)
%     guard_symbols = 5;                            % Size of guard sequence, where we don't transmit anything (in symbols).
                                                  % Guard bits are appended to transmitted bits so
                                                  % that the transients in the beginning and end
                                                  % of received sequence do not affect the samples
                                                  % which contain the training and data symbols.
    Q = 8;                                        % Number of samples per symbol in baseband.
%     nr_blocks=nr_data_bits/nr_bits_x_symbol/length_blk;  % The number of blocks to simulate, how to block the data.
    
    
    vLO_TX=@(p) exp(1i*2*pi*0.58*(p-1));
    vLO_RX=@(p) exp(1i*2*pi*0.6*(p-1));
    G_RX=1;
    G_TX=1;
    delay=10;
    EbN0=10; %in dB
%% ----------------------------Transmiter----------------------------------
%Design for a single frame each transmission for the moment.


%First step
  data=generate_data(nr_data_bits);  %Generate data
  [train_seq,~]=LFSR(6,[6 5],4);  %Generate the training sequence

%Modulations
  a=qpsk(data);
  b=bpsk(train_seq);

%Group data
  serial_data=[b;a];

%Define pulse shape and sampling frequency. Remember that our Fs=25 MHz, so
%in the end sampling frequency would be Fs/Q.
  pulse_shape=ones(1,Q);
  
  
%Sending: $$$$$$$$
  tx=upfirdn(serial_data, pulse_shape, Q, 1);
%   tx=[tx;zeros(nr_guard_bits/nr_bits_x_symbol)];
%Model of LO 
  tx=G_TX*tx;
%% ----------------------------Channel-------------------------------------

%Try to model a shift on the recieved vector.
% w=0;
sigma=1/10^(EbN0/10);
w=sigma*randn(2*length(tx),1)+1i*sigma*randn(2*length(tx),1);
rx=[tx;tx]+w;
rx=[zeros(delay,1);rx];
pH=pi/2*rand(length(rx),1);

%% ----------------------------Reciever------------------------------------
 %gain of the receiver
% Model of the LO 
  k=1:1:length(rx);
  rx=G_RX*rx.*conj((vLO_TX(k).*conj(vLO_RX(k)))').*exp(1i*pH);

% Frequency estimation
  rxL=conj(vLO_RX(k)')+conj(vLO_TX(k)');
  rx_f=abs(fft(rxL,ceil(2^log2(length(rx)))));
  v_offs=get_fOffset(rx_f);
  
% Frequency correction & filtering
  k=0:1:length(rx)-1;
  v_corr=exp(-1i*2*pi*v_offs*k);
  rx_bb=rx.*conj(v_corr');

% Matched filter. 
  mf_pulse_shape = fliplr(pulse_shape);
  rx_bb=conv(mf_pulse_shape/Q,rx_bb);

% Sampling time synchronization
  [t_samp,phi_hat]=sync_catch(rx_bb, b, Q, nr_guard_bits);

%Downsample
  rx_mod=rx_bb(t_samp:Q:t_samp+Q*(nr_data_bits/nr_bits_x_symbol+nr_training_bits)-1);
  
%Phase correction
rx_mod2=filter_phase(rx_mod, phi_hat, b, pi^2/8, 10^-3, 10^-3);

%Detection
  rx_data=detect(rx_mod2);
  plot(rx_mod2,'o')
  
%BER.
    pc=find(rx_data==data);
    pc=length(pc);
    pe=1-pc/length(data);