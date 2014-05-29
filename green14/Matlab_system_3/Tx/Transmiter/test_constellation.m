function [ nr_bits_per_symbol ] = test_constellation(type_of_mod)
%UNTITLED5 Summary of this function goes here
%   Detailed explanation goes here

if strcmp(type_of_mod, 'qpsk') == 1
     nr_bits_per_symbol=2;
elseif strcmp(type_of_mod, 'bpsk') == 1
     nr_bits_per_symbol=1;
elseif strcmp(type_of_mod, '16-QAM') == 1
     nr_bits_per_symbol=4;   
elseif strcmp(type_of_mod, '32-QAM') == 1
     nr_bits_per_symbol=5;   
elseif strcmp(type_of_mod, '64-QAM') == 1
     nr_bits_per_symbol=6;
elseif strcmp(type_of_mod, '128-QAM') == 1
     nr_bits_per_symbol=7;     
elseif strcmp(type_of_mod, '64-BEES') == 1
     nr_bits_per_symbol=6;     
elseif strcmp(type_of_mod, '64-WASPS') == 1
     nr_bits_per_symbol=6;     
end