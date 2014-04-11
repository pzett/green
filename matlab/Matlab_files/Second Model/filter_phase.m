% Function is supposed to filter the phase offset using state space model,
% it is a Kalman filter in a particular case with known phi_hat
%
%sigma_phi -  is defined as a constant term
%sigma_m   -  may vary in time
%
%          B(n)A+B(n)+A
% B(n+1)=--------------- 
%            B(n)+A
%   
%              A                     B(n) 
% phi(n+1)=--------phi(n)+       ---------*phi(n+1)
%           B(n)+A                 B(n)+A  
%         Q0
%  B0=-------------- +1
%       sigma_phi^2
%
%  sigma_phi_sqr=pi/4*1/number of bits transmitted
%  
function [Y] = filter_phase(rx_data, phi_hat, train, Q0, sigma_phi_sqr, sigma_m_sqr)
B=Q0/sigma_phi_sqr+1;
A=sigma_m_sqr/sigma_phi_sqr;
phi=phi_hat;
Y=zeros(length(rx_data)-length(train));
for i=1:length(train)
E=B+A;
phi_mes_estimated=angle(rx_data(i)*conj(train(i))); %Attention, for good result this angle should be defined between -pi and pi
phi=A/E*phi_mes_estimated+B/E*phi;                  % measurment update
B=B/E+1;
end
for i=length(train)+1:length(received)
E=B+A;
phi_mes_estimated=(mod(angle(rx_data(i))-phase,pi/2))-pi/4;   % attention, here we assume the QPSK mapping and this formula is not very good, we should change it
phi=A/E*phi_mes_estimated+B/E*phi;
B=B/E+1;
Y(i-length(train))=rx_data(i)*exp(-1j*phi);
end
end