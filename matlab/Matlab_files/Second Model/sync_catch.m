function [t_sampl, phi_hat] = sync_catch(data,train)
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
%   data            matched filter output, complex
%   train           training sequence, complex
%  
%  % Output:
%   t_samp          sampling instant for first symbol
%   phi_hat         phase offset estimate
%   
   
    train_seq=train;
    mf_train=abs(conj(train_seq(end:-1:1)))-sum(abs(train_seq))/length(train_seq);     % because we don't know the frequency offset and we use absolute value of TS  , 
                                                                                       % Frederic  thinks that it is good to have zero mean MF that is why we devide by length of TS   
    c =filter(mf_train,1,abs(data));
    plot(abs(c))
    c_dash=c(1:length(c)/2);
    [~,t_samp] = max(c_dash);
    t_sampl=t_samp-length(mf_train);
    
% Phase estimator using the training sequence.         
    
    phi_hat=angle(c(t_samp));
   % phi_hat=mod(phi_hat,2*pi);