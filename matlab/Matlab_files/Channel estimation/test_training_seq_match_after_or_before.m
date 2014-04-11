clear all
close all
clc
nr_training_bits=100;
nr_data=1000;
Q=4;
sigma_noise=2;
training=randn(1,nr_training_bits);
data=qpsk(sign(randn(1,nr_data)));
seq=[training data'];
seq_up=upfirdn(seq,ones(Q,1),Q,1);
training_up=upfirdn(training,ones(Q,1),Q,1);
pre_seq=upfirdn(qpsk(sign(randn(1,nr_training_bits))),ones(1,Q),Q,1)';
tx=[pre_seq zeros(1,10) seq_up zeros(1,10)];
rx=tx+sqrt(sigma_noise)/sqrt(2)*(randn(1,length(tx))+1j*randn(1,length(tx)));
rx_filt=filter(ones(Q,1),1,rx);
training_filt=filter(ones(Q,1),1,training_up);
figure
a=abs(xcorr(training_filt,rx_filt));
plot(a(2400:2420)/max(max(a(1:2400)),max(a(2420:end))));
hold on
b=abs(xcorr(training_up,rx));
plot(b(2400:2420)/max(max(b(1:2400)),max(b(2420:end))),'red');