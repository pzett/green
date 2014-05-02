function [data] = generate_data_from_picture(name)
%UNTITLED9 Summary of this function goes here
%   Detailed explanation goes here
data=zeros(1,3*16+8*size(name,1)*size(name,2)*size(name,3));
data(1:16)=dec2bin(size(name,1),16)-'0';
data(17:32)=dec2bin(size(name,2),16)-'0';
data(33:48)=dec2bin(size(name,3),16)-'0';
pointer=48;
for i=1:size(name,2)
    for j=1:size(name,3)
        a=dec2bin(name(:,i,j),8)-'0';
        data(pointer+1:pointer+8*size(name,1))=reshape(a,1,8*size(name,1));
        pointer=pointer+8*size(name,1);
    end
end