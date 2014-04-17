%rx_test.m


%initial values

Nsamples= 2*10000; %Two times longer than transmit signal
RFfreq=5.5e9;
ref_clk=0;
gain=30;
rx_rate=25e6;
LOoffset=0;%10e6;


%X=rx(Nsamples,RFfreq,ref_clk,gain,rx_rate,LOoffset);


t=(-10:10)+1e-5;
 
hlp=sin(2*pi*0.3*t)./t;

A=fft(hlp);
plot((1:21)/21,abs(A)/25);
 
% X=conv(X,hlp);

figure(1)

plot(X,'o');
drawnow;
 
%  env=fftshift(fft(X));
%  figure(2); plot(linspace(0,1,length(env)),abs(env))

figure(2)

pwelch(X);
 

