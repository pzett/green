function b = random_data(nr_data_bits)
% b = random_data(nr_data_bits)
%
% Generates a block of equiprobable random binary data, {0, 1}
%
% Input:
%   nr_data_bits = the number of bits to generate
%
% Output:
%   b = random data bits, {0, 1}

% History:
%   2000-06-26  written /Stefan Parkvall
%   2001-10-21  modified /George Jöngren

b = (rand(1, nr_data_bits) > .5);
