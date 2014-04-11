close all
clear all
clc
load('tx_data_4');
%load('m-seq_short.mat')
load('rx_data_4');
load('filter');
Q=3;
%L_est=15000;
delta_f=1;
%seq_bpsk_Qs_exp=seq_bpsk_3s_3times;
seq_bpsk_Qs_exp=seq';
%X=X;
nr_data=length(X);
A=abs(fft(X));
plot(A)
plot(A(floor(nr_data*0.55):floor(nr_data*0.65)))


%Catching frequency of the receiver LO (around -10MHz (approx nu=0.6)) (the
%biggest amplitude should be (not always :'( ) the one of the receiver LO because no channel
%attenuation.
[a fLO_recever]=max(A(floor(nr_data*0.55):floor(nr_data*0.65)));
plot(A)
hold on
A(floor(nr_data*0.55)+fLO_recever-1-delta_f:floor(nr_data*0.55)+fLO_recever-1+delta_f)=0;
plot(A,'red')
hold off
%Catching frequency of the transmiter LO (around -10MHz (approx nu=0.6))
[a fLO_transmiter]=max(A(floor(nr_data*0.55):floor(nr_data*0.65)));
%calculation of the difference to get nu_offset (division because we were
%calculation the freq in terme of sample and not in term of nu.
nu_offset=-(fLO_recever-fLO_transmiter)/nr_data;

Y=X.*exp(-1j*2*pi*nu_offset*(0:length(X)-1));
plot(abs(xcorr([seq_bpsk_Qs_exp' seq_bpsk_Qs_exp'],Y)))
E=abs(xcorr([seq_bpsk_Qs_exp' seq_bpsk_Qs_exp'],Y));
[a delay_r1]=max(E(1:length(E)/3));
plot(abs(xcorr([seq_bpsk_Qs_exp' seq_bpsk_Qs_exp'],Y)))
delay_r = (length(abs(xcorr([seq_bpsk_Qs_exp' seq_bpsk_Qs_exp'],Y)))+3)/2-delay_r1;
Z=Y(delay_r:delay_r+length(seq_bpsk_Qs_exp)-1);
cross_corr=xcorr([seq_bpsk_Qs_exp'],Z);
auto_corr=xcorr([seq_bpsk_Qs_exp']);
[A1 B1]=max(abs(cross_corr));
[A2 B2]=max(abs(auto_corr));
cross_corr(B1)
Z_filtered=filter(ones(Q,1),[1],Z);
seq_bpsk_Qs_exp_filtered=filter(ones(Q,1),[1],seq_bpsk_Qs_exp);
cross_corr=xcorr(seq_bpsk_Qs_exp_filtered,Z_filtered);
auto_corr=xcorr(seq_bpsk_Qs_exp_filtered);
auto_corr2=xcorr(Z_filtered);
figure
tau=0;
semilogy((-length(cross_corr)/2+1/2+tau:length(cross_corr)/2+tau-1/2),abs(cross_corr)/max(abs(cross_corr)),'red')
hold on
semilogy((-length(auto_corr)/2+1/2:length(auto_corr)/2-1/2),abs(auto_corr)/max(abs(auto_corr)))
% semilogy((-length(auto_corr2)/2+1/2:length(auto_corr2)/2-1/2),abs(auto_corr2)/max(abs(auto_corr2)),'green')
grid on
figure
subplot(2,1,1)
semilogy(abs(fftshift(fft(cross_corr))))
subplot(2,1,2)
semilogy(abs(fftshift(fft(auto_corr))))