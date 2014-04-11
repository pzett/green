function [Y] = Upconvert(Signal, Fs)
% Upconverter - insert a signal and the signal sampling frequency, Fs, 
% and have it frequency shifted. CAUTION! Not even nearly done yet!!!

Signal = Signal / max(abs(Signal));    % scale the signal

% plot(abs(fft(Signal)).^2);
N = 2;
B = fir1(10,0.94/N);

inter = upsample(Signal,N);
inter = filter(B,1,inter);

Spectrum = fft(inter);
posSpec = sqrt(2).*Spectrum(1:length(Spectrum)/2);  % positive side of spectrum

cmplSignal = ifft(posSpec); %this is the pre envelope

t = 0:1/Fs:(length(cmplSignal)-1)/Fs;
fc = 5000;

cmplBB = cmplSignal.*exp(-1i*2*pi*fc.*t);
Y = real(sqrt(2)*cmplBB.*exp(1i*2*pi*fc*t));

Y = Y / max(abs(Y));    % scale signal

Signal = Signal
% I = real(cmplSignal);
% Q = imag(cmplSignal);
% 
% I = upsample(I,N); I = filter(B,1,I);
% Q = upsample(Q,N); Q = filter(B,1,Q);
% 
% 
% fc = 5000;
% Fs2 = Fs*N;
% t = 0:1/Fs2:(length(Q)-1)/Fs2;
% sinfc = -1i.*sqrt(2).*sin(2*pi*fc*t);
% cosfc = sqrt(2).*cos(2*pi*fc*t);
% 
% sI = cosfc.*I;
% sQ = sinfc.*Q;
hold on
plot(Y(1:64));plot(Signal(1:64),'r');


end