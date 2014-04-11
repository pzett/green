%% ----------------------------Reciever Simulation for harness-------------


 %Comments:
%     1. We should take care of the fact that we don't know when the TX
%     will transmitt, we assume that the first sample of rx_bb is correct
%     2. The v_offs is not numerically perfect, that's why we have a
%     rotation in the plot.
%% ----------------------Initialization------------------------------------
    nr_bits_x_symbol = 2;                         % Depends on the modulation (Binary, quaternary, etc...)

    nr_training_bits = 100;                        % Size of training sequence (in bits).
    nr_data_bits = 6250;                          % Size of each data sequence (in bits).
    nr_guard_bits=10;

    %     nr_blk_bits=1000;                             % Number of bits per block (in symbols)
%     length_blk_sym=nr_blk_bits/nr_bits_x_symbol;  % Number of bits per block (in bits)
%     guard_symbols = 5;                            % Size of guard sequence, where we don't transmit anything (in symbols).
                                                  % Guard bits are appended to transmitted bits so
                                                  % that the transients in the beginning and end
                                                  % of received sequence do not affect the samples
                                                  % which contain the training and data symbols.
    Q = 4;                                        % Number of samples per symbol in baseband.
%     nr_blocks=nr_data_bits/nr_bits_x_symbol/length_blk;  % The number of blocks to simulate, how to block the data.

    G_RX=1;
    G_TX=1;
    pulse_shape=ones(1,Q);


%% ----------------------------Reciever------------------------------------

% Frequency estimation

  rx_f=abs(fft(rx_data,2^16));
  v_offs=get_fOffset(rx_f);
  
% Frequency correction & filtering
  k=0:1:length(rx_data)-1;
  v_corr=exp(-1i*2*pi*v_offs*k);
  rx_bb=rx_data.*conj(v_corr');
  
    length(rx_bb)
% Matched filter. :mmq
  mf_pulse_shape = fliplr(pulse_shape);
  rx_bb_conv=conv(mf_pulse_shape/Q,rx_bb);
    length(rx_bb_conv)
    plot(imag(rx_bb_conv))
fefe
% Sampling time synchronization
  [t_samp,phi_hat]=sync_catch(rx_bb, train, Q, nr_guard_bits);

%Downsample
  rx_mod=rx_bb(t_samp:Q:t_samp+Q*(nr_data_bits/nr_bits_x_symbol+nr_training_bits)-1);
  %rx_mode=rx_bb(t_samp:1:t_samp+Q*(nr_data_bits/nr_bits_x_symbol+nr_training_bits)-1);
%Phase correction
  rx_mod2=filter_phase(rx_mod, phi_hat, train, 0.01, 0.01, 0.01);
  
%Detection
  rx_data_detect=detect(rx_mod2);
  
  plot(rx_data_detect)