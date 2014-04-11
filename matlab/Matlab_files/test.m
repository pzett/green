%clear all
close all
clc
EbN0_db = 0:8:70;                     % Eb/N0 values to simulate (in dB)
nr_bits_per_symbol = 2;             % Corresponds to k in the report
nr_guard_bits =0;                 % Size of guard sequence (in nr bits)
nr_data_bits =2000;               % Size of each data sequence (in nr bits)
nr_training_bits = 0;             % Size of training sequence (in nr bits)
nr_blocks=200;                     % The number of blocks to simulate
nr_errors = zeros(1, length(EbN0_db));   % Error counter
N_carriers=1000;
L_filter=200;
Q=4;

for snr_point = 1:length(EbN0_db)
  snr_point
  for blk=1:nr_blocks;
      
  % create filter
   filt=randn(1,L_filter-150);
      
  % Generate training sequence.
    b_train = training_sequence(nr_training_bits);
   
  % Generate random source data {0, 1}.
    b_data = random_data(nr_data_bits);
    
  % Generate guard sequence.
    b_guard = random_data(nr_guard_bits);
    
   % Multiplex training and data into one sequence.
    b_begin = [b_guard b_train]; 
    b_end=b_guard;
    
   % Map bits into complex-valued QPSK symbols.
    d_begin = qpsk(b_begin);
    d_end = qpsk(b_end);
    d_data=qpsk(b_data);
    d_data_OFDM=OFDM(d_data,N_carriers,L_filter,Q);

   % Multiplex training and data into one sequence.
    d=[d_begin; d_data_OFDM; d_end]; 
    
%    [a b]=periodogram(d_data_OFDM); 
%    plot(b,a);
%    figure
%    semilogy(b/(2*pi)-1/2,fftshift(a));
%    grid on
%    plotspectrum(d_data_OFDM);
%    figure
%    subplot(2,1,1)
%    plot(real(d_data_OFDM))
%    subplot(2,1,2)
%    plot(imag(d_data_OFDM))
%     
%     
%     
    
    %No praticular pluse: maybe might be changed later.
    tx=circshift([d; d],[0 76]);
    [ theta fi] = Synchronize_(tx, N_carriers, L_filter-1, 4);
    %%%
    %%% AWGN Channel
    %%%
    
    % Compute variance of complex noise according to report.
    sigma_sqr = 1^2 / nr_bits_per_symbol / 10^(EbN0_db(snr_point)/10);

    % Create noise vector.
    n = sqrt(sigma_sqr/2)*(randn(size(tx))+1j*randn(size(tx)));
    
    rx=filter(filt,1,tx)+n;
    t_samp_ideal=(nr_guard_bits+nr_training_bits)/nr_bits_per_symbol+1;
    r_ideal_OFDM=rx(t_samp_ideal:t_samp_ideal+length(d_data_OFDM)-1);
    
    %freq response of the filter
    H_f=fft(filt,N_carriers*Q);
    %H_f=H_f.*exp(1j*2*pi*(1/(2*Q))*(0:N_carriers*Q-1));
    
    %OFDM inverse + equalization
    r_ideal_equalized=OFDM_inv(r_ideal_OFDM,N_carriers,L_filter,H_f,Q);
    
   % %equalization
   % r_ideal_equalized=r_ideal./conj(H_f');
    
    
    bhat_ideal=detect(r_ideal_equalized(1:end));
    temp_ideal=bhat_ideal(1:nr_data_bits)~= b_data;
    nr_errors(snr_point) = nr_errors(snr_point) + sum(temp_ideal);
    
  end
end
BER=nr_errors/(nr_data_bits*nr_blocks);
semilogy(EbN0_db,BER)