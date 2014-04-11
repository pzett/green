function y = multipath(x, tau, h)
% y = multipath(x, tau, h)
%
% A simple multipath channel with two paths. The amplification of respective
% path is sqrt(3/4) and sqrt(1/4) (unless h is given as an input
% argument). No noise is added, but the AWGN channel can be put afterwards
% to take care of this.
%
% Input:
%   x   = the transmitted signal
%   tau = the delay between the first and second path measured in samples
%   h   = vector of tap coefficients (optional)
%
% Output:
%   y   = the sum of the two paths

% 2000-08-22  written /Stefan

if nargin < 3
  h = [sqrt(3/4) ; sqrt(1/4)];
end

y = h(1)*x + h(2)*[zeros(1, tau) x(1:end-tau)];
