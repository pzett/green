% close all;
% fid1=fopen('sent.dat','r');
% x=fread(fid1,82682*2,'short');
% fclose(fid1);
% 
% xComplex2= zeros(1,82682);
% count = 1;
% for i=1:2:82682*2
%     xComplex2(count) = complex(x(i),x(i+1));
%     count = count+1;
% end
% 
% xComplex2=conj(xComplex2)';


close all;
fid1=fopen('sent.dat','r');
x=fread(fid1,29909*2,'short');
fclose(fid1);

xComplex1= zeros(1,29909);
count = 1;
for i=1:2:29909*2
    xComplex1(count) = complex(x(i),x(i+1));
    count = count+1;
end

xComplex1=conj(xComplex1)';




% figure()
% plot(real(xComplex));

%figure()
%semilogy(abs(fft(xComplex)));
% close all;
% fid1=fopen('dataBinNPaq.dat','r');
% x=fread(fid1,10000,'double');
% fclose(fid1);