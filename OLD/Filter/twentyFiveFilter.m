clc;clear;close all;
fid1=fopen('x.bin','r');
x=fread(fid1,1000,'double');
fclose(fid1);

xComplex= zeros(1,500);
count = 1;
for i=1:2:1000
    xComplex(count) = complex(x(i),x(i+1));
    count = count+1;
end


fid2=fopen('xShift.bin','r');
xShift=fread(fid2,1000,'double');
fclose(fid2);

xShiftComplex= zeros(1,500);
count = 1;
for i=1:2:1000
    xShiftComplex(count) = complex(xShift(i),xShift(i+1));
    count = count+1;
end

fid3=fopen('xFilt.bin','r');
xFilt=fread(fid3,1000,'double');
fclose(fid3);

xFiltComplex= zeros(1,500);
count = 1;
for i=1:2:1000
    xFiltComplex(count) = complex(xFilt(i),xFilt(i+1));
    count = count+1;
end

figure();

semilogy((0:(1/500):(1-(1/500))),abs(fft(xComplex)));
hold all;
semilogy((0:(1/500):(1-(1/500))),abs(fft(xShiftComplex)));
semilogy((0:(1/500):(1-(1/500))),abs(fft(xFiltComplex)));
grid on;
