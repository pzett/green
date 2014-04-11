%Test of the channel estimator

%% AWGN channel
[outS,period]=LFSR(9,[9,5],1);
w=whiteNoise(0,1,period-1);
y=outS+w;

figure;
subplot(2,1,1), plot(xcorr(outS,y)), title('Noisy correlation')
subplot(2,1,2), plot(xcorr(outS)), title('Noisless correlation')

%% Multipath channel with 1-tap filter
h=[1 zeros(1,10) 0.8];
[outS,period]=LFSR(9,[9,5],1);
ytmp=filter(h,1,outS);
w=whiteNoise(0,1,length(ytmp));
y=ytmp;

figure;
subplot(2,1,1), plot(xcorr(outS,y)), title('Noisy correlation')
subplot(2,1,2), plot(xcorr(outS)), title('Noisless correlation')