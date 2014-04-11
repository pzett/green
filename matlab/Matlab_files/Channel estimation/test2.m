clear all
close all
clc
Q=2;
C=0.5;
nu=-2*6/8/5;
%load('m-seq_short.mat')
[outS,period]=LFSR(14,[14 2 1],1);
outS=outS(1:2^13-1);
pulse_shape = ones(1, Q);
tx = upfirdn(outS, pulse_shape, Q, 1);
seq_bpsk_Qs = [2*tx-1];
%size(C*exp(1j*2*pi*nu*(0:length(seq_bpsk_Qs)-1)));
seq_bpsk_Qs_exp=seq_bpsk_Qs+C*exp(1j*2*pi*nu*(0:length(seq_bpsk_Qs)-1))';
%seq_bpsk_Qs_exp=C*exp(1j*2*pi*nu*(0:length(seq_bpsk_Qs)-1))';
%seq_bpsk_3s_3times=seq_bpsk_Qs
semilogy(abs(fft(xcorr(seq_bpsk_Qs_exp))/length(seq_bpsk_Qs_exp)))