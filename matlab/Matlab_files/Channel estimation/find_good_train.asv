close all
clear all
clc
nr_training_bits=10;
Q=4;
train_best=0;
maximum=1;
for i=1:1000000
a=(randn(nr_training_bits,1)+j*randn(nr_training_bits,1));
b=upfirdn(a,ones(Q,1),Q,1);
c=abs(xcorr(abs(b),abs(b)-sum(abs(b))/length(b)));
d=c/max(c);
d(Q*nr_training_bits-Q/2:Q*nr_training_bits+Q/2)=0;
if max(d)<maximum
   train_best=a;
   maximum=max(d)
   i
end
end