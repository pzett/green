function [modulated_data] = WASPS64(input_data)
nr_symbol=length(input_data)/6;
data_r=reshape(input_data,6,nr_symbol);
mat=(2.^(5:-1:0)')*ones(1,nr_symbol);
index=sum(data_r.*mat,1)+1;
load('wasps-64');
wasp=conj(wasp)';
modulated_data=wasp(index);
end

