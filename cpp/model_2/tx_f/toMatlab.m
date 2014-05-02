close all;
fid1=fopen('dataBinNPaq.dat','r');
x=fread(fid1,82714*2,'short');
fclose(fid1);

xComplex1= zeros(1,82714);
count = 1;
for i=1:2:82714*2
    xComplex1(count) = complex(x(i),x(i+1));
    count = count+1;
end

% figure()
% plot(real(xComplex));

%figure()
%semilogy(abs(fft(xComplex)));