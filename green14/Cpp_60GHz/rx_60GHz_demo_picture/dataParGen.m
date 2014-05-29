% data and parameter generator
clc;clear;close all;
load('parameters16-QAM32');
%load('golay12');

%lengthOfDataSeq = 100000;
%data = a(1:lengthOfDataSeq);

%Q = 2; pulse_shape=[1 1];

nr_data_bits = 10000;
data = generate_data(nr_data_bits);
% transfer and save pilots and training seq
piloT=zeros(1,size(pilot,1)*size(pilot,2));
n=1;
for j=1:size(pilot,2)
    for i=1:size(pilot,1)
    piloT(n)=real(pilot(i,j));
    n=n+1;
    piloT(n)=imag(pilot(i,j));
    n=n+1;
    end
end

%train_seq = training;
trainSeq=zeros(1,2*length(train_seq));
w=1;
for m=1:length(train_seq)
trainSeq(w)=real(train_seq(m));
w=w+1;
trainSeq(w)=imag(train_seq(m));
w=w+1;
end

% Write to file (note the type of the data used)
fid=fopen('dataBinNPaq.dat','w');
fwrite(fid,data,'double');
fclose(fid);

% Write to file (note the type of the data used)
fid=fopen('dataPilotN.dat','w');
fwrite(fid,piloT,'double');
fclose(fid);

% Write to file (note the type of the data used)
fid=fopen('dataPattern.dat','w');
fwrite(fid,car_pattern,'double');
fclose(fid);

% Write to file (note the type of the data used)
fid=fopen('pilotPattern.dat','w');
fwrite(fid,pilot_pattern,'double');
fclose(fid);

% % Write to file (note the type of the data used)
% fid=fopen('dataTrain.dat','w');
% fwrite(fid,training,'double');
% fclose(fid);

% Write to file (note the type of the data used)
fid=fopen('dataTrain.dat','w');
fwrite(fid,train_seq,'double');
fclose(fid);