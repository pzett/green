%close all;
fid1=fopen('sent.dat','r');
x=fread(fid1,55680,'short');
fclose(fid1);

xComplex= zeros(1,55680/2);
count = 1;
for i=1:2:55680
    xComplex(count) = complex(x(i),x(i+1));
    count = count+1;
end

figure()
plot(real(xComplex));

%figure()
%semilogy(abs(fft(xComplex)));