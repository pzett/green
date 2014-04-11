function t_samp = sync(mf, b_train, Q, t_start, t_end)
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
% Input:
%   mf            = matched filter output, complex baseband, I+jQ
%   b_train       = the training sequence bits
%   Q             = number of samples per symbol
%   t_start       = start of search window
%   t_end         = end of search window
%
% Output:
%   t_samp = sampling instant for first symbol

    s_train = qpsk(b_train);
    delta_func=[1];
    s_train = upfirdn(s_train, delta_func, Q, 1);
    %length(samp_train)
    %mf_train = mf(t_start:t_end);
    mf_train = mf;
    length(mf_train);
    c = xcorr(mf_train,s_train);
    c_dash = c(length(mf_train)+t_start-1:length(mf_train)+t_end-1);
    [max_val t_samp] = max(abs(c_dash));
    t_samp=t_samp+t_start-1;
    %[max_val t_samp] = max(abs(c))
    %stem(1:length(c_dash),abs(c_dash));
    %hold on;