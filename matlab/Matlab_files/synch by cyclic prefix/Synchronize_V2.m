function [theta freq phase] = Synchronize_V2( Signal, training, Q)
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

N=(length(training))*Q;
L=11;
training=[training(end-10:end) training];
%training=filter(ones(Q,1),1,[training training]);
r = Signal;% (1:2*N+L);
% I1 = [n:n+L-1];
% I2 = [n+N;n+N+L-1];

lambda = zeros(N,1);    % Likelyhood vector

for i = 1:length(Signal)/2%-L-N-1;
    % the equation is split up into several steps, for overview
    % the jist of it: go through the signal to identify the start of
    % an OFDM frame by correlating the cyclic prefix with the corresponding
    % data
    
    ro = 0;     % ? to be completed - can be found in the article
    first = sum(r(i:i+L-1).*conj(r(i+N:i+L-1+N)));
    second = ro*sum(abs(r(i:i+L-1)).^2 + abs(r(i+N:i+L-1+N)).^2);
    lambda(i) = 2*abs(first) - second;

    
    
end

[~, theta] = max(lambda);    % find the appropriate shift
plot(lambda)

% good but defind modulo 2*pi (inside angle---> create the mistakes we get... must find a solution. 
fi_mod = -1/2/pi*angle(sum(r(theta:theta+L-1).*conj(r(theta+N:theta+L-1+N))))/N;


Received_train=Signal(theta:Q:theta+2*N);
%training=training(Q:Q:end);
%Kalman filtering to get phi and f_offset  (but not modulo 2*pi)
y=0;
Q0=[pi^2 0;0 1];
phi_hat=0;
freq=0;
x=[phi_hat;freq];
phase=0;
for i=1:length(training)
   % angle(Received_train(i)*conj(train(i)))-x(1)
   % mod(angle(Received_train(i)*conj(train(i)))+pi-x(1),2*pi)-pi
   % x(2)*2*pi
  % mod(angle(Received_train(mod(i-1,N)+1)*conj(training(i)))+pi-x(1),2*pi)-pi
   [~,x,~,~,Q0]=kalman(mod(angle(Received_train(i)*conj(training(i)'))+pi-x(1),2*pi)-pi,[0 2*pi;0 1],1,[1 0],[0.01 0; 0 0],0.004,x,Q0);
%x(1)
%x(2)
x(1)=x(1)+phase;
phase=x(1);
   %Received(i)=Received(i)*exp(-j*x(1));
%x(1)=mod(x(1)+pi,2*pi)-pi;
end
phase
freq=(x(2)-(mod(x(2)+pi,2*pi)-pi))/4+mod(fi_mod,2*pi/N);








% shift = shift-1;          % how much the signal had been shifted
% Signal2 = r(theta:theta+N+L-1);



 

end

