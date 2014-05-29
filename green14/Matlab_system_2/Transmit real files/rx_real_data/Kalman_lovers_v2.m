function [data_received_equalized, channel, channel_pilot] = Kalman_lovers_v2( rx_data_reshaped, train, rx_train, phase_init, nr_carriers, nr_used_car, length_cpre, length_cpost, Q, pattern,pattern_pilot, data_pilot,nr_OFDM_symbols,nr_pilot,gain_pilot_0,theta_est,length_past, length_fut)
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

theta_c=[theta_est(length_fut+1:end) zeros(1,nr_carriers-(length_fut+length_past+1)) theta_est(1:length_fut)];
a=fft(theta_c);
gain_pilot_0=abs(a(pattern_pilot));
phase_pilot_0=0;
Q0=[10 0;0 1];

state_phase_pilot_filt=zeros(2,nr_OFDM_symbols,nr_pilot);
gain_pilot_filt=zeros(nr_OFDM_symbols,nr_pilot);

% tracking the phase and the gain using the pilot
channel_pilot=zeros(nr_pilot,nr_OFDM_symbols);
for i=1:nr_pilot
    [~,state_phase_pilot_filt(:,:,i),~,~,Q01] = kalman_phase(conj(phase_pilot(i,1:end)'),[1 2*pi;0 1],[1 0;0 1],[1 0],[0.1 0;0 0.000001],0.1,phase_pilot_0,0,Q0,0);
    [~,gain_pilot_filt(:,i),~,~,Q02] = kalman(gain_pilot(i,:).',1,1,1,0.001,0.1,gain_pilot_0(i),3,1);
    channel_pilot(i,:)=gain_pilot_filt(:,i)'.*exp(1i*state_phase_pilot_filt(1,:,i));
end

%% linear interpolation of the channel estimates for the other channels
channel_gain=zeros(nr_OFDM_symbols,nr_carriers);
channel_phase=zeros(nr_OFDM_symbols,nr_carriers);


phase_pilot_filt=angle(channel_pilot).';

middle=find((pattern_pilot-1)<nr_carriers/2,1,'last');
index_middle=pattern_pilot(middle);

%% Between last and first pilot
step_phase_tot=mod(phase_pilot_filt(:,1)-phase_pilot_filt(:,end)+pi,2*pi)-pi;
step_gain_tot=gain_pilot_filt(:,1)-gain_pilot_filt(:,end);

delta_index=mod(pattern_pilot(1)-pattern_pilot(end),nr_carriers);

step_phase=step_phase_tot./delta_index;
step_gain=step_gain_tot./delta_index;

%fill the left part of channel
n_interp=[0:1:nr_carriers-pattern_pilot(end)];
channel_gain(:,pattern_pilot(end):nr_carriers)=gain_pilot_filt(:,end)*ones(1,length(n_interp))+(step_gain*ones(1,length(n_interp))).*(ones(nr_OFDM_symbols,1)*n_interp);
channel_phase(:,pattern_pilot(end):nr_carriers)=mod(phase_pilot_filt(:,end)*ones(1,length(n_interp))+(step_phase*ones(1,length(n_interp))).*(ones(nr_OFDM_symbols,1)*n_interp)+pi,2*pi)-pi;

% fill the right hand side
n_interp=[1:1:pattern_pilot(1)];
channel_gain(:,1:pattern_pilot(1))=channel_gain(:,end)*ones(1,length(n_interp))+(step_gain*ones(1,length(n_interp))).*(ones(nr_OFDM_symbols,1)*n_interp);
channel_phase(:,1:pattern_pilot(1))=mod(channel_phase(:,end)*ones(1,length(n_interp))+(step_phase*ones(1,length(n_interp))).*(ones(nr_OFDM_symbols,1)*n_interp)+pi,2*pi)-pi;

%% Pilots in the high freq
for i=1:middle-1
    step_phase_tot=mod(phase_pilot_filt(:,i+1)-phase_pilot_filt(:,i)+pi,2*pi)-pi;
    step_gain_tot=gain_pilot_filt(:,i+1)-gain_pilot_filt(:,i);
    
    delta_index=mod(pattern_pilot(i+1)-pattern_pilot(i),nr_carriers);
    
    step_phase=step_phase_tot./delta_index;
    step_gain=step_gain_tot./delta_index;
    
    %fill the high freq
    n_interp=[0:1:pattern_pilot(i+1)-pattern_pilot(i)];
    channel_gain(:,pattern_pilot(i):pattern_pilot(i+1))=gain_pilot_filt(:,i)*ones(1,length(n_interp))+(step_gain*ones(1,length(n_interp))).*(ones(nr_OFDM_symbols,1)*n_interp);
    channel_phase(:,pattern_pilot(i):pattern_pilot(i+1))=mod(phase_pilot_filt(:,i)*ones(1,length(n_interp))+(step_phase*ones(1,length(n_interp))).*(ones(nr_OFDM_symbols,1)*n_interp)+pi,2*pi)-pi;
end

%% Points in really high freq (hold)
channel_gain(:,pattern_pilot(middle):nr_carriers/2)=gain_pilot_filt(:,middle)*ones(1,nr_carriers/2-pattern_pilot(middle)+1);
channel_phase(:,pattern_pilot(middle):nr_carriers/2)=mod(phase_pilot_filt(:,middle)*ones(1,nr_carriers/2-pattern_pilot(middle)+1)+pi,2*pi)-pi;

%% Points in really low freq (hold)
channel_gain(:,nr_carriers/2+1:pattern_pilot(middle+1))=gain_pilot_filt(:,middle+1)*ones(1,pattern_pilot(middle+1)-(nr_carriers/2+1)+1);
channel_phase(:,nr_carriers/2+1:pattern_pilot(middle+1))=mod(phase_pilot_filt(:,middle+1)*ones(1,pattern_pilot(middle+1)-(nr_carriers/2+1)+1)+pi,2*pi)-pi;

%% Pilots in the low freq
for i=middle+1:nr_pilot-1
        step_phase_tot=mod(phase_pilot_filt(:,i+1)-phase_pilot_filt(:,i)+pi,2*pi)-pi;
    step_gain_tot=gain_pilot_filt(:,i+1)-gain_pilot_filt(:,i);
    
    delta_index=mod(pattern_pilot(i+1)-pattern_pilot(i),nr_carriers);
    
    step_phase=step_phase_tot./delta_index;
    step_gain=step_gain_tot./delta_index;
    
    %fill the low freq
    n_interp=[0:1:pattern_pilot(i+1)-pattern_pilot(i)];
    channel_gain(:,pattern_pilot(i):pattern_pilot(i+1))=gain_pilot_filt(:,i)*ones(1,length(n_interp))+(step_gain*ones(1,length(n_interp))).*(ones(nr_OFDM_symbols,1)*n_interp);
    channel_phase(:,pattern_pilot(i):pattern_pilot(i+1))=mod(phase_pilot_filt(:,i)*ones(1,length(n_interp))+(step_phase*ones(1,length(n_interp))).*(ones(nr_OFDM_symbols,1)*n_interp)+pi,2*pi)-pi;
end
% figure(1);
% subplot(2,1,1); stem(channel_gain(1,:)); 
% subplot(2,1,2); stem(channel_phase(1,:));
% figure(2);
% subplot(2,1,1); stem(channel_gain(2,:)); 
% subplot(2,1,2); stem(channel_phase(2,:));
channel=channel_gain.*exp(1i*channel_phase);

% for i=1:nr_OFDM_symbols
% channel_p(:,i) = spline(mod(pattern_pilot+64,128)-64,state_phase_pilot_filt(1,i,:),mod(pattern+64,128)-64);
% channel_g(:,i) = spline(mod(pattern_pilot+64,128)-64,gain_pilot_filt(i,:),mod(pattern+64,128)-64);
% end
% channel=channel_g.*exp(1i*channel_p);
channel=transpose(channel(:,pattern));

%% Removal of the frequency response from the estimate
data_received_equalized=data_received./channel;
end

