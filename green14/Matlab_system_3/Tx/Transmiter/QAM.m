function [ data_modulated ] = QAM(input_data,nr_bits_axe)
reshaped_data=reshape(input_data,2*nr_bits_axe,length(input_data)/(2*nr_bits_axe));
data_modulated=zeros(1,length(input_data)/(2*nr_bits_axe));
for i=1:nr_bits_axe
    data_modulated=data_modulated+2^(nr_bits_axe-i).*(-1).^sum(reshaped_data(1:i,:),1);
    data_modulated=data_modulated+2^(nr_bits_axe-i)*1j*(-1).^sum(reshaped_data(nr_bits_axe+1:nr_bits_axe+i,:),1);
end
end



%16-QAM
%    1000 1100   0100 0000
%     x    x   |+3 x    x
%              |
%    1001 1101 | 0101 0001
%     x    x   |+1 x    x
%              |  
%  ---3-----1--+--+1----+3-------
%    1011 1111 | 0111 0011
%     x    x   |-1 x    x
%              |
%    1010 1110 | 0110 0010
%     x    x   |-3 x    x
%
