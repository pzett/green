function [modulated_data] = BEES64(input_data)
nr_symbol=length(input_data)/6;
data_r=reshape(input_data,6,nr_symbol);
mat=(2.^(5:-1:0)')*ones(1,nr_symbol);
index=sum(data_r.*mat,1)+1;
load('bees_64');
modulated_data=bees_64(index);
end