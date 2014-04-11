function bhat = detect(r)
% bhat = detect(r)
%
% Computes the received bits given a received sequence of (phase-corrected)
% QPSK symbols. Gray coding of the individual bits is assumed. Hence, the
% two bits for each symbol can be detected from the real and imaginary
% parts, respectively. The first of the two bits below is output first in
% the bhat-sequence.
%
% Assumed mapping:
%
%  10 x   |   x 00
%         |
%  -------+-------
%         |
%  11 x   |   x 01
%
% Input:
%   r  = sequence of complex-valued QPSK symbols
%
% Output:
%   bhat  = bits {0,1} corresponding to the QPSK symbols

bhat = zeros(1,length(r)*2);
k=1;
for i=1:1:length(r)
    rpart = sign(real(r(i)));
    if(rpart == +1)
        bhat(k) = 0;
    else
        bhat(k) = 1;
    end
    k=k+1;
    ipart = sign(imag(r(i)));   
    if(ipart == +1)
        bhat(k) = 0;
    else
        bhat(k) = 1;
    end
    k=k+1;
end

