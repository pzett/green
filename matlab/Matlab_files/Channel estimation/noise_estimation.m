close all
clear all
clc
load('noise')
plotspectrum(X);
figure
subplot(2,1,1)
plot(real(X))
subplot(2,1,2)
plot(imag(X))
Y=abs(fftshift(fft(xcorr(X))));
% figure(1)
% %hold on
% semilogy((0:1/(2*length(X)):1/2-1/(2*length(X))),Y(length(X):end),'blue');