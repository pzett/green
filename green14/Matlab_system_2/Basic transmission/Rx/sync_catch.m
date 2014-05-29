function [t_sampl, phi_hat, A_hat] = sync_catch(data, train, Q)
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
%   
    pulse=ones(1, Q);
    train_seq=upfirdn(train, pulse, Q, 1);      %upsample original TS
   % t_start=1+Q*G/2;                            %start of the window
   % t_end=t_start+length(train_seq)-1;            %end of the window 

    c = xcorr(data,train_seq); %we use data because we don't know where it starts. The short vector is zero-pad in order to match both dimensions
    c_dash = c(length(data):floor(3/2*length(data)+1));
    [~,t_samp] = max(abs(c_dash));
    t_sampl=t_samp;
    
% Phase estimator using the training sequence.         
    
    phi_hat=angle(c(t_samp));
    phi_hat=mod(phi_hat,2*pi);
    A_hat=abs(c(t_samp));
    
end