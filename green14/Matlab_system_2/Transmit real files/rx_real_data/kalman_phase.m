% apply the kalman filter assuming modulo 2*pi and the state space model
% defined for the phase
function [yhat,xhatfilt,xhatpred,P,Q] = kalman_phase(y,F,G,H,R1,R2,phase,freq_off,Q0,backward)
x0=[phase;freq_off];
K=size(y,1);
xhatpred=zeros(size(x0,1),K);
Q=zeros(size(Q0,1),size(Q0,2),length(y));
P=zeros(size(Q0,1),size(Q0,2),length(y));
yhat=zeros(size(R2,1),K);
xhatfilt=zeros(size(x0,1),K);
xhatpred(:,1)=F*x0;
yhat(:,1)=H*xhatpred(:,1);
P(:,:,1)=F*Q0*F'+G*R1*G';
L=P(:,:,1)*H'*(H*P(:,:,1)*H'+R2)^(-1);
xhatfilt(:,1)=xhatpred(:,1)+L*(mod(conj(y(1,:)')-yhat(:,1)+pi,2*pi)-pi);
Q(:,:,1)=P(:,:,1)-L*H*P(:,:,1);
for i=2:length(y)
    xhatpred(:,i)=F*xhatfilt(:,i-1);
    yhat(:,i)=H*xhatpred(:,i);
    P(:,:,i)=F*Q(:,:,i-1)*F'+G*R1*G';
    L=P(:,:,i)*H'*(H*P(:,:,i)*H'+R2)^(-1);
    xhatfilt(:,i)=xhatpred(:,i)+L*(mod(conj(y(i,:)')-yhat(:,i)+pi,2*pi)-pi);
    Q(:,:,i)=P(:,:,i)-L*H*P(:,:,i);
end
if backward
    for i=length(y)-1:-1:1
        a=[(mod(xhatfilt(1,i)-xhatpred(1,i)+pi,2*pi)-pi);(xhatfilt(2,i)-xhatpred(2,i))];
        xhatfilt(:,i)=xhatfilt(:,i)+Q(:,:,i)*F'*P(:,:,i)^(-1)*a;  
    end   
end
Q=Q(:,:,end);
end