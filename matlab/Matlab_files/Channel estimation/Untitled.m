%clear all
close all
clc
%train=(2*LFSR(4,[4 1 0],1)-1);
sync_err=0;
for i=1:1000
    i
seq=sign(randn(1,10000));
nr_data=length(seq)+length(train);
delay=floor((nr_data)*rand(1)+1);
r=circshift([train seq train seq],[0 delay]);
autocorr=xcorr(r,train);
plot(autocorr);
[a b]=max(abs(autocorr(2*nr_data:3*nr_data)));
b=b-length(r)+2*nr_data-1;
if b~=delay
    sync_err=sync_err+1
end
end