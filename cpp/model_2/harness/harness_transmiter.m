clc;clear;close all;

% loading data
load('parameters');
modulation_type='qpsk';
nr_bits_x_symbol=2;

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

trainSeq=zeros(1,2*length(train_seq));
w=1;
for m=1:length(train_seq)
trainSeq(w)=real(train_seq(m));
w=w+1;
trainSeq(w)=imag(train_seq(m));
w=w+1;
end



[tx_data,data,data_mapped] = transmiter;

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


% Write to file (note the type of the data used)
fid=fopen('dataTrain.dat','w');
fwrite(fid,trainSeq,'double');
fclose(fid);

% Run the harness command
cmd_str=['./harness_tx '];
cmd_str=[cmd_str,' --nCar=',num2str(nr_car)];
cmd_str=[cmd_str,' --nUsedC=',num2str(nr_used_car)];
cmd_str=[cmd_str,' --nPilotC=',num2str(nr_pilot_car)];
cmd_str=[cmd_str,' --pre=',num2str(length_cpre)];
cmd_str=[cmd_str,' --post=',num2str(length_suf)];
cmd_str=[cmd_str,' --nDataBin=',num2str(length(data))];
cmd_str=[cmd_str,' --nPilotData=',num2str(length(piloT))];
cmd_str=[cmd_str,' --nBits=',num2str(nr_bits_x_symbol/2)];
cmd_str
system(cmd_str);

% Load data from file (note the type)
fid=fopen('data_from_harness.dat','r');
result_from_harness=fread(fid,2*(2*nr_guard_bits+4*nr_training_bits+(floor((length(data)-1)/(nr_used_car*nr_bits_x_symbol))+1)*(nr_car+length_cpre+length_suf)),'short');
fclose(fid);

xComplex= zeros(1,length(result_from_harness)/2);
count = 1;
for i=1:2:length(result_from_harness)
    xComplex(count) = complex(result_from_harness(i),result_from_harness(i+1));
    count = count+1;
end


