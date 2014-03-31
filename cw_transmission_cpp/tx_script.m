%
%   Sript to test code
%

clear all
clc

%create modulator:

freq1=10e6;

mod1=cw(freq1);

mod1(1:1:10)/5000

lkpl

freq2=2.5e6;

mod2=cw(freq2);

mod=mod1+mod2;

%mod=5000*[1, zeros(1, 99999)];

%mod=[1 j]*randn(2, 100000);


%QPSK constellation




n_bits=100000;

Q=8;

pulse_shape = ones(1, Q);

%Random bits
%b = (rand(1, n_bits) > .5);


% Map bits into complex-valued QPSK symbols. -> Es=1
%s = qpsk(b);

s=mod;

plot(s, 'o');


% Upsample the signal, apply pulse shaping.
tx_seq =s;% upfirdn(s, pulse_shape, Q, 1);


figure (2);
pwelch(tx_seq);

drawnow;


Nsamples=length(s);

%Nsamples=length(X) or Nsamples=length(X)+guard


RF_freq=5.5e9; %or 5.6e9


ref_clk=0;
gain=10;
tx_rate=25e6;
LOoffset=10e6;
low_res=0;


tx( Nsamples,RF_freq, tx_seq, ref_clk , gain, tx_rate, LOoffset, low_res);







