close all
clear all
clc
%load('tx_data_7.mat');
Q=4;
delta_f=1;
%train=training_10;
nr_bits_train_2=20;
train=randn(1,nr_bits_train_2)+1j*randn(1,nr_bits_train_2);
nr_train_bit=length(train);
nr_data=1000;
nr_guard=2;
nr_blocks=10000;
data=qpsk(randn(2*nr_data,1)>0);
data_pre=qpsk(randn(4*nr_train_bit,1)>0);
seq=[data_pre; zeros(nr_guard,1); conj(train'); data];
seq_up=upfirdn(seq, ones(Q,1), Q, 1);
train_up=upfirdn(train, ones(Q,1), Q, 1);
SNR=(2:1:4);
nu_offset=1;%10^(-4)*(-80:16:80);
err_t_sync=zeros(length(SNR),length(nu_offset));
for i=1:length(SNR)
  %  i
    for l=1:length(nu_offset)
     %   l
        freq_off=exp(-1j*2*pi*nu_offset(l)*(0:length(seq_up)-1))';
        for k=1:nr_blocks
            Y=seq_up.*freq_off+10^(-SNR(i)/10)*(randn(length(seq_up),2)*[1; j]);
            %Y_filtered=filter(ones(Q,1),1,Y);
            E_abs=xcorr(abs(Y),abs(train_up));
            %E_abs=xcorr(Y,train_up);
            [amp t_sync]=max(abs(E_abs(floor(length(E_abs)*1/2):floor(length(E_abs)*3/4))));
%             phi_hat=angle(E(floor(length(E)*1/2)+t_sync));
%             Received=Y_filtered(t_sync+1+Q*nr_train_bit:Q:t_sync+1+nr_data*Q+4*nr_train_bit-1)*exp(-1j*phi_hat);
%             y=0;
%             Q0=10;
%             phi_hat=zeros(length(Received)+1,1);
%             phi_hat(1)=0;
%             for i=1:length(Received)
%                 [yhat,phi_hat(i+1),x,P,Q0]=kalman((mod(angle(Received(i))-phi_hat(i),pi/2))+phi_hat(i)-pi/4,[1],[1],[1],0.1,20,phi_hat(i),Q0);
%                 Received(i)=Received(i)*exp(-j*phi_hat(i+1));
%             end
%             
%             figure
%             plot(Received,'.r')
%             data_est=detect(Received);
%             figure
%             plot(angle(Received)-phi_hat(1:end-1)'+phi_hat(1:end-1)'-pi/4,'.');
%             figure
%             load('tx_data_6')
%             plot(data_est-detect(data))
            if t_sync~=Q*(2*nr_train_bit+nr_guard)+Q/2
                err_t_sync(i,l)=err_t_sync(i,l)+1;
            end         
        end
    end
end
err_t_sync(2)







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