function [ data_modulated ] = ASK(input_data,nr_bits_axe)
reshaped_data=reshape(input_data,nr_bits_axe,length(input_data)/(nr_bits_axe));
data_modulated=zeros(1,length(input_data)/(nr_bits_axe));
for i=1:nr_bits_axe
    data_modulated=data_modulated+2^(nr_bits_axe-i).*(-1).^sum(reshaped_data(1:i,:),1);
end
end