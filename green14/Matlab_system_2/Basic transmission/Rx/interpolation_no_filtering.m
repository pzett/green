function [data_received_equalized, channel, channel_pilot] = interpolation_no_filtering( rx_data_reshaped, train, rx_train, phase_init, nr_carriers, nr_used_car, length_cpre, length_cpost, Q, pattern,pattern_pilot, data_pilot,nr_OFDM_symbols,nr_pilot,gain_pilot_0,theta_est,length_past, length_fut)
% !!!!!!!!!!!!!!!!!!!Attention!!!!!!!!!!!!!!!!!!!
%We should define more carfully the the variances of the Kalmamn filter
%INPUT
%  rx_data_reshaped     -   received OFDM frame [N(freq)xL(time)]
%  train                -   training sequence (TS), original
%  rx_train             -   received TS, downsampled
%  phase_init           -   phase estimate from function sync_catch
%  nr_carriers          -   total number of carriers
%  nr_used_carriers     -   number of used carriers
%  length_cpre          -   length of cyclic prefix
%  length_cpost         -   length of cyclic suffix
%  Q                    -   upsampling rate
%  pattern              -   allocation pattern for information carriers
%  pattern_pilot        -   allocation pattern for pilot tones
%  data_pilot           -   matrix with used pilot sequences
%  nr_OFDM_symbols      -   number of OFDM symbols
%  nr_pilot             -   number of pilot tones
%  gain_pilot_0         -   pilots' gain

% The function Kalman_lovers equalizes the received data, removing the frequency offsets
% It uses Kalman filters for estimation of the phase and channel
% gain respectively, using pilot sequences. The initialization coefficients
% are obtained from the training sequence. To obtain channel parameters at
% all frequencies, linear interpolation is used. It is done separately for
% channel phase and channel gain. It is performed in a smart way, as it takes
% into account that we don't allocate any information to high
% frequencies. Therefore, the data is first circularly shifted before the
% interpolation.

%% Initialisation of the coefficients for the Kalman filter
[rx_data] = OFDM_inv(rx_data_reshaped,nr_carriers, length_cpre,length_cpost);

%% pick the data corresponding to the different pattern
data_received=rx_data(pattern,:);
data_pilot_received=rx_data(pattern_pilot,:);
channel_pilot=data_pilot_received./data_pilot;                             % Estimation of the channel on the pilot (here we haven't filtered)
phase_pilot=angle(channel_pilot);                                          % take the phase
gain_pilot=abs(channel_pilot);                                             % take the gain

% tracking the phase and the gain using the pilot

channel_pilot=gain_pilot.*exp(2*pi*1i*phase_pilot);


%% linear interpolation of the channel estimates for the other channels
shift=min(find(pattern>nr_carriers/2),1);
shift_pilot=min(find(pattern_pilot>nr_carriers/2),1);
pattern_shift=circshift(pattern,[0 -(shift-1)]);
pattern_pilot_shift=circshift(pattern_pilot,[0 -(shift_pilot-1)]);
phase_pilot_shift=circshift(phase_pilot,[-(shift_pilot-1) 0]);
gain_pilot_shift=circshift(gain_pilot,[-(shift_pilot-1) 0]);
channel_shift=zeros(nr_used_car,nr_OFDM_symbols);
for i=pattern_shift(1):pattern_pilot_shift(1)-1
    if ~isempty(logical(pattern_shift==i))
        channel_shift(1,:)=gain_pilot_shift(1,:).*exp(1j*phase_pilot_shift(1,:));
    end
end


for i=1:nr_pilot-1
    if pattern_pilot_shift(i+1)>pattern_pilot_shift(i)+1
        [used_carriers_between_pilot,~,index]=intersect((pattern_pilot_shift(i)+1:(pattern_pilot_shift(i+1)-1)),pattern_shift);
        a=ones(length(index),1)*phase_pilot_shift(i,:);
        b=(used_carriers_between_pilot'-pattern_pilot_shift(i))/(pattern_pilot_shift(i+1)-pattern_pilot_shift(i))*(mod(phase_pilot_shift(i+1,:)-phase_pilot_shift(i,:)+pi,2*pi)-pi);
        channel_phase=a+b;
        channel_gain=(pattern_pilot_shift(i+1)-used_carriers_between_pilot')/(pattern_pilot_shift(i+1)-pattern_pilot_shift(i))*gain_pilot_shift(i,:)+(used_carriers_between_pilot'-pattern_pilot_shift(i))/(pattern_pilot_shift(i+1)-pattern_pilot_shift(i))*gain_pilot_shift(i+1,:);
        channel_shift(index,:)=channel_gain.*exp(1j*channel_phase);
    end
end
for i=pattern_pilot_shift(end)+1:pattern_shift(end)
    if ~isempty(find(pattern_shift==i))
        n=find(pattern_shift==i);
        channel_shift(n,:)=gain_pilot_shift(end,:).*exp(1j*phase_pilot_shift(end,:));
    end
end
channel=circshift(channel_shift,[shift-1 0]);

%% Removal of the frequency response from the estimate
data_received_equalized=data_received./channel;
end

