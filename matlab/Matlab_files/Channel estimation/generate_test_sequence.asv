clear all
close all
clc
nr_bits_train_1=10;
nr_bits_train_2=20;
nr_bits_train_3=30;
nr_bits_train_4=40;
nr_bits_train_5=50;
nr_bits_train_6=60;
nr_bits_train_7=70;
%nr_bits_train_8=80;
%nr_bits_train_9=90;
nr_bits_train_10=100;
nr_data=1000;
Q=4;
sigma_noise=1;
training_10=randn(1,nr_bits_train_10)+1j*randn(1,nr_bits_train_10);
data=qpsk(randn(1,nr_data)>0);
seq=[training_10 data'];
seq_up=upfirdn(seq,ones(Q,1),Q,1);
training_3_up=upfirdn(training_10,ones(Q,1),Q,1);
pre_seq=upfirdn(qpsk(sign(randn(1,nr_bits_train_1))),ones(1,Q),Q,1)';
tx=[pre_seq seq_up];
rx=tx+sqrt(sigma_noise)/sqrt(2)*(randn(1,length(tx))+1j*randn(1,length(tx)));
