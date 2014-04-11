function [yhat,xhatfilt,xhatpred,P,Q] = kalman(y,F,G,H,R1,R2,x0,Q0)
K=size(y,1);
xhatpred=zeros(size(x0,1),K);
yhat=zeros(size(R2,1),K);
xhatfilt=zeros(size(x0,1),K);
xhatpred(:,1)=F*x0;
yhat(:,1)=H*xhatpred(:,1);
P=F*Q0*F'+G*R1*G';
L=P*H'*(H*P*H'+R2)^(-1);
xhatfilt(:,1)=xhatpred(:,1)+L*(y(1,:)'-yhat(:,1));
Q=P-L*H*P;
for i=2:length(y)
xhatpred(:,i)=F*xhatfilt(:,i-1);
yhat(:,i)=H*xhatpred(:,i);
P=F*Q*F'+G*R1*G';
L=P*H'*(H*P*H'+R2)^(-1);
xhatfilt(:,i)=xhatpred(:,i)+L*(y(i,:)'-yhat(:,i));
Q=P-L*H*P;
end
end