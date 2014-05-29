function [ tx_data ] = channel_fct( tx_data, sigma_sqr_AWGN, frequency_offset, phase_offset, LO_leakage, channel_FIR, channel_gain, phase_noise )
%Channel Summary of this function goes here
%   Detailed explanation goes here
length_tx_data=length(tx_data);

 tx_data=channel_gain.*(filter(channel_FIR, 1, tx_data)); 
%AWGN
% figure
% plot(abs(tx_data))
if sqrt(sigma_sqr_AWGN)
   
w=sqrt(sigma_sqr_AWGN)*(randn(length_tx_data,1)+1i*randn(length_tx_data,1));
tx_data=tx_data+w;
% figure
% plot(abs(tx_data),'r')
% sum(w.*conj(w))

end
freq=frequency_offset(1)+frequency_offset(2);
if 1
    tx_data=tx_data.*exp(2*pi*1j*freq*(1:length_tx_data)');
end
    phase= rand(1)*2*pi;
if phase_offset 
    
    tx_data=tx_data.*exp(1i*phase);
end

 
 if LO_leakage
     p=(0:length_tx_data-1);
     tx_data=tx_data+exp(1i*2*pi*LO_leakage*(p'));
 end
 
 %phase_noise_freq=[100,1e3,10e3,100e3,1e6];%,10e6];   our  [1e1 1e2 1e3 1e4 1e5 1e6 1e7]
%phase_noise_power=[-83,-85,-85,-112,-132];%,-145];    our  [-45 -55 -64 -70 -78 -89 -110]
 if phase_noise==1
     tx_data=add_phase_noise(tx_data, 25e6,[1e1 1e2 1e3 1e4 1e5 1e6 1e7],[-45 -55 -64 -70 -78 -89 -110],0); %our
 elseif phase_noise==2
      tx_data=add_phase_noise(tx_data, 25e6,[100,1e3,10e3,100e3,1e6, 10e6], [-83,-85,-85,-112,-132, -145],0); %good
     
 elseif phase_noise==3
      tx_data=add_phase_noise(tx_data, 25e6,[1e3,10e3,100e3,1e6], [-30,-60,-90,-100],0); %bad
 end
end

