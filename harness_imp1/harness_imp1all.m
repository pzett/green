% harness.m
% For more information search for "README:" in harness.cpp.

clear all;
close all;

% Read data from file -> rx_data
load('rx_test.mat');

rx_data_test=5000*rx_data_test;

plot(abs(rx_data_test))

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

% Load data from file (note the type)
fid=fopen('data_from_harness.dat','r');
temp=fread(fid, nr_data_bits ,'double');
fclose(fid);

%C++ results done here 

%Read train sequence from file
fid1=fopen('train_norm.dat','r');
train_seq_double=fread(fid1,100*2,'double');
fclose(fid1);

count = 1;
for i=1:2:(100*2)
    train(count) = complex(train_seq_double(i),train_seq_double(i+1));
    count = count+1;
end


%Parameters to the script:
rx_data=rx_data_test.';
train;

% Run the matlab implementation -> Call the script that process that
imp1_rx_sim

result_from_harness=zeros(1,length(temp)/2);
i2=1;
for i1=1:2:length(temp)
   result_from_harness(i2)=temp(i1)+i*temp(i1+1);
   i2=i2+1;
end;

%figure(1);
%hold off
%plot(result_from_harness,'ok');
%hold on
%plot(result_from_matlab,'-x');
%legend('C++ implementation (harness)','matlab implementation');
%axis([-1000 1000 -1000 1000])

%ERROR=sum(abs(result_from_harness-result_from_matlab.'))

% Load data_sent from file (note the type)
fid=fopen('data.dat','r');
data_correct=fread(fid, 6250 ,'double');
fclose(fid);

length(rx_data_detect)

BER=sum(abs(data_correct.'-rx_data_detect))/6250





