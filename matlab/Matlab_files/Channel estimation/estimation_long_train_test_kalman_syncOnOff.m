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
plot(A);
plot(A(floor(nr_data*0.55):floor(nr_data*0.65)))
train_up=upfirdn(train, ones(Q,1), Q, 1);
sigma_n=1;
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
%Y=Y.*exp(-1j*2*pi*0.005*(0:length(X)-1));
plot(Y,'.')
Y_filtered=filter(ones(Q,1),1,Y);
plot(Y_filtered,'.')
%plot(abs(xcorr(Y,seq)))
E_abs=xcorr(abs(Y_filtered),abs(train_up));
E=xcorr(Y_filtered,train_up);
plot(abs(E_abs))
[amp t_sync]=max(abs(E_abs(floor(length(E)*1/2):floor(length(E)*3/4))));
phi_hat=angle(E(floor(length(E)*1/2)+t_sync));
C=abs(E(floor(length(E)*1/2)+t_sync-1))/abs(sum(train_up*train_up'));
% E_abs(floor(length(E)*1/2)+t_sync-1)=0;
% [amp t_sync2]=max(abs(E_abs(floor(length(E)*1/2):floor(length(E)*3/4))));
% A1=E(floor(length(E)*1/2)+min(t_sync,t_sync2));
% A2=E(floor(length(E)*1/2)+max(t_sync,t_sync2));
% 
% %estimate the channel
% 
% C=(A2-A1)/(2-(1/Q));
% epsilon1=1-A1/(A2-A1)*(1-(1/Q));
% epsilon2=1/Q-epsilon1;
% U=[epsilon1 1-epsilon1 0;0 1-epsilon2 epsilon2];
% 
% 
% C_w=sigma_n/C*[Q Q-1;Q-1 Q];
% %C_w=toeplitz(w_k);
% %C_w=(1/EbN0)*C_w;
% e=[0; 1; 0];
% R=U*U';
% R=R+C_w;
% C_mmse=R\U*e;
% %C_mmse=conj(C_mmse(length(C_mmse):-1:1));
% 
% phi_hat1=angle(A1);
% phi_hat2=angle(A2);

% figure
% P=4;
% hold on
% plot(real(E(floor(length(E)*1/2)+t_sync-10+1:floor(length(E)*1/2)+t_sync+10+1)*exp(-1j*phi_hat)-(E(floor(length(E)*1/2)+t_sync-(10+P)+1:floor(length(E)*1/2)+t_sync+10-P+1)*exp(-1j*phi_hat))-(E(floor(length(E)*1/2)+t_sync-(10-P)+1:floor(length(E)*1/2)+t_sync+(10+P)+1)*exp(-1j*phi_hat))))
% plot(real(E(floor(length(E)*1/2)+t_sync-10+1:floor(length(E)*1/2)+t_sync+10+1)*exp(-1j*phi_hat)-(E(floor(length(E)*1/2)+t_sync-(10+P)+1:floor(length(E)*1/2)+t_sync+10-P+1)*exp(-1j*phi_hat))+(E(floor(length(E)*1/2)+t_sync-(10-P)+1:floor(length(E)*1/2)+t_sync+(10+P)+1)*exp(-1j*phi_hat))),'green')
% plot(real(E(floor(length(E)*1/2)+t_sync-10+1:floor(length(E)*1/2)+t_sync+10+1)*exp(-1j*phi_hat)+(E(floor(length(E)*1/2)+t_sync-(10+P)+1:floor(length(E)*1/2)+t_sync+10-P+1)*exp(-1j*phi_hat))+(E(floor(length(E)*1/2)+t_sync-(10-P)+1:floor(length(E)*1/2)+t_sync+(10+P)+1)*exp(-1j*phi_hat))),'black')
% plot(real(E(floor(length(E)*1/2)+t_sync-10+1:floor(length(E)*1/2)+t_sync+10+1)*exp(-1j*phi_hat)+(E(floor(length(E)*1/2)+t_sync-(10+P)+1:floor(length(E)*1/2)+t_sync+10-P+1)*exp(-1j*phi_hat))-(E(floor(length(E)*1/2)+t_sync-(10-P)+1:floor(length(E)*1/2)+t_sync+(10+P)+1)*exp(-1j*phi_hat))),'red')
% hold off

% Received=zeros(2,25000);
% Received(1,:)=Y_filtered(min(t_sync,t_sync2)+400:Q:min(t_sync,t_sync2)+25000*Q+400-1); %*exp(-1j*phi_hat1);
% Received(2,:)=Y_filtered(max(t_sync,t_sync2)+400:Q:max(t_sync,t_sync2)+25000*Q+400-1); %*exp(-1j*phi_hat2);

received=Y_filtered(t_sync+400:Q:t_sync+25000*Q+400-1)*exp(-1j*phi_hat);
%received=C_mmse(1)*Received(1,:)+C_mmse(2)*Received(2,:);
%received=Received(2,:)*exp(-1j*phi_hat2);

plot(received,'.')
y=0;
Q0=10;
phi_hat=zeros(length(received)+1,1);
phi_hat(1)=0;
for i=1:length(received)
  %  angle(Received(i))-phi_hat(i)
  %  mod(angle(Received(i))-phi_hat(i)+pi/2,pi)-pi/2
    [yhat,phi_hat(i+1),x,P,Q0]=kalman((mod(angle(received(i))-phi_hat(i),pi/2))+phi_hat(i)-pi/4,[1],[1],[1],0.1,20,phi_hat(i),Q0);
    received(i)=received(i)*exp(-1j*phi_hat(i+1));
end

figure
plot(received,'.r')
data_est=detect(received);
figure
plot(angle(received)-phi_hat(1:end-1)'+phi_hat(1:end-1)'-pi/4,'.');
figure
load('tx_data_6')
plot(data_est-detect(data))
figure
plot(C*qpsk(data_est)-conj(received'),'.')
cov(C*qpsk(data_est)-conj(received'))

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