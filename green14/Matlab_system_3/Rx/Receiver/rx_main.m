% main receiver function

%% Initialisation
clear
close all
clc

test=0;
n_sim=31;
fileTX=['tx_data_', num2str(n_sim),'.mat'];
fileRX=['rx_data_', num2str(n_sim),'.mat'];
load(fileTX)
load(fileRX)


%% Synchronisation
[t_samp]=sync(rx_data,train_seq,Q,test);

t_start=t_samp+Q*nr_training_bits;
t_end=t_start+(nr_car+length_cpre+length_suf)*nr_OFDM_symbols-1;

received_data=rx_data(t_start:t_end);% received data whithout TS
rx_data_reshaped=reshape(received_data,nr_car+length_cpre+length_suf,nr_OFDM_symbols);    % received data, []

%% Joint estimation of the channel and frequency offset

sigma_sqr=sigma_sqr_awgn;

%---------------------------LLH Estimation PHN-------------------------
phi_rms=9;
W=100000;
u=abs(-nr_car+1:nr_car-1)+1;
phi_row=(pi*phi_rms/180)^2*exp(-2*pi*W*u/25e6);
phi_cov=zeros(nr_car);
for i2=1:nr_car
    phi_cov(i2,:)=phi_row(nr_car:end);
    phi_row=circshift(phi_row,[0 1]);
end  
%---------------------------------------------------------------------
%L=length(channel_filter);
L=3;
rho=c/(2);

[delta_nu,phi_hat,channel_est]=LLH_estReal(rx_data_reshaped(length_cpre+1:length_cpre+nr_car,1),(train_OFDM),nr_car, phi_cov,sigma_sqr,L,rho);


channel_freq=fft(channel_est,nr_car);

nu_corr=[1:(nr_OFDM_symbols-1)*(nr_car+length_cpre+length_suf)];

nu_corr=reshape(nu_corr,nr_car+length_cpre+length_suf,nr_OFDM_symbols-1);
data_OFDM=rx_data_reshaped(:,2:end).*exp(-1i*2*pi*nu_corr*delta_nu);


%% FFT + CP removal
data_FFT=fft(rx_data_reshaped(length_cpre+1:end-length_suf,2:end));
% apply channel
data_chan=data_FFT./((channel_freq)*ones(1,size(data_FFT,2)));
% Normalise pilots
pilot_corr=ones(nr_car,nr_OFDM_symbols-1);
pilot_corr(pilot_pattern,:)=pilot;

data_delta=data_chan./pilot_corr;


%% Tracking of CFO and SCO
% [dataTrack]=tracking(data_delta, pilot_pattern, nr_car,length_cpre);

phase_pilot_0=0;
Q0=[10 0;0 1];
for i3=1:length(pilot_pattern)
        [~,state_phase_pilot_filt(:,:,i3),~,~,Q01] = kalman_phase(angle(data_delta(pilot_pattern(i3),:)).',[1 2*pi;0 1],[1 0;0 1],[1 0],[0.001 0;0 0],0.1,phase_pilot_0,0,Q0,0);
        phase_pilot_filt(i3,:)=state_phase_pilot_filt(1,:,i3).';
end
% dataCorr=data_delta.*exp(-1i*ones(nr_car,1)*((mod(phase_pilot_filt(1,:)-phase_pilot_filt(2,:)-pi,2*pi)-pi)+2*phase_pilot_filt(2,:))/2);
dataCorr=data_delta.*exp(-1i*ones(nr_car,1)*(atan2(mean([sin(phase_pilot_filt(1,:));sin(phase_pilot_filt(2,:))],1),mean([cos(phase_pilot_filt(1,:));cos(phase_pilot_filt(2,:))],1))));


%% Hard decision
 data_line=reshape(circshift(dataCorr(car_pattern, 1:end),[nr_used_car/2 0]),1,(nr_OFDM_symbols-1)*nr_used_car);
 data_dft_inv=DFT_precode(data_line.', nr_car_used, 'decode' );
 figure(50); plot(data_dft_inv(1:end-100),'.')


if strcmp(modulation_type,'qpsk')
    dataQPSK=reshape(data_dft_inv,nr_used_car,(nr_OFDM_symbols-1));
    data_send=reshape(data_mapped,nr_used_car,(nr_OFDM_symbols-1));
    u_hat=floor(sign(dataQPSK));
    u_hat_real=abs(real(u_hat));
    u_hat_imag=abs(imag(u_hat));
    u=floor(sign(data_send(:,1:end)));
    u_real=abs(real(u));
    u_imag=abs(imag(u));
    BER=sum(sum(abs(u_real-u_hat_real)+abs(u_imag-u_hat_imag)))/2/size(u_real,1)/size(u_real,2)
else
    bhat=detect16QAM(data_dft_inv);
    BER=sum(abs(bhat(1:nr_data_bits)-data(1:nr_data_bits)))/nr_data_bits
end

