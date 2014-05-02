close all;
clear all;

%% Data Received

N_samps=72920;
fid1=fopen('received.dat','r');
x_rec=fread(fid1, N_samps,'short');
fclose(fid1);


xComplex_rec= zeros(1,N_samps/2);
count = 1;
for i=1:2:N_samps
    xComplex_rec(count) = complex(x_rec(i),x_rec(i+1));
    count = count+1;
end

length(xComplex_rec);

figure(1)
subplot(2,1,1)
plot(real(xComplex_rec));
subplot(2,1,2)
semilogy(linspace(0,1,2^16),abs(fft(xComplex_rec,2^16)))
%pwelch(xComplex_rec)

%% Processing stages


fid1=fopen('x_freq.dat','r');
x_freq=fread(fid1, N_samps/2,'double');
fclose(fid1);


xComplex_freq= zeros(1,N_samps/2);
count = 1;
for i=1:2:N_samps
    xComplex_freq(count) = complex(x_freq(i),x_freq(i+1));
    count = count+1;
end

length(xComplex_freq);

figure(2)
subplot(3,2,1)
plot(real(xComplex_freq));

fid1=fopen('x_matched.dat','r');
x_matched=fread(fid1, N_samps,'double');
fclose(fid1);


xComplex_matched= zeros(1,N_samps/2);
count = 1;
for i=1:2:N_samps
    xComplex_matched(count) = complex(x_matched(i),x_matched(i+1));
    count = count+1;
end

length(xComplex_matched);

subplot(3,2,3)
plot(real(xComplex_matched));



fid1=fopen('x_downsamp.dat','r');
x_downsamp=fread(fid1, 2*9480,'double');
fclose(fid1);


xComplex_downsamp= zeros(1,9480);
count = 1;
for i=1:2:9480*2
    xComplex_downsamp(count) = complex(x_downsamp(i),x_downsamp(i+1));
    count = count+1;
end

length(xComplex_downsamp);

subplot(3,2,2)
plot(xComplex_downsamp,'.');


fid1=fopen('xcorr.dat','r');
xcorr=fread(fid1, N_samps,'double');
fclose(fid1);


xComplex_corr= zeros(1,N_samps/2);
count = 1;
for i=1:2:N_samps
    xComplex_corr(count) = complex(xcorr(i),xcorr(i+1));
    count = count+1;
end

length(xComplex_corr);

figure(2)
subplot(3,2,5)
plot(abs(xComplex_corr));



fid1=fopen('x_filt.dat','r');
x_filt=fread(fid1, 2*9480,'double');
fclose(fid1);


xComplex_filt= zeros(1,9480);
count = 1;
for i=1:2:9480*2
    xComplex_filt(count) = complex(x_filt(i),x_filt(i+1));
    count = count+1;
end

length(xComplex_filt);

subplot(3,2,4)
plot(xComplex_filt,'.');


% Load data_sent from file (note the type)
fid=fopen('data.dat','r');
data_correct=fread(fid, 18760 ,'double');
fclose(fid);


% Load data from file (note the type)
fid=fopen('decoded0.dat','r');
temp=fread(fid, 18760 ,'short');
fclose(fid);
result_from_harness=temp;

figure(2)
subplot(3,2,6)
plot(abs(result_from_harness-data_correct));
title('C++ Rx processing');


BER_cpp=sum(abs(result_from_harness-data_correct))/18760
