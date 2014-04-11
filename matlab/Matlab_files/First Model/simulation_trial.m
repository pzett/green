% Skeleton code for simulation chain

% History:
%   2000-06-28  written /Stefan Parkvall
%   2001-10-22  modified /George Jongren

clear all

% Initialization
EbN0_db = 0:20;                     % Eb/N0 values to simulate (in dB)
nr_bits_per_symbol = 2;             % Corresponds to k in the report
nr_guard_bits = 10;                 % Size of guard sequence (in nr bits)
                                    % Guard bits are appended to transmitted bits so
                                    % that the transients in the beginning and end
                                    % of received sequence do not affect the samples
                                    % which contain the training and data symbols.
nr_data_bits = 1000;               % Size of each data sequence (in nr bits)
nr_training_bits = 10;             % Size of training sequence (in nr bits)
nr_blocks =20;                     % The number of blocks to simulate
Q = 8;                              % Number of samples per symbol in baseband

% Define the pulse-shape used in the transmitter. 
% Pick one of the pulse shapes below or experiemnt
% with a pulse of your own.
pulse_shape = ones(1, Q);
%pulse_shape = root_raised_cosine(Q);

% Matched filter impulse response. 
mf_pulse_shape = fliplr(pulse_shape);

% Loop over different values of Eb/No.
nr_errors = zeros(1, length(EbN0_db));   % Error counter
phi_err=zeros(1, length(EbN0_db));   % Error on phi
sync_err=zeros(1, length(EbN0_db));   % Error on phi
nr_errors_ideal= zeros(1, length(EbN0_db));
for snr_point = 1:length(EbN0_db)
  snr_point
  % Loop over several blocks to get sufficient statistics.
  %blk=0;
  %while ((blk<nr_blocks)||(nr_errors(snr_point)<200))&&(blk<10*nr_blocks)
  %blk=blk+1
  %nr_errors(snr_point)
  for blk=1:nr_blocks;
  % blk
    %%%
    %%% Transmitter
    %%%

    % Generate training sequence.
%     b_train = LFSR(9, [9 2],1);
    b_train = [1 1 1 0 0 0 0 1];
    
    % Generate random source data {0, 1}.
    b_data = random_data(nr_data_bits);

    % Generate guard sequence.
    b_guard = random_data(nr_guard_bits);
 
    % Multiplex training and data into one sequence.
    b = [b_guard b_train b_data b_guard];
    
    % Map bits into complex-valued QPSK symbols.
    d = qpsk(b);

    % Upsample the signal, apply pulse shaping.
    tx = upfirdn(d, pulse_shape, Q, 1);

    %%%
    %%% AWGN Channel
    %%%
    
    % Compute variance of complex noise according to report.
    sigma_sqr = norm(pulse_shape)^2 / nr_bits_per_symbol / 10^(EbN0_db(snr_point)/10);

    % Create noise vector.
    n = sqrt(sigma_sqr/2)*(randn(size(tx))+1j*randn(size(tx)));

    % Received signal.
    %phi=(rand(1,1)-0.5)*2*pi;
    phi=0;
    rx = exp(1j*(phi))*tx+n;
%     subplot(2,1,1)
%     plot((-1/2:1/(2*length(tx)-1):1/2-1/(2*length(tx)-1)),abs(fftshift(fft(xcorr(tx))))/length(tx));
%     title('Power spectrum of the transmited signal transmited with retangular pulse shape')
%     xlabel('reduced frequency (high sample rate)')
    
    %%%
    %%% Receiver
    %%%
    
    % Matched filtering.
    mf=conv(mf_pulse_shape,rx);
    
    % Synchronization. The position and size of the search window
    % is here set arbitrarily. Note that you might need to change these
    % parameters. Use sensible values (hint: plot the correlation
    % function used for syncing)! 
    t_start=1+Q*nr_guard_bits/2;
    t_end=t_start+50;
    %t_samp_ideal =48;
    t_samp = sync(mf, b_train, Q, t_start, t_end);
    sync_err(snr_point)=sync_err(snr_point)+abs(t_samp-48);
    
    % Down sampling. t_samp is the first sample, the remaining samples are all
    % separated by a factor of Q. Only training+data samples are kept.
    r_ideal=mf(t_samp:Q:t_samp+Q*(nr_training_bits+nr_data_bits)/2-1);
    %r = mf(t_samp:Q:t_samp+Q*(nr_training_bits+nr_data_bits)/2-1);
    bhat_ideal=detect(r_ideal*exp(-1j*(phi)));
    temp_ideal=bhat_ideal(1+nr_training_bits:nr_training_bits+nr_data_bits) ~= b_data;
    nr_errors_ideal(snr_point) = nr_errors_ideal(snr_point) + sum(temp_ideal);
    
    % Phase estimation and correction.
   % phihat = phase_estimation(r, b_train);
    %phi_err(snr_point)=phi_err(snr_point)+abs(mod(phihat-phi+pi,2*pi)-pi);
    
    %r = r * exp(-1j*(phihat));
  
    % Make decisions. Note that dhat will include training sequence bits
    % as well.
   % bhat = detect(r);
    
    % Count errors. Note that only the data bits and not the training bits
    % are included in the comparison. The last data bits are missing as well
    % since the whole impulse response due to the last symbol is not
    % included in the simulation program above.
   % temp=bhat(1+nr_training_bits:nr_training_bits+nr_data_bits) ~= b_data;
   % nr_errors(snr_point) = nr_errors(snr_point) + sum(temp);
    % Next block.
  end

  % Next Eb/No value.
end
%phi_err2=phi_err/blk;
sync_err2=sync_err/blk/Q;


% Compute the BER. 
%BER2 = nr_errors / nr_data_bits / blk;
BER_ideal = nr_errors_ideal / nr_data_bits / blk;
%load saved200train.mat
% figure
% subplot(3,1,1)
% plot(EbN0_db,(BER2))
% hold on
% plot(EbN0_db,(BER_ideal),'red')
% %hold on
% %plot(EbN0_db,(BER),'green')
% title('BER')
% subplot(3,1,2)
% %plot(EbN0_db,phi_err)
% %hold on
% plot(EbN0_db,phi_err2,'red')
% title('Mean error on phi estimation')
% subplot(3,1,3)
% %plot(EbN0_db,sync_err)
% %hold on
 plot(EbN0_db,sync_err2,'red')
 title('Mean error on the optimal sampling time estimation')
 xlabel('Eb/No in dB')