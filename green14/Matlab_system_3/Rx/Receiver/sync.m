function [t_samp]=sync(rx_data, train, Q,mode)
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
%   data          = the recieved data, complex
%   train_seq     = the training sequence, complex
%   Q             = number of samples per symbol
% Output:
%   t_samp = sampling instant for first symbol of the training sequence

    pulse=ones(1, Q);
    train_seq=upfirdn(train, pulse, Q, 1);      %upsample original TS
   % t_start=1+Q*G/2;                            %start of the window
   % t_end=t_start+length(train_seq)-1;            %end of the window 

    c = xcorr(rx_data,train_seq); %we use data because we don't know where it starts. The short vector is zero-pad in order to match both dimensions
    c_dash = c(length(rx_data):floor(3/2*length(rx_data)+1));
    if mode
    c_dash(1:100)=0;
    end
%      figure(100); plot(abs(c_dash));
    [~,t_samp] = max(abs(c_dash));
    
end
