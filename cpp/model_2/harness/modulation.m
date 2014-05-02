function [ modulated_data ] = modulation( input_data, type_of_mod )
%UNTITLED4 Summary of this function goes here
%   Modulation of the data

if strcmp(type_of_mod, 'qpsk') == 1
    modulated_data=qpsk(input_data);
    % disp('the data is mapped qpsk');
elseif strcmp(type_of_mod, 'bpsk') == 1
    modulated_data = bpsk(input_data);
    disp('the data is mapped bpsk');
elseif strcmp (type_of_mod, '32-QAM')==1
    modulated_data=qam32(input_data);
elseif strcmp (type_of_mod, '16-QAM')==1
    modulated_data=QAM(input_data,2);
elseif strcmp (type_of_mod, '64-QAM')==1
    modulated_data=QAM(input_data,3);
elseif strcmp (type_of_mod, '128-QAM')==1
    modulated_data=QAM128(input_data);
elseif strcmp (type_of_mod, '64-BEES')==1
    modulated_data=BEES64(input_data);
elseif strcmp (type_of_mod, '64-WASPS')==1
    modulated_data=WASPS64(input_data);
end

