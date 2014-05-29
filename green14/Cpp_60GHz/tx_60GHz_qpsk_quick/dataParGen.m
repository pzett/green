% data and parameter generator
clc;
clear all;
close all;

load('parameters16-QAM32');
load('dData.mat')

% Write to file (note the type of the data used)
fid=fopen('dataBin.dat','w');
fwrite(fid,dataT,'double');
fclose(fid);


% transfer and save pilots
temp=zeros(1,size(pilot,1)*size(pilot,2));

n=1;
for j=1:size(pilot,2)
    for i=1:size(pilot,1)
    temp(n)=real(pilot(i,j));
    n=n+1;
    temp(n)=imag(pilot(i,j));
    n=n+1;
    end
end

temp = temp./rms(temp);


% Write to file (note the type of the data used)
fid=fopen('dataPilotN.dat','w');
fwrite(fid,temp,'double');
fclose(fid);


%Save training sequence
temp=zeros(1,2*length(train_seq));
w=1;
for m=1:length(train_seq)
    temp(w)=real(train_seq(m));
    w=w+1;
    temp(w)=imag(train_seq(m));
    w=w+1;
end

% Write to file (note the type of the data used)
fid=fopen('dataTrain.dat','w');
fwrite(fid,temp,'double');
fclose(fid);




% Write to file (note the type of the data used)
fid=fopen('dataPattern.dat','w');
fwrite(fid,car_pattern,'double');
fclose(fid);

% Write to file (note the type of the data used)
fid=fopen('pilotPattern.dat','w');
fwrite(fid,pilot_pattern,'double');
fclose(fid);


