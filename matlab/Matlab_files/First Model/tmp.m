clear all
close all
clc

%Comments:
%     1. We should take care of the fact that we don't know when the TX
%     will transmitt, we assume that the first sample of rx_bb is correct

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
  
%Model of LO 
  tx=G_TX*tx;
%% ----------------------------Channel-------------------------------------

%Try to model a shift on the recieved vector.
w=0;
% w=randn(2*size(tx))+1i*randn(2*size(tx));
rx=tx+w;


%% ----------------------------Reciever------------------------------------
 %gain of the receiver
% Model of the LO 
  rx=G_RX*rx;

% Matched filter. 
mf_pulse_shape = fliplr(pulse_shape);
rx_bb=conv(mf_pulse_shape/Q,rx);

% Sampling time synchronization
[t_samp,phi_hat]=sync_catch(rx_bb, b, Q, nr_guard_bits);

%Downsample
rx_mod=rx_bb(t_samp:Q:t_samp+Q*(nr_data_bits/nr_bits_x_symbol+nr_training_bits)-1);
rx_mod2=rx_mod(nr_training_bits+1:end);

%Detection
rx_data=detect(rx_mod2);
plot(rx_mod2,'o')
%BER.
    pc=find(rx_data==data);
    pc=length(pc);
    pe=1-pc/length(data)