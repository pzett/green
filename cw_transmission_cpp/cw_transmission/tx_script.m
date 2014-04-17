%
%   Sript to test code
%


%create modulator:

freq=3e6;

%mod=cw(freq);

%mod=5000*[1, zeros(1, 99999)];

mod=5000*[1 j]*randn(2, 100000);

%figure(1);
%plot(mod,'o');

figure (2);
pwelch(mod);

drawnow;


Nsamples=length(mod);

%Nsamples=length(X) or Nsamples=length(X)+guard


RF_freq=5.5e9; %or 5.6e9


ref_clk=0;
gain=10;
tx_rate=25e6;
LOoffset=0;%10e6;
low_res=0;


tx( Nsamples,RF_freq, mod, ref_clk , gain, tx_rate, LOoffset, low_res);







