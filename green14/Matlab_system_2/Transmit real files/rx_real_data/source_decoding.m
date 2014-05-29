% read the file and decode it
close all
clear all

load('rx_info_dec')

data_coded=reshape(rx_info.',1,nr_blocs*(100000-2*4));
data=data_coded;
pointer=1;
L_data_coded=sum(data(pointer:pointer+24-1).*2.^(24-1:-1:0));
pointer=pointer+24;
nr_bits_bloc=sum(data(pointer:pointer+4-1).*2.^(4-1:-1:0));
pointer=pointer+4;
for i=1:2^nr_bits_bloc
index(i)=sum(data(pointer:pointer+nr_bits_bloc-1).*2.^(nr_bits_bloc-1:-1:0));
pointer=pointer+nr_bits_bloc;
code(i,(1:index(i)))=data(pointer:pointer+index(i)-1);
pointer=pointer+index(i);
end

j=0;
while pointer<L_data_coded+1
    j=j+1;
   i=1;
   while pointer+index(i)-1>length(data_coded)||sum(data_coded(pointer:pointer+index(i)-1)~=code(i,1:index(i)))
   i=i+1;
   end
   if pointer+index(i)-1>length(data_coded)
       for k=i+1:2^nr_bits_bloc;
           if pointer+index(i)-1<=length(data_coded)&&(~sum(data_coded(pointer:pointer+index(i)-1)~=code(i,1:index(i))))
               i=k;
               data_decoded_r(:,j)=(dec2bin(i-1,nr_bits_bloc)-'0').';
               pointer=pointer+index(i);
              break
           end
       end
   end
   data_decoded_r(:,j)=(dec2bin(i-1,nr_bits_bloc)-'0');
   pointer=pointer+index(i);
end
data_decoded=reshape(data_decoded_r,1,j*nr_bits_bloc);
save('data_decoded','data_decoded')