% harness.m
% For more information search for "README:" in harness.cpp.

clear all;
close all;

% Read data from file
%load('test_function_filter_phase.mat')

%data

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

rx_mod=xComplex_downsamp;


%train
fid1=fopen('train_norm.dat','r');
x=fread(fid1,100*2,'double');
fclose(fid1);

tComplex= zeros(1,100);
count = 1;
for i=1:2:100*2
    tComplex(count) = complex(x(i),x(i+1));
    count = count+1;
end


train=tComplex;

phi_hat=0;


temp=zeros(1,2*length(train));
for i1=1:size(train,2)
   temp(i1*2-1)=real(train(1,i1));
   temp(i1*2-0)=imag(train(1,i1));
end;

% Write to file (note the type of the data used)
fid=fopen('train_to_harness.dat','w');
fwrite(fid,temp,'double');
fclose(fid);

temp=zeros(1,2*length(rx_mod));
for i1=1:size(rx_mod,2)
   temp(i1*2-1)=real(rx_mod(1,i1));
   temp(i1*2-0)=imag(rx_mod(1,i1));
end;

% Write to file (note the type of the data used)
fid=fopen('data_to_harness.dat','w');
fwrite(fid,temp,'double');
fclose(fid);


% Run the harness command
cmd_str=['./harness_filt '];
cmd_str=[cmd_str,' --phi_hat=',num2str(phi_hat),' ','--data_size=',num2str(length(rx_mod)),' ','--train_size=',num2str(length(train))];
cmd_str
system(cmd_str);

% Load data from file (note the type)
fid=fopen('data_from_harness.dat','r');
temp=fread(fid,2*(length(rx_mod)-length(train)),'double');
fclose(fid);


% Run the matlab implementation
result_from_matlab=filter_phase(rx_mod, phi_hat, train, 0.01, 0.01,0.01);



result_from_harness=zeros(length(temp)/2,1);
i2=1;
for i1=1:2:length(temp)
   result_from_harness(i2)=complex(temp(i1),temp(i1+1));
   i2=i2+1;
end;

figure(1);
hold off
plot(result_from_harness,'.');
hold on
plot(result_from_matlab,'o');
legend('C++ implementation (harness)','matlab implementation');
%axis([-1000 1000 -1000 1000])

ERROR=sum(abs(result_from_harness-result_from_matlab))
