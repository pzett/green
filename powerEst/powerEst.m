function [ P ] = powerEst( signal )
%powerEst Estimate the power of the received signal (baseband) in different 
%frequency bands
%   @input:
%       - signal: the signal to be analysed
%   @output:
%       - P: 1X25 vectors with the power corresponding to the center 
%               frequencies -12, -11, -10, ..., 12

N_window=25;
%fs=1/0.05;
fs=25e6;
% Freqs=[-12:1:12]*1e6;
% range=fs/N_window;
% range_2=1/N_window;

signal=signal-mean(signal);

P=zeros(1,N_window);

[Pxx, w] = periodogram(signal, hamming(length(signal)));

%plot(Pxx);

N=length(Pxx);
nb_points=floor(N/N_window);

for i=1:N_window
    P(i)=sum(Pxx((i-1)*nb_points+1:i*nb_points).*fs/N);
end

end

