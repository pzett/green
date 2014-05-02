clear all;
close all;

F=[1 6.28318; 0 1];
G=[1 0 ; 0 0 ];
H=[1 0];
R1=[0.001 0 ;0 0];

x0=[1 ;0.1];
Q0=[3 0 ;0 9.8696];
backward=0;
R2=0.1;

x(:,1)=x0;
%% Generating data
for i=1:100000
    x(:,i)=mod(x(:,i)+pi,2*pi)-pi;
    x(:,i+1)=F*x(:,i);%+[0.001*randn(1) ;0];
    yp(i)=H*x(:,i);
end

y=yp;%+0.1*randn(1,100000);

fid=fopen('data_to_harness.dat','w');
fwrite(fid,y,'double');
fclose(fid);


for i=1:100000
    yc=y(i);
    [yhat,xhatfilt,xhatpred,P,Q] =kalman_phase(yc,F,G,H,R1,R2,x0,Q0,backward);
    %x0=[mod(xhatfilt(1)+pi, 2*pi)-pi; xhatfilt(2)];
    x0=xhatfilt;
   
    Q0=Q;
    resultM(i)=yhat;
end

%system('./a.out')

fid1=fopen('data_from_harness.dat','r');
resultC=fread(fid1,100000,'double');
fclose(fid1);


plot(abs(resultC-resultM'), 'r'); hold on;
figure;
plot(mod(resultM-yp+pi,2*pi)-pi)
