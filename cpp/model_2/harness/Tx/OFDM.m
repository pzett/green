function [data_OFDM] = OFDM(data,L,M)
%This function creates
%
%
% INPUT: 
% data      data that has to be modulated        
% L         lentgh of the cyclic prefix
% M         length of the sufix


%data1=reshape(data,N,length(data)/N);
N=size(data,1);                         %Number of subcarriers
data_OFDM=zeros(N+L+M,size(data,2));

    for i=1:size(data,2)
        data_OFDM(end-N+1-M:end-M,i)=ifft(data(:,i));
        data_OFDM(1:L,i)=data_OFDM(end-L+1-M:end-M,i);
        data_OFDM(end-M+1:end,i)=data_OFDM(L+1:L+M,i);
    end
    
data_OFDM=reshape(data_OFDM,size(data_OFDM,2)*(N+L+M),1);

end
