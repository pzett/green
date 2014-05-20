

%Script that compputes the BER of the received data:

clear all
close all


load('params.mat')
load('dData.mat')

nr_OFDM_symbols = 200;


N_samps=11000;

N_test=320%1000;

Modulation=2;

error_test=zeros(1,N_test);

for var=0:N_test-1

    
    
    filename=['data/received',num2str(var),'.dat'];
    
    fid1=fopen(filename,'r');
    x_rec=fread(fid1, 2*N_samps,'short');
    fclose(fid1);

    received_data= zeros(1,N_samps);
    count = 1;
    for count=1:2:2*N_samps
        received_data(count) = complex(x_rec(count),x_rec(count+1));
        count = count+1;
    end

    
    filename=['data/decoded',num2str(var),'.dat'];
    
    fid=fopen(filename,'r');
    temp=fread(fid, Modulation*nr_used_car*nr_OFDM_symbols,'short');
    fclose(fid); 
    
    rx_data_bin=temp;
 
    error=(abs(dataT(1:Modulation*nr_used_car*nr_OFDM_symbols).'-rx_data_bin));
    error_test(var+1)=sum(error)/length(error);
    
    
end

    BER=sum(error_test)/length(error_test)
    
    
    
    var=75;
    
    filename=['data/received',num2str(var),'.dat'];
    
    fid1=fopen(filename,'r');
    x_rec=fread(fid1, 2*N_samps,'short');
    fclose(fid1);
    
    received_data= zeros(1,N_samps);
    count = 1;
    for i=1:2:2*N_samps
        received_data(count) = complex(x_rec(i),x_rec(i+1));
        count = count+1;
    end
% 
%     %save('testCPP.mat','received_data')

    figure(1)
    plot(error_test)

    figure(2)
    subplot(2,1,1)
    plot(real(received_data))
    title('Data received')
    subplot(2,1,2)
    semilogy(abs(fft(received_data)))
    title('Data fft')
    
    
    
    
    
    
    


