function [ theta fi] = Synchronize_( Signal, N, L, Q)
%SYNCHRONIZE Use cyclic prefix to find timing and frequency  offset
%
%Input
%SIGNAL - the entire modulated signal
%N - the nr of data symbols per frame
%L - the length of the cyclic prefix, in symbols
%Q - the length of the pulse
%Output
%THETA - element number of the first element in the first full frame
%FI - the frequency offset of the signal

N = N*Q;
L = L*Q;

r = Signal;% (1:2*N+L);
% I1 = [n:n+L-1];
% I2 = [n+N;n+N+L-1];

lambda = zeros(N,1);    % Likelyhood vector

for i = 1:N+L-1;
    % the equation is split up into several steps, for overview
    % the jist of it: go through the signal to identify the start of
    % an OFDM frame by correlating the cyclic prefix with the corresponding
    % data
    
    ro = 0;     % ? to be completed - can be found in the article
    first = sum(r(i:i+L-1).*conj(r(i+N:i+L-1+N)));
    second = ro*sum(abs(r(i:i+L-1)).^2 + abs(r(i+N:i+L-1+N)).^2);
    lambda(i) = 2*abs(first) - second;

    
    
end
plot(lambda)
[~, theta] = max(lambda);    % find the appropriate shift
plot(lambda)

% good but defind modulo 2*pi (inside angle---> create the mistakes we get... must find a solution. 
fi = -1/2/pi*angle(sum(r(theta:theta+L-1).*conj(r(theta+N:theta+L-1+N))))/N;

'a';
% shift = shift-1;          % how much the signal had been shifted
% Signal2 = r(theta:theta+N+L-1);



 

end

