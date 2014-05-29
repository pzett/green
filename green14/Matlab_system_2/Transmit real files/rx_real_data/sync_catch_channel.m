function [t_sampl, phi_hat, A_hat,theta_est, sigma_sqr_noise] = sync_catch_channel(data, train, Q,leng_post,leng_pre)
% t_samp = sync(mf, b_train, Q, t_start, t_end)
%
% Determines when to sample the matched filter outputs. The synchronization
% algorithm is based on cross-correlating a replica of the (known)
% transmitted training sequence with the output from the matched filter
% (before sampling). Different starting points in the matched filter output
% are tried and the shift yielding the largest value of the absolute value
% of the cross-correlation is chosen as the sampling instant for the first
% symbol.
% 
%
% Computes the angle of the peak value of the crosscorrelation
% Input:
%   data          = matched filter output, complex
%   train_seq     = the training sequence, complex
%   Q             = number of samples per symbol
%   G             = number of guard bits
% Output:
%   t_samp = sampling instant for first symbol

    pulse=ones(1, Q);
    train_seq=upfirdn(train, pulse, Q, 1);      %upsample original TS
   % t_start=1+Q*G/2;                            %start of the window
   % t_end=t_start+length(train_seq)-1;            %end of the window 

    c = xcorr(data,train_seq); %we use data because we don't know where it starts. The short vector is zero-pad in order to match both dimensions
    c_dash = c(length(data):floor(3/2*length(data)+1));
    c_dash(1:100)=0;
    [A1,t_samp1] = max(abs(c_dash));
    c_dash(t_samp1)=0;
    [A2,t_samp2] = max(abs(c_dash));
    if A1/A2<2
    t_sampl=min(t_samp1,t_samp2);
    else
        t_sampl=t_samp1;
    end
    t_samp=t_sampl;
% Cahnnel estimator using the training sequence.         
    
train_r=data(t_samp:t_samp+length(train_seq)-1);
%train_r=train_r-sum(train_r)/length(train_r);
crosscorr = xcorr(train_r,train_seq);
autocorr=xcorr(train_seq);
% sigmayy=zeros(leng_pre+leng_post+1);
% for i=1:leng_pre+leng_post+1;
% sigmayy(i,:)=autocorr(length(train_seq)-leng_post-i+1:length(train_seq)+leng_pre-i+1);
% end
sigmayy=toeplitz(autocorr(length(train_seq):length(train_seq)+leng_pre+leng_post));
sigmaxy=crosscorr(length(train_seq)-leng_post:length(train_seq)+leng_pre).';
theta_est=sigmaxy*sigmayy^(-1);
train_r_est=conv(theta_est,train_seq);
a=xcorr(train_r-train_r_est(1+leng_post:end-leng_pre));
sigma_sqr_noise=a(length(train_r));
sigma_sqr_noise=1250;
    phi_hat=angle(c(t_samp));
    phi_hat=mod(phi_hat,2*pi);
    A_hat=abs(c(t_samp))/max(abs(autocorr));
    
end