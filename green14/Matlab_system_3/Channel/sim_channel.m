fileTX=['tx_data_', num2str(n_sim),'.mat'];
load(fileTX);

sigma_sqr_awgn=1e-5;          %Power of each noise dimension
frequency_offset=[1e-3 0];  %First parameter+second 
channel_filter=[1 0.3];                     %Impulse response of the channel
channel_gain = 1;                   %Overall channel gain (it will be over each channel coefficient)

%Flags
phase_offset=0;   %Add uniformly distributed random phase offset
LO_leak=0;        %Add LO leakage
phase_noise=1;    %Add phase noise in the receiver


%% ----------------------------Channel-------------------------------------
%    Models:
%    1) AWGN noise
%    2) Frequency Offset
%    3) Phase Offset
%    3) LO_leakage
%    4) Impulse Response
%    5) Gain in function of frequency
%    6) Phase Noise

rx_data=channel_fct(tx_data, sigma_sqr_awgn, frequency_offset, phase_offset, LO_leak, channel_filter, channel_gain, phase_noise );
SNR=10*log10( sum(rx_data.*conj(rx_data))/(2*length(rx_data))/sigma_sqr_awgn)
%Save parameters and datas.
file_str=['rx_data_', num2str(n_sim),'.mat'];
file_strF=fullfile(fileparts('/data'),file_str);
save(file_str)
