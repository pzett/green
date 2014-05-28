% cppresults.m
% This script is use to display the result of the c++ receiver on a graphical format.
clear all;
close all;

% Read data from file
load('params.mat')
load('dData.mat')

nr_OFDM_symbols = 800;


N_samps=40000;
fid1=fopen('received.dat','r');
x_rec=fread(fid1, 2*N_samps,'short');
fclose(fid1);

received_data= zeros(1,N_samps);
count = 1;
for i=1:2:2*N_samps
    received_data(count) = complex(x_rec(i),x_rec(i+1));
    count = count+1;
end

%load Residual Frequency Corrected data:

fid1=fopen('freqCorr.dat','r');
x_rec=fread(fid1, 2*N_samps,'double');
fclose(fid1);

received_data_corr= zeros(1,N_samps);
count = 1;
for i=1:2:2*N_samps
    received_data_corr(count) = complex(x_rec(i),x_rec(i+1));
    count = count+1;
end


%save('testCPP.mat','received_data')

figure(1)
subplot(4,2,1)
plot(real(received_data))
title('Data received')
subplot(4,2,2)
semilogy(abs(fft(received_data)))
title('Data fft')


  %load cross correlation:  
  fid1=fopen('xcorr.dat','r');
  xcorrAbs=fread(fid1, N_samps,'double');
  fclose(fid1);

  length(xcorrAbs);

  figure(1)
  subplot(4,2,3)
  plot(xcorrAbs);
  title('xcorr')

  %load OFDM symbols corrected and without CP:
  
 fid=fopen('ofdm_cp.dat','r');
 temp=fread(fid,2*nr_car*nr_OFDM_symbols,'double');
 fclose(fid); 

 count=1; 
 dataOFDM_cpp=zeros(nr_OFDM_symbols,nr_car);
 for i=1:nr_OFDM_symbols
    for k=1:nr_car
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
 temp=fread(fid,length_pilot*nr_OFDM_symbols,'double');
 fclose(fid); 

 count=1; 
 pilot_gain=zeros(nr_OFDM_symbols,length_pilot);
 for i=1:nr_OFDM_symbols
    for k=1:length_pilot
        pilot_gain(i,k)=temp(count);   
        count=count+1;
    end
 end
 
 %load received Pilot phase:
  
 fid=fopen('rx_pilot_phase.dat','r');
 temp=fread(fid,length_pilot*nr_OFDM_symbols,'double');
 fclose(fid); 

 count=1; 
 pilot_phase=zeros(nr_OFDM_symbols,length_pilot);
 for i=1:nr_OFDM_symbols
    for k=1:length_pilot
        pilot_phase(i,k)=temp(count);   
        count=count+1;
    end
 end
 
 
 fid=fopen('cons_ofdm.dat','r');
 temp=fread(fid,2*nr_used_car*nr_OFDM_symbols,'double');
 fclose(fid); 

 count=1; 
 consOFDM_cpp=zeros(nr_OFDM_symbols,nr_used_car);
 for i=1:nr_OFDM_symbols
    for k=1:nr_used_car
        consOFDM_cpp(i,k)=complex(temp(count),temp(count+1));   
        count=count+2;
    end
 end
 
 %load filtered Pilot gain:
  
 fid=fopen('filt_pilot_gain.dat','r');
 temp=fread(fid,length_pilot*nr_OFDM_symbols,'double');
 fclose(fid); 

 count=1; 
 filt_pilot_gain=zeros(nr_OFDM_symbols,length_pilot);
 for i=1:nr_OFDM_symbols
    for k=1:length_pilot
        filt_pilot_gain(i,k)=temp(count);   
        count=count+1;
    end
 end
 
 %load filtered Phase gain:
  
 fid=fopen('filt_pilot_phase.dat','r');
 temp=fread(fid,length_pilot*nr_OFDM_symbols,'double');
 fclose(fid); 

 count=1; 
 filt_pilot_phase=zeros(nr_OFDM_symbols,length_pilot);
 for i=1:nr_OFDM_symbols
    for k=1:length_pilot
        filt_pilot_phase(i,k)=temp(count);   
        count=count+1;
    end
 end
 
 
 %load interp Pilot gain:
  
 fid=fopen('int_pilot_gain.dat','r');
 temp=fread(fid,nr_car*nr_OFDM_symbols,'double');
 fclose(fid); 

 count=1; 
 int_pilot_gain=zeros(nr_OFDM_symbols,nr_car);
 for i=1:nr_OFDM_symbols
    for k=1:nr_car
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
 temp=fread(fid,nr_car*nr_OFDM_symbols,'double');
 fclose(fid); 

 count=1; 
 int_pilot_phase=zeros(nr_OFDM_symbols,nr_car);
 for i=1:nr_OFDM_symbols
    for k=1:nr_car
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
 temp=fread(fid,2*nr_used_car*nr_OFDM_symbols,'double');
 fclose(fid); 

 count=1; 
 data_qpsk=zeros(nr_OFDM_symbols,nr_used_car);
 for i=1:nr_OFDM_symbols
    for k=1:nr_used_car
        data_qpsk(i,k)=complex(temp(count),temp(count+1));   
        count=count+2;
    end
 end
 
  figure(1)
  subplot(4,2,7)
  plot(reshape(data_qpsk(1:end,:),1,nr_used_car*nr_OFDM_symbols),'.b')
  title('OFDM QPSK received conste1llation')
  
  
  figure(3)
  %plot(reshape(data_qpsk(20:end,1:46),1,24978),'.b')
  plot(reshape(data_qpsk(1:end,1:end),1,nr_used_car*nr_OFDM_symbols-0*nr_used_car),'.b') 
  grid on
  hold on
  figure(3)
  plot((1/sqrt(2))*[1+1i , 1-1i, -1-1i, -1+1i],'X')
  %load binary data:
  
 %Modulation=2;%QPSK
 Modulation=4;%16-QAM
  
 fid=fopen('dataBin.dat','r');
 temp=fread(fid, Modulation*nr_used_car*nr_OFDM_symbols,'short');
 fclose(fid); 
 
 count=1; 
 data_B_cpp=zeros(nr_OFDM_symbols,Modulation*nr_used_car);
 data_B_trans=zeros(nr_OFDM_symbols,Modulation*nr_used_car);
 for i=1:nr_OFDM_symbols
    for k=1:Modulation*nr_used_car
        data_B_cpp(i,k)=temp(count);
        data_B_trans(i,k)=dataT(count);
        count=count+1;
    end
 end
 
 figure(4)
 imagesc(abs(data_B_cpp-data_B_trans));
 

 rx_data_bin=temp;
 
 error=(abs(dataT(1:Modulation*nr_used_car*nr_OFDM_symbols).'-rx_data_bin));
 BER=sum(error)/length(rx_data_bin)
 
 figure(1)
  subplot(4,2,8)
  plot(error)
  title('BER')
  
  data_rate=(1-2*BER)*(length(rx_data_bin)/N_samps)*25;
        ['Data rate: ', num2str(data_rate), 'Mbits/s']
        
        
        
        


