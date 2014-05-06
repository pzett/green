close all;
fid1=fopen('sent.dat','r');
x=fread(fid1,8000,'short');
fclose(fid1);

xComplex1= zeros(1,4000);
count = 1;
for i=1:2:4000*2
    xComplex1(count) = complex(x(i),x(i+1));
    count = count+1;
end

% figure()
% plot(real(xComplex));

%figure()
%semilogy(abs(fft(xComplex)));
% close all;
% fid1=fopen('dataBinNPaq.dat','r');
% x=fread(fid1,10000,'double');
% fclose(fid1);