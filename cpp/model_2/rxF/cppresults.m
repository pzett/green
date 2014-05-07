% harness.m
% For more information search for "README:" in harness.cpp.

clear all;
close all;

% Read data from file
load('rx_data_23.mat')
load('tx_data_for_inter.mat')


N_samps=2*86000;
fid1=fopen('received0.dat','r');
x_rec=fread(fid1, N_samps,'short');
fclose(fid1);

received_data= zeros(1,N_samps/2);
count = 1;
for i=1:2:N_samps
    received_data(count) = complex(x_rec(i),x_rec(i+1));
    count = count+1;
end

save('testCPP.mat','received_data')

figure(1)
subplot(4,2,1)
plot(real(received_data))
title('Data received')
subplot(4,2,2)
semilogy(abs(fft(received_data)))
title('Data fft')


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
 
  figure(1)
  subplot(4,2,5)
  plot(int_pilot_gain(1,:))
  title('Channel Gain Estimation')

  figure(2)
  subplot(2,1,1)
  surf(20*log10(int_pilot_gain))
  title('gain (dB scale)')
 
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
 
  figure(1)
  subplot(4,2,6)
  stem(int_pilot_phase(1,:))
  title('Channel Phase Estimation')
  
  figure(2)
  subplot(2,1,2)
  surf(int_pilot_phase)
 
  %load OFDM symbols corrected and without CP:
  
 fid=fopen('qpsk_data.dat','r');
 temp=fread(fid,2*89*562,'double');
 fclose(fid); 

 count=1; 
 data_qpsk=zeros(562,89);
 for i=1:562
    for k=1:89
        data_qpsk(i,k)=complex(temp(count),temp(count+1));   
        count=count+2;
    end
 end
 
  figure(1)
  subplot(4,2,7)
  plot(reshape(data_qpsk(20:end,:),1,89*543),'.b')
  title('OFDM QPSK received constellation')
 
  figure(3)
  plot(reshape(data_qpsk(20:end,1:46),1,24978),'.b')
  
  %load binary data:
  
 fid=fopen('dataBin.dat','r');
 temp=fread(fid, 2*89*562,'short');
 fclose(fid); 
 
 

 rx_data_bin=temp;
 
 error=(abs(data.'-rx_data_bin));
 BER=sum(error)/length(rx_data_bin)
 
 figure(1)
  subplot(4,2,8)
  plot(error)
  title('BER')


