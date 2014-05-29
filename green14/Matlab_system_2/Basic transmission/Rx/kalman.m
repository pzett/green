% Kalman filtering
function [yhat,xhatfilt,xhatpred,P,Q] = kalman(y,F,G,H,R1,R2,x0,Q0,backward)
K=size(y,1);
Q=zeros(size(Q0,1),size(Q0,2),length(y));
P=zeros(size(Q0,1),size(Q0,2),length(y));
xhatpred=zeros(size(x0,1),K);
yhat=zeros(size(R2,1),K);
xhatfilt=zeros(size(x0,1),K);
xhatpred(:,1)=F*x0;
yhat(:,1)=H*xhatpred(:,1);
P(:,:,1)=F*Q0*F'+G*R1*G';
L=P(:,:,1)*H'*(H*P(:,:,1)*H'+R2)^(-1);
xhatfilt(:,1)=xhatpred(:,1)+L*(y(1,:)'-yhat(:,1));
Q(:,:,1)=P(:,:,1)-L*H*P(:,:,1);
for i=2:length(y)
xhatpred(:,i)=F*xhatfilt(:,i-1);
yhat(:,i)=H*xhatpred(:,i);
P(:,:,i)=F*Q(:,:,i-1)*F'+G*R1*G';
L=P(:,:,i)*H'*(H*P(:,:,i)*H'+R2)^(-1);
xhatfilt(:,i)=xhatpred(:,i)+L*(y(i,:)'-yhat(:,i));
Q(:,:,i)=P(:,:,i)-L*H*P(:,:,i);
end
if 0
    for i=length(y)-1:-1:1
        xhatfilt(:,i)=xhatfilt(:,i)+Q(:,:,i)*F'*P(:,:,i)^(-1)*(xhatfilt(:,i)-xhatpred(:,i));       
    end   
end
Q=Q(:,:,end);
end