function [ ts ] = golay_generator( len )
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here
%m=length of golay training sequence
% G_complex=zeros(1,length(len));
% maximum=zeros(3, 20000);
if mod(len,2)==0
m=len/2;
else m=(len-1)/2;
end

A=(rand(1, m)<0.5);
B=(rand(1, m)<0.5);
A=upsample(A, 2);
B=upsample(B, 2);
B1=zeros(1,(length(B)+1));

for l=1:length(B)
    B1(l+1)=B(l);
end
for l=1:2*m
G(l)=xor(A(l),B1(l));
end
G=G.*2-1; 

for l=1:length(G)
if G(l)==-1 
   G_complex(l)=-1-1i;
else G_complex(l)=1+1i;
end
end

% c=xcorr(G);
% c_complex=xcorr(G_complex);
% [~, M]=max(c);
% [~, M_c]=max(c_complex);



ts=conj(G_complex');
end

