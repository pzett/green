function [data] = OFDM_inv(data_OFDM,N,L,M)
% OFDM_inv uses modulated data and demodulates it
% INPUT:
%
% data_OFDM             modulated data 
% N                     total number of carriers we use
% L                     length of cyclic prefix
% M                     length of cyclic suffix   

%data_OFDM=reshape(data_OFDM,N+L,length(data_OFDM)/(N+L));
data=zeros(N,size(data_OFDM,2));
% for i=1:size(data,2)
%     A_off=sum(conj(data_OFDM(1:L,i)).*data_OFDM(end-L+1-M:end-M,i));  
% end
% nu_off=angle(A_off)/N;%-1*2*pi/N;
for i=1:size(data,2)
    nu_off=angle(sum(conj(data_OFDM(1:L,i)).*data_OFDM(end-L+1-M:end-M,i)))/(2*pi*N);
    data(:,i)=fft(data_OFDM(L+1:end-M,i).*exp(-1j*2*pi*nu_off*(0:size(data_OFDM,1)-M-L-1)'));
end
end

