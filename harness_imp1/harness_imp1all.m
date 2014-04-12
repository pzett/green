% harness.m
% For more information search for "README:" in harness.cpp.

clear ;
close all;

% Read data from file -> rx_data
load('rx_test.mat');
rx_data_test=5000*rx_data_test;
%plot(abs(fft(rx_data_test,2^20)))
temp=zeros(1,2*length(rx_data_test));
for i1=1:size(rx_data_test,2)
   temp(i1*2-1)=real(rx_data_test(1,i1));
   temp(i1*2-0)=imag(rx_data_test(1,i1));
end;

% Write to file (note the type of the data used)
fid=fopen('data_to_harness.dat','w');
fwrite(fid,temp,'short');
fclose(fid);

buffer_size=2*length(rx_data_test);

% Run the harness command
cmd_str=['./harness_imp1all '];
cmd_str=[cmd_str,' --buffer_size=',num2str(buffer_size)];
cmd_str
system(cmd_str);


nr_data_bits = 6250;  


%C++ results done here 

%Read train sequence from file
fid1=fopen('train_norm.dat','r');
x=fread(fid1,100*2,'double');
fclose(fid1);

train= zeros(1,100);
count = 1;
for i=1:2:100*2
    train(count) = complex(x(i),x(i+1));
    count = count+1;
end
train_up=upfirdn(train,ones(4,1),4,1);


% figure
% subplot(3,2,1)
% plot(real(rx_data_test))
% subplot(3,2,3)
% plot(imag(rx_data_test))
% subplot(3,2,5)
% plot(abs(rx_data_test))
% subplot(3,2,2)
% plot(real(train_up))
% subplot(3,2,4)
% plot(imag(train_up))
% subplot(3,2,6)
% plot(abs(train_up))
% o=xcorr(abs(rx_data_test),abs(train_up))
% figure
% plot(o)


%Parameters to the script:
rx_data=rx_data_test.';


% Run the matlab implementation -> Call the script that process that

imp1_rx_sim




% Load data from file (note the type)
fid=fopen('data_from_harness.dat','r');
temp=fread(fid, nr_data_bits ,'short');
fclose(fid);


result_from_harness=temp;

%figure(1);
%hold off
%plot(result_from_harness,'ok');
%hold on
%plot(result_from_matlab,'-x');
%legend('C++ implementation (harness)','matlab implementation');
%axis([-1000 1000 -1000 1000])

% Load data_sent from file (note the type)
fid=fopen('data.dat','r');
data_correct=fread(fid, 6250 ,'double');
fclose(fid);

ERROR_cpp=sum(abs(result_from_harness-data_correct))/6250

% Load data_sent from file (note the type)
fid=fopen('data.dat','r');
data_correct=fread(fid, 6250 ,'double');
fclose(fid);

length(rx_data_detect);

BER_MATLAB=sum(abs(data_correct.'-rx_data_detect))/6250

subplot(3,2,6)
plot(abs(rx_data_detect'-data_correct)/2);
title('Matlab Rx processing');

figure(2)
cppresults
subplot(3,2,6)
plot(abs(result_from_harness-data_correct)/2);
title('C++ Rx processing');






