% harness.m
% For more information search for "README:" in harness.cpp.

clear all;
close all;

% Read data from file
load('rx_data_23.mat')
load('test.mat')

%data:
received_data=rx_data_23.';
%train_seq
%Q
length_post=1;
length_pre=4;


%write data to C++

temp=zeros(1,2*length(train_seq));
for i1=1:length(train_seq)
   temp(i1*2-1)=real(train_seq(i1,1));
   temp(i1*2-0)=imag(train_seq(i1,1));
end;

% Write to file (note the type of the data used)
fid=fopen('train_to_harness.dat','w');
fwrite(fid,temp,'double');
fclose(fid);

temp=zeros(1,2*length(received_data));
for i1=1:length(received_data)
   temp(i1*2-1)=real(received_data(i1,1));
   temp(i1*2-0)=imag(received_data(i1,1));
end;

% Write to file (note the type of the data used)
fid=fopen('data_to_harness.dat','w');
fwrite(fid,temp,'double');
fclose(fid);


%Run the harness command
cmd_str=['./harness '];
cmd_str=[cmd_str,' --length_post=',num2str(length_post),' ','--length_pre=',num2str(length_pre),' ','--train_size=',num2str(length(train_seq)),' ','--data_size=', num2str(length(received_data))];
cmd_str
system(cmd_str);

% Load data from file (note the type)
% fid=fopen('data_from_harness.dat','r');
% temp=fread(fid,2*(length(rx_mod)-length(train)),'double');
% fclose(fid);


% Run the matlab implementation
[t_sampl, phi_hat, A_hat,theta_est sigma_sqr_noise] = sync_catch_channel(received_data, train_seq, Q,length_post,length_pre);
