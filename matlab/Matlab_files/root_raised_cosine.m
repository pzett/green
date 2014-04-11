function pulse_shape = root_raised_cosine(Q, b, trunc)% h = root_raised_cosine(Q, b, trunc)%% Computes a truncated root raised cosine FIR filter.%% Input:%   Q     = number of samples per symbol%   b     = roll-off factor (optional, default 0.22)%   trunc = truncation, computes impulse response in the interval%           [-trunc*Tsymb , trunc*Tsymb ] (optional, default trunc=5)%% Output:%   pulse_shape     = impulse respone%% History:%   2000-09-03  written /Stefan Parkvall
if nargin < 3  trunc = 5;end
if nargin < 2  b = 0.22;end
% The symbol time T is assumed to be unity.
T = 1*Q;t = -trunc*Q:1:trunc*Q;
pulse_shape = 4 * b * ...            ( cos((1+b)*pi*t/T) + (1-b)*pi/(4*b) * sinc((1-b)*t/T) ) ./ ...            (pi*sqrt(T)*(1-16*b^2*t.^2/T^2));





