close all
clear all
clc
load('rx_data_6');
%load('tx_data_4');
Q=4;
delta_f=1;
% plot(X,'.')
nr_data=length(X);
A=abs(fft(X));
% plot(A);
% plot(A(floor(nr_data*0.55):floor(nr_data*0.65)))
Length_train=100;
train_up=upfirdn(train, ones(Q,1), Q, 1);

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


figure
Y=X.*exp(-1j*2*pi*nu_offset*(0:length(X)-1));
Y=Y.*exp(-1j*2*pi*-0.0100*(0:length(X)-1));
plot(Y,'.')
Y_filtered=filter(ones(Q,1),1,Y);
plot(Y_filtered,'.')
%plot(abs(xcorr(Y,seq)))
E_abs=xcorr(abs(Y_filtered),abs(train_up));
E=xcorr(Y_filtered,train_up);
plot(abs(E_abs))
[amp t_sync]=max(abs(E_abs(floor(length(E_abs)*1/2):floor(length(E_abs)*3/4))));
phi_hat=angle(E(floor(length(E)*1/2)+t_sync));

figure
P=4;
hold on
plot(real(E(floor(length(E)*1/2)+t_sync-10+1:floor(length(E)*1/2)+t_sync+10+1)*exp(-1j*phi_hat)-(E(floor(length(E)*1/2)+t_sync-(10+P)+1:floor(length(E)*1/2)+t_sync+10-P+1)*exp(-1j*phi_hat))-(E(floor(length(E)*1/2)+t_sync-(10-P)+1:floor(length(E)*1/2)+t_sync+(10+P)+1)*exp(-1j*phi_hat))))
plot(real(E(floor(length(E)*1/2)+t_sync-10+1:floor(length(E)*1/2)+t_sync+10+1)*exp(-1j*phi_hat)-(E(floor(length(E)*1/2)+t_sync-(10+P)+1:floor(length(E)*1/2)+t_sync+10-P+1)*exp(-1j*phi_hat))+(E(floor(length(E)*1/2)+t_sync-(10-P)+1:floor(length(E)*1/2)+t_sync+(10+P)+1)*exp(-1j*phi_hat))),'green')
plot(real(E(floor(length(E)*1/2)+t_sync-10+1:floor(length(E)*1/2)+t_sync+10+1)*exp(-1j*phi_hat)+(E(floor(length(E)*1/2)+t_sync-(10+P)+1:floor(length(E)*1/2)+t_sync+10-P+1)*exp(-1j*phi_hat))+(E(floor(length(E)*1/2)+t_sync-(10-P)+1:floor(length(E)*1/2)+t_sync+(10+P)+1)*exp(-1j*phi_hat))),'black')
plot(real(E(floor(length(E)*1/2)+t_sync-10+1:floor(length(E)*1/2)+t_sync+10+1)*exp(-1j*phi_hat)+(E(floor(length(E)*1/2)+t_sync-(10+P)+1:floor(length(E)*1/2)+t_sync+10-P+1)*exp(-1j*phi_hat))-(E(floor(length(E)*1/2)+t_sync-(10-P)+1:floor(length(E)*1/2)+t_sync+(10+P)+1)*exp(-1j*phi_hat))),'red')
hold off

Received_train=Y_filtered(t_sync+1:Q:t_sync+Q+Q*Length_train-5)*exp(-1j*phi_hat);
Received=Y_filtered(t_sync+1+Q*Length_train:Q:t_sync+1+25000*Q+Q*Length_train-1)*exp(-1j*phi_hat);

plot(Received,'.')
y=0;
Q0=[pi^2 0;0 0.82];
phi_hat=0;
freq=0;
x=[phi_hat;freq];
phase=mod(angle(Received_train(1)*conj(train(1)))+pi+x(1),2*pi)-pi;
for i=2:length(Received_train)
   % angle(Received_train(i)*conj(train(i)))-x(1)
   % mod(angle(Received_train(i)*conj(train(i)))+pi-x(1),2*pi)-pi
   % x(2)*2*pi
   [~,x,~,P,Q0]=kalman(mod(angle(Received_train(i)*conj(train(i)))+pi-x(1),2*pi)-pi,[0 2*pi;0 1],1,[1 0],[0.1 0; 0 0],(pi/8)^2,x,Q0);
Received_train(i)*conj(train(i))
   x(1)
x(2)
x(1)=x(1)+phase;
phase=x(1);
   %Received(i)=Received(i)*exp(-j*x(1));
%x(1)=mod(x(1)+pi,2*pi)-pi;
end
x(2)
phi_hat2=phi_hat;
phi_hat=zeros(length(Received)+1,1);
phi_hat(1)=phi_hat2;
for i=1:length(Received)
  %  angle(Received(i))-phi_hat(i)
  %  mod(angle(Received(i))-phi_hat(i)+pi/2,pi)-pi/2
   [~,x,~,P,Q0]=kalman((mod(angle(Received(i))-phase,pi/2))-pi/4,[0 2*pi;0 1],1,[1 0],[0.1 0; 0 0],(pi/2)^2,x,Q0);

phase=x(1)+phase;
phi_hat(i+1)=phase;
x(1)=0;
%x(1)=mod(x(1)+pi,2*pi)-pi;
   Received(i)=Received(i)*exp(-j*phi_hat(i+1));
end


plot(Received,'.r')
data_est=detect(Received);
figure
plot(angle(Received)-phi_hat(1:end-1)'+phi_hat(1:end-1)'-pi/4,'.');
figure
load('tx_data_6')
plot(data_est-detect(data))

% [a delay_r1]=max(E(1:length(E)/3));
% plot(abs(xcorr([seq_bpsk_Qs_exp' seq_bpsk_Qs_exp'],Y)))
% delay_r = (length(abs(xcorr([seq_bpsk_Qs_exp' seq_bpsk_Qs_exp'],Y)))+3)/2-delay_r1;
% Z=Y(delay_r:delay_r+length(seq_bpsk_Qs_exp)-1);
% cross_corr=xcorr([seq_bpsk_Qs_exp'],Z);
% auto_corr=xcorr([seq_bpsk_Qs_exp']);
% [A1 B1]=max(abs(cross_corr));
% [A2 B2]=max(abs(auto_corr));
% cross_corr(B1)
% Z_filtered=filter(ones(Q,1),[1],Z);
% seq_bpsk_Qs_exp_filtered=filter(ones(Q,1),[1],seq_bpsk_Qs_exp);
% cross_corr=xcorr(seq_bpsk_Qs_exp_filtered,Z_filtered);
% auto_corr=xcorr(seq_bpsk_Qs_exp_filtered);
% auto_corr2=xcorr(Z_filtered);
% figure
% tau=0;
% semilogy((-length(cross_corr)/2+1/2+tau:length(cross_corr)/2+tau-1/2),abs(cross_corr)/max(abs(cross_corr)),'red')
% hold on
% semilogy((-length(auto_corr)/2+1/2:length(auto_corr)/2-1/2),abs(auto_corr)/max(abs(auto_corr)))
% semilogy((-length(auto_corr2)/2+1/2:length(auto_corr2)/2-1/2),abs(auto_corr2)/max(abs(auto_corr2)),'green')
% grid on
% figure
% subplot(2,1,1)
% semilogy(abs(fftshift(fft(cross_corr))))
% subplot(2,1,2)
% semilogy(abs(fftshift(fft(auto_corr))))