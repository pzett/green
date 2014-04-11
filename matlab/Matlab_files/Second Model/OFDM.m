function [data_OFDM] = OFDM(data,L)
%OFDM 
% INPUT: 
% data      data that has to be modulated        
% L         lengh of the cyclic prefix
%data1=reshape(data,N,length(data)/N);
N=size(data,1);
data_OFDM=zeros(N+L,size(data,2));

    for i=1:size(data,2)
        data_OFDM(end-N+1:end,i)=ifft(data(:,i));
        data_OFDM(1:L,i)=data_OFDM(end-L+1:end,i);
    end
data_OFDM=reshape(data_OFDM,size(data_OFDM,2)*(N+L),1);
end