function [data] = OFDM_inv(data_OFDM,N,L)
% OFDM_inv uses modulated data and demodulates it
% INPUT:
%
% data_OFDM             modulated data  
% N                     total number of carriers we use
% L                     length of cyclic prefix
% 

%data_OFDM=reshape(data_OFDM,N+L,length(data_OFDM)/(N+L));
data=zeros(N,size(data_OFDM,2));
for i=1:size(data,2)
    nu_off=angle(sum(conj(data_OFDM(1:L,i)).*data_OFDM(end-L+1:end,i)))/N;
  
    data(:,i)=fft(data_OFDM(L+1:end,i).*exp(-j*nu_off*(L+1:size(data_OFDM,1))'));
end
end

