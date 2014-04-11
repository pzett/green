% test frequency coherence training sequence
clear all
close all
clc
N_bits_train=30;
Q=4;
sigma_noise=1;
sigma_train=1;
nu_range=10^-4*(-100:100);
train=(randn(1,N_bits_train)+1j*randn(1,N_bits_train))/sqrt(2);
noise=(randn(1,Q*N_bits_train)+1j*randn(1,Q*N_bits_train))/sqrt(2);
%[outS,period]=LFSR(5,[5 3],1);
%train=qpsk(train_0);
%train_2=2*train-1;
freq_coherence=zeros(21,length(nu_range));
train_2=upfirdn(train,ones(Q,1),Q,1);
for i=1:length(nu_range)
train_freq_off=train_2.*exp(2j*pi*nu_range(i)*(0:Q*(N_bits_train)-1))+noise;
corr=xcorr(train_freq_off,train_2);
freq_coherence(:,i)=abs(corr(Q*N_bits_train-10:Q*N_bits_train+10));
[a m]=max(abs(freq_coherence(:,i)));
if m==11 %Q*N_bits_train
    'no err'
    nu_range(i)
end
end
plot(nu_range,freq_coherence);
