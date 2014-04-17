%rx_test.m


%initial values

n_bits=1000000;%3145725;

Nsamples= 2*n_bits; %Two times longer than transmit signal
RFfreq=5.5e9;
ref_clk=0;
gain=10;
rx_rate=25e6;
LOoffset=10e6;




Q=8;

pulse_shape = ones(1, Q);
% Matched filter impulse response. 
mf_pulse_shape = fliplr(pulse_shape);


%received sequence
rx_seq=rx(Nsamples,RFfreq,ref_clk,gain,rx_rate,LOoffset);

rx_seq=rx_seq;

X=rx_seq;%conv(mf_pulse_shape,rx_seq);


t=(-10:10)+1e-5;
 
hlp=sin(2*pi*0.3*t)./t;

A=fft(hlp);
plot((1:21)/21,abs(A)/25);
 
X=conv(X,hlp);

figure(1)

plot(X,'o');
drawnow;
 
%  env=fftshift(fft(X));
%  figure(2); plot(linspace(0,1,length(env)),abs(env))

figure(2)

pwelch(X);

P = powerEst(X);
figure(3)
plot(P);

