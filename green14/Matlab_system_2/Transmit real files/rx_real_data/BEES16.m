% Map bit with 16-BEES constellation
function [modulated_data] = MOD16(input_data)
nr_symbol=length(input_data)/4;
map1=[11 10 12 2 13 1 15 5 14 6 3 4 9 16 7 8];
data_r=reshape(input_data,4,nr_symbol);
mat=(2.^(3:-1:0)')*ones(1,nr_symbol);
index=sum(data_r.*mat,1)+1;
load('BEES16');
mod_16_1=mod_16_1(map1);
modulated_data=mod_16_1(index);
end
