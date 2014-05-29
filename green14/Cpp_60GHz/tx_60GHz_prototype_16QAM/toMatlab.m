% Script used to import data from cpp to matlab

N_samps=37028;


close all;
fid1=fopen('sent.dat','r');
x=fread(fid1,2*N_samps,'short');
fclose(fid1);

xComplex1= zeros(1,N_samps);
count = 1;
for i=1:2:2*N_samps
    xComplex1(count) = complex(x(i),x(i+1));
    count = count+1;
end

xComplex1=conj(xComplex1)';


%disp(['rms= ' num2str(rms(xComplex1))]);

%figure()
%plot(real(xComplex1));
% figure()
% plot(imag(xComplex1));

%figure()
%pwelch(xComplex1);
%semilogy(abs(fft(xComplex1)));
figure()
subplot(2,1,1)
plot(real(xComplex1)); 
title('Transmitted Data');
xlabel('n');
ylabel('Amplitude');
subplot(2,1,2)
semilogy(linspace(0,1,length(xComplex1)),abs(fft(xComplex1)));
title('Spectrum of Transmitted Data');
xlabel('f (normalized frequency');
ylabel('Gain (dB)');

pause();

%plot(abs(fft(xComplex1)));
% close all;
% fid1=fopen('dataBinNPaq.dat','r');
% x=fread(fid1,10000,'double');
% fclose(fid1);
