%close all;
%clear all;

fid1=fopen('x_freq.dat','r');
x_freq=fread(fid1, 55002,'double');
fclose(fid1);


xComplex_freq= zeros(1,55002/2);
count = 1;
for i=1:2:55002
    xComplex_freq(count) = complex(x_freq(i),x_freq(i+1));
    count = count+1;
end

length(xComplex_freq);

figure(2)
subplot(3,2,1)
plot(real(xComplex_freq));

fid1=fopen('x_matched.dat','r');
x_matched=fread(fid1, 55002,'double');
fclose(fid1);


xComplex_matched= zeros(1,55002/2);
count = 1;
for i=1:2:55002
    xComplex_matched(count) = complex(x_matched(i),x_matched(i+1));
    count = count+1;
end

length(xComplex_matched);

subplot(3,2,3)
plot(real(xComplex_matched));


fid1=fopen('x_downsamp.dat','r');
x_downsamp=fread(fid1, 2*3225,'double');
fclose(fid1);


xComplex_downsamp= zeros(1,3225);
count = 1;
for i=1:2:3225*2
    xComplex_downsamp(count) = complex(x_downsamp(i),x_downsamp(i+1));
    count = count+1;
end

length(xComplex_downsamp);

subplot(3,2,2)
plot(xComplex_downsamp,'.');


fid1=fopen('xcorr.dat','r');
xcorr=fread(fid1, 55002,'double');
fclose(fid1);


xComplex_corr= zeros(1,55002/2);
count = 1;
for i=1:2:55002
    xComplex_corr(count) = complex(xcorr(i),xcorr(i+1));
    count = count+1;
end

length(xComplex_corr);

figure(2)
subplot(3,2,5)
plot(abs(xComplex_corr));



fid1=fopen('x_filt.dat','r');
x_filt=fread(fid1, 2*3125,'double');
fclose(fid1);


xComplex_filt= zeros(1,3125);
count = 1;
for i=1:2:3125*2
    xComplex_filt(count) = complex(x_filt(i),x_filt(i+1));
    count = count+1;
end

length(xComplex_filt);

subplot(3,2,4)
plot(xComplex_filt,'.');




