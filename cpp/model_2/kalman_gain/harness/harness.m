% harness.m
% For more information search for "README:" in harness.cpp.

clear all;
close all;

% Read data from file
load('testKalman.mat')



F=1.0;
G=1.0;
H=1.0;
R1=0.001;
R2=0.01;
Q0=2.0;

%write data to C++
count=1;
temp=zeros(1, size(gain_pilot,1)*size(gain_pilot,2));
for i1=1:size(gain_pilot,2)
   for i2=1:size(gain_pilot,1)
    temp(count)=gain_pilot(i2,i1);
    count=count+1;
   end
end;

% Write to file (note the type of the data used)
fid=fopen('data_to_harness.dat','w');
fwrite(fid,temp,'double');
fclose(fid);

% Write to file (note the type of the data used)
fid=fopen('init_to_harness.dat','w');
fwrite(fid,gain_pilot_0,'double');
fclose(fid);

%Run the harness command
cmd_str=['./harness '];
cmd_str=[cmd_str,' --F=',num2str(F),...
    ' ','--G=',num2str(G),' ','--H=',...
    num2str(H),' ','--R1=', num2str(R1),...
    ' ','--R2=',num2str(R2),' ','--Q0=',num2str(Q0),' ','--nPilot=',num2str(nr_pilot)...
    ,' ','--data_size=',num2str(size(gain_pilot,1)*size(gain_pilot,2))];
cmd_str
system(cmd_str);

% Load data from file (note the type)
 fid=fopen('data_from_harness.dat','r');
 temp=fread(fid,size(gain_pilot,1)*size(gain_pilot,2),'double');
 fclose(fid); 

 count=1; 
gain_pilot_filt_cpp=zeros(size(gain_pilot,2),size(gain_pilot,1));
for i=1:size(gain_pilot,2)
    for k=1:size(gain_pilot,1)
        gain_pilot_filt_cpp(i,k)=temp(count);   
        count=count+1;
    end
end
gain_pilot_filt_matlab=zeros(size(gain_pilot,2),size(gain_pilot,1));
% Run the matlab implementation
for i=1:nr_pilot
 [~,gain_pilot_filt_matlab(:,i),~,~,Qf] = kalman(gain_pilot(i,:).',F,G,H,R1,R2,gain_pilot_0(i),Q0,0);
end


%Error calculation:
error=zeros(1,size(gain_pilot,2));
for i=1:size(gain_pilot,2)
    error(1,i)=sum(abs((gain_pilot_filt_matlab(i,:)-gain_pilot_filt_cpp(i,:))))/size(gain_pilot,1);

end

figure(1)
plot(error)
title('C++ error implementation of Kalman Gain function')
