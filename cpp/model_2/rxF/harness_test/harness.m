% harness.m
% For more information search for "README:" in harness.cpp.

clear all;
close all;

% Read data from file
load('rx_data_23.mat')
load('tx_data_for_inter.mat')

figure(1)
subplot(4,2,1)
plot(real(rx_data_23))
title('Data for test')
subplot(4,2,2)
semilogy(abs(fft(rx_data_23)))
title('Data fft')



temp=zeros(1,2*length(rx_data_23));
for i1=1:length(rx_data_23)
   temp(i1*2-1)=real(rx_data_23(1,i1));
   temp(i1*2-0)=imag(rx_data_23(1,i1));
end;

% Write to file (note the type of the data used)
fid=fopen('data_to_harness.dat','w');
fwrite(fid,temp,'short');
fclose(fid);

nDataB=1e6;

%Run the harness command
cmd_str=['./harness '];
cmd_str=[cmd_str,' --buffersize=',num2str(length(temp)),...
    ' ','--nDataB=',num2str(nDataB)];
cmd_str
system(cmd_str);

% Load cpp data from file s(note the type)

  %load cross correlation:  
  fid1=fopen('xcorr.dat','r');
  xcorrAbs=fread(fid1, 165364,'double');
  fclose(fid1);

  length(xcorrAbs);

  figure(1)
  subplot(4,2,3)
  plot(xcorrAbs);
  title('xcorr')

  %load OFDM symbols corrected and without CP:
  
 fid=fopen('ofdm_cp.dat','r');
 temp=fread(fid,143872,'double');
 fclose(fid); 

 count=1; 
 dataOFDM_cpp=zeros(562,128);
 for i=1:562
    for k=1:128
        dataOFDM_cpp(i,k)=complex(temp(count),temp(count+1));   
        count=count+2;
    end
 end
 
  figure(1)
  subplot(4,2,4)
  plot(real((dataOFDM_cpp(1,:))));
  title('1st OFDM symbol')

  %load received Pilot gain:
  
 fid=fopen('rx_pilot_gain.dat','r');
 temp=fread(fid,10*562,'double');
 fclose(fid); 

 count=1; 
 pilot_gain=zeros(562,10);
 for i=1:562
    for k=1:10
        pilot_gain(i,k)=temp(count);   
        count=count+1;
    end
 end
 
 %load received Pilot phase:
  
 fid=fopen('rx_pilot_phase.dat','r');
 temp=fread(fid,10*562,'double');
 fclose(fid); 

 count=1; 
 pilot_phase=zeros(562,10);
 for i=1:562
    for k=1:10
        pilot_phase(i,k)=temp(count);   
        count=count+1;
    end
 end
 
 
 fid=fopen('cons_ofdm.dat','r');
 temp=fread(fid,2*89*562,'double');
 fclose(fid); 

 count=1; 
 consOFDM_cpp=zeros(562,89);
 for i=1:562
    for k=1:89
        consOFDM_cpp(i,k)=complex(temp(count),temp(count+1));   
        count=count+2;
    end
 end
 
 %load filtered Pilot gain:
  
 fid=fopen('filt_pilot_gain.dat','r');
 temp=fread(fid,10*562,'double');
 fclose(fid); 

 count=1; 
 filt_pilot_gain=zeros(562,10);
 for i=1:562
    for k=1:10
        filt_pilot_gain(i,k)=temp(count);   
        count=count+1;
    end
 end
 
 %load filtered Phase gain:
  
 fid=fopen('filt_pilot_phase.dat','r');
 temp=fread(fid,10*562,'double');
 fclose(fid); 

 count=1; 
 filt_pilot_phase=zeros(562,10);
 for i=1:562
    for k=1:10
        filt_pilot_phase(i,k)=temp(count);   
        count=count+1;
    end
 end
 
 
 %load interp Pilot gain:
  
 fid=fopen('int_pilot_gain.dat','r');
 temp=fread(fid,128*562,'double');
 fclose(fid); 

 count=1; 
 int_pilot_gain=zeros(562,128);
 for i=1:562
    for k=1:128
        int_pilot_gain(i,k)=temp(count);   
        count=count+1;
    end
 end
 
 %load interp Phase gain:
  
 fid=fopen('int_pilot_phase.dat','r');
 temp=fread(fid,128*562,'double');
 fclose(fid); 

 count=1; 
 int_pilot_phase=zeros(562,128);
 for i=1:562
    for k=1:128
        int_pilot_phase(i,k)=temp(count);   
        count=count+1;
    end
 end
 
  %load binary data:
  
 fid=fopen('dataBin.dat','r');
 temp=fread(fid, 2*89*562,'short');
 fclose(fid); 


 rx_data_bin=temp;
 
 BER=sum(abs(data.'-rx_data_bin))/length(rx_data_bin)
 
 
%   figure(1)
%   subplot(4,2,4)
%   plot(real((dataOFDM_cpp(1,:))));
%   title('1st OFDM symbol')
%Matlab simulation:

