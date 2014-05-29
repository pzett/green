function [phi_out,K_out,I_out]=kalmanF(rx_data,I0,K0,R1,R2,pilot_pattern,pilot)
%------Code used when one want to use kalmanF------------------------
%                 rx_data_S=rx_data(:,2:end)./(ones(32,1)*exp(1j*2*pi*epsilon*(nr_car+length_cpre+length_suf)*[1:nr_OFDM_symbols-1]));
%                 channel=channel/channel(1);
%                 I0=1;
%                 K0=0;
%                 phi_out=zeros(1,nr_OFDM_symbols-1);
%                 for v=1:nr_OFDM_symbols-1
%                   [phi_out(v),K,I]=kalmanF(rx_data_S(:,v),I0,4*pi*((2*N^2+1)/(3*N)+l_cp)*R1,0.01,0.1,pilot_pattern,fft(channel,32),length_cpre,nr_car,pilot((v-1)*length_pilot+1:v*length_pilot));
%                   I0=I;
%                   K0=K;
%                 end
%               data_received_equalized=rx_data_S.*(fft(channel,32)*exp(-1j*phi_out));
I_pred=zeros(1,length(pilot_pattern));
I_filt=zeros(1,length(pilot_pattern));
K=zeros(1,length(pilot_pattern));
K_pred=zeros(1,length(pilot_pattern));

I_pred(1)=I0;
K_pred(1)=K0;

A=pilot;

for k=1:length(pilot_pattern)
    G=(K_pred(1)*A(k)')/(A(k)*K_pred(1)*A(k)'+R2);
    I_filt(k)=I_pred(k)+G*(rx_data(pilot_pattern(k))-A(k)*I_pred(k));
    K(k)=(1-G*A(k))*K_pred(k);
    K_pred(k+1)=K(k)+R1;
    I_pred(k+1)=I_filt(k);
end

phi_out=angle(I_filt(length(pilot_pattern)));
K_out=K(end);
I_out=I_filt(length(pilot_pattern));