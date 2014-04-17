%close all;
fid1=fopen('train_norm.dat','r');
x=fread(fid1,100*2,'double');
fclose(fid1);

tComplex= zeros(1,100);
count = 1;
for i=1:2:100*2
    tComplex(count) = complex(x(i),x(i+1));
    count = count+1;
end



fid1=fopen('sent.dat','r');
x=fread(fid1,55680,'short');
fclose(fid1);

xComplex= zeros(1,55680/2);
count = 1;
for i=1:2:55680
    xComplex(count) = complex(x(i),x(i+1));
    count = count+1;
end

figure(1)
plot(real(xComplex))

a=rms(xComplex)

figure(2)
plot(abs(xcorr(tComplex)))

figure(3)
Q=4;
pulse=ones(1,4);
train_seq=upfirdn(tComplex, pulse, Q, 1);      %upsample original TS
    
    c = xcorr((xComplex),train_seq);
    c_dash = c(length(xComplex):floor(3/2*length(xComplex)+1));
    plot(abs(c_dash))
    [~,t_samp] = max(abs(c_dash));
    t_sampl=t_samp;
    
% Phase estimator using the training sequence.         
    
    phi_hat=angle(c(t_samp));
    phi_hat=mod(phi_hat,2*pi);
