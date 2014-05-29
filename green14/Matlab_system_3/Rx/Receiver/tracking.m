function [data] = tracking(data_FFT, pilot_pattern, nr_car,length_cpre)
% OFDM_inv uses modulated data and demodulates it
% INPUT:
%
% data_OFDM             modulated data 
% N                     total number of carriers we use
% L                     length of cyclic prefix
% M                     length of cyclic suffix   

L=10;

for i=1:size(data_FFT,2)-L+1
    [delta_nu, eta]=LLSE_tracking(data_FFT(pilot_pattern,i:i+L-1), pilot_pattern,nr_car, length_cpre ,L );
    step=[0:1:size(data_FFT,2)-i]*(nr_car+length_cpre)+length_cpre;
    phase_corr=2*pi*delta_nu*ones(nr_car,1)*step+2*pi/nr_car*[0:1:nr_car-1]'*step*eta;
    data_FFT(:,i:end)=data_FFT(:,i:end).*exp(-1i*phase_corr);
    
end
data=data_FFT;
end

