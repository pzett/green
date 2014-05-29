function [ pilot ] = generate_pilot(length_pilot, type_of_pilot, pilot_pattern,modulation,nr_bits_symbols)
%generate_pilot generates pilot sequence
%   Detailed explanation goes here
if strcmp(type_of_pilot, 'random')
    pilot=3/sqrt(2)*((2*rand(length(pilot_pattern),length_pilot)-1)+(2*rand(length(pilot_pattern),length_pilot)-1)*1i);            % to get the mean eqal to the mean of the transmitted symbols
end
if strcmp(type_of_pilot, 'random_phase')
   
    pilot = exp(1i*2*pi*rand(length(pilot_pattern),length_pilot));
    
end

if strcmp (type_of_pilot, 'modulated') && strcmp(modulation, 'bpsk')
    
    pilot=bpsk(rand(1, length_pilot*nr_bits_symbols*length(pilot_pattern))<0.5);      % bpsk and qpsk works only with vectors => reshape the data
    pilot=reshape(pilot,length(pilot_pattern),length_pilot);
elseif strcmp (type_of_pilot, 'modulated') && strcmp(modulation, 'qpsk')
    pilot=qpsk(rand(1, length_pilot*nr_bits_symbols*length(pilot_pattern))<0.5);
     pilot=reshape(pilot,length(pilot_pattern),length_pilot);
end

