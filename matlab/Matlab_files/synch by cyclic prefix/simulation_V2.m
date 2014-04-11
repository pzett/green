% Skeleton code for simulation chain
% History:
%   2000-06-28  written  /Stefan Parkvall
%   2001-10-22  modified /George Jongren
%   2012-02-13  modified /Daniel Anfelt

%clear all
 clc
close all

% Initialization
tic
load('training')
EbN0_db = 10;%:1:-0;                % Eb/N0 values to simulate (in dB)
nr_bits_per_symbol = 2;             % Corresponds to k in the report
nr_guard_bits = 0;                 % Size of guard sequence (in nr bits)
                                    % Guard bits are appended to transmitted bits so
                                    % that the transients in the beginning and end
                                    % of received sequence do not affect the samples
                                    % which contain the training and data symbols.
nr_data_bits = 1000;                % Size of each data sequence (in nr bits)
cyclic_prefix = 10;                % Size of cyclic prefix
nr_training_bits = 50;              % Size of training sequence (in nr bits)
nr_blocks = 50;                     % The number of blocks to simulate
Q = 4;                              % Number of samples per symbol in baseband

% Below, we sum up size of packets for use in qpsk

% Define the pulse-shape used in the transmitter. 
% Pick one of the pulse shapes below or experiemnt
% with a pulse of your own.


pulse_shape = ones(1, Q);
% pulse_shape = root_raised_cosine(Q);

% Matched filter impulse response. 
mf_pulse_shape = fliplr(pulse_shape);

% Loop over different values of Eb/No.
nr_errors = zeros(1, length(EbN0_db));   % Error counter


for snr_point = 1:length(EbN0_db)
  
  % Loop over several blocks to get sufficient statistics.
  % for blk = 1:nr_blocks
%% Transmitter
  
    % Generate training sequence.
    % b_train = training_sequence(nr_training_bits);
    b = [];
   % for i = 1:5;
    % Generate random source data {0, 1}.
    b_data = (rand(1,nr_data_bits)>0.5);

    % Generate guard sequence.
     b_guard =  (rand(1,nr_guard_bits)>0.5);
    b_cyc_prefix = b_data(nr_data_bits-cyclic_prefix+1:nr_data_bits);
 
    % Multiplex training and data into one sequence.
   % b_block = [b_cyc_prefix b_data];
    
    b = [b_data];    % concatenate
  %  end
    % Map bits into complex-valued QPSK symbols.
    d = qpsk(b);
    d=[conj(train(end-10:end)'); conj(train'); d; conj(train(end-10:end)'); conj(train'); d];
    
    

%% Channel
theta_vec = zeros(20,1);
fi_vec = zeros(20,1);

for shift = 1:1:40;
% Upsample the signal, apply pulse shaping.
%tx = d;    
tx = upfirdn(d, pulse_shape, Q, 1);
%train_up=upfirdn(train, pulse_shape, Q, 1);
nu =0; %shift*0.00001;
time=(0:length(tx)-1);
tx_freqoffset=tx.*exp(1j*nu*2*pi*time');
tx_freqoffset= circshift(tx_freqoffset,[650 0]);

    %%%
    %%% AWGN Channel
    %%%
% subplot(1,2,1); 
% freqtxoffset = abs(fft(tx_freqoffset));
% plot(freqtxoffset(1:length(freqtxoffset)/2));
% subplot(1,2,2);
% freqtx = abs(fft(tx));
% plot(freqtx(1:length(freqtx)/2));




%% Define noise

    % Compute variance of complex noise according to report.
    sigma_sqr = norm(pulse_shape)^2 / nr_bits_per_symbol / 10^(EbN0_db(snr_point)/10);

    % Create noise vector.
    n = sqrt(sigma_sqr/2)*(randn(size(tx))+1i*randn(size(tx)));
    
%% Receive

    % Received signal.
    rx1 = tx; % + n;
 %  rx2 = filter(ones(Q,1),[1],tx_freqoffset);
rx2 = tx_freqoffset;
    % alf1 = Synchronize_(rx1,nr_data_bits/2, cyclic_prefix/2,Q);
    [theta fi] = Synchronize_V2(rx2,train,Q);
    theta_vec(shift) = theta;
    fi_vec(shift) = fi;
end  
    % Receiver known b_train in all forms and shapes
    b_train_qpsk = d(length(b_guard)/2+1:(length(b_guard)+length(b_train))/2);


%     % Matched filtering.
%     mf=conv(mf_pulse_shape,rx);
% 
%     % Synchronization. Start test at t_start, right after the guard bits
%     % Test shift of 3*Q elements
%     t_start=1+Q*nr_guard_bits/2;  % start search here
%     t_end=t_start+3*Q;            % end here
%     t_samp = sync(mf, b_train_qpsk, Q, t_start, t_end, nr_guard_bits);
% 
%     
%     % Down sampling. t_samp is the first sample, the remaining samples are all
%     % separated by a factor of Q. Only training+data samples are kept.
%     r = mf(t_samp:Q:t_samp+Q*(nr_training_bits+nr_data_bits)/2-1);
%     % vector r now consists of training bits and data bits in that order
% 
%     % Phase estimation and correction.
%     phihat = phase_estimation(r, b_train_qpsk);
%     
% 
%     r = r * exp(-1i*phihat);
% 
%     plot(r,'.')
%     % the resulting r is the best version of b_train + b_data we can muster
% 
%     % Make decisions. Note that dhat will include training sequence bits
%     % as well.
% 
%     bhat = detect(r); % bhat = [b_train + b_data]
%         
%     % Count errors. Note that only the data bits and not the training bits
%     % are included in the comparison. The last data bits are missing as well
%     % since the whole impulse response due to the last symbol is not
%     % included in the simulation program above.
%     temp=bhat(1+nr_training_bits:nr_training_bits+nr_data_bits) ~= b_data;
% 
%     nr_errors(snr_point) = nr_errors(snr_point) + sum(temp);
% 
%     % Next block.
%    end
% 
%   % Next Eb/No value.
 end
% toc
% % Compute the BER. 
% grid
% BER = nr_errors / nr_data_bits / nr_blocks;
