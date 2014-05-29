% map bits following a QAM mapping with independant axis with gray coding
function [ data_modulated ] = QAM(input_data,nr_bits_axe)
reshaped_data=reshape(input_data,2*nr_bits_axe,length(input_data)/(2*nr_bits_axe));
data_modulated=zeros(1,length(input_data)/(2*nr_bits_axe));
for i=1:nr_bits_axe
    data_modulated=data_modulated+2^(nr_bits_axe-i).*(-1).^sum(reshaped_data(1:i,:),1);
    data_modulated=data_modulated+2^(nr_bits_axe-i)*1j*(-1).^sum(reshaped_data(nr_bits_axe+1:nr_bits_axe+i,:),1);
end
end

