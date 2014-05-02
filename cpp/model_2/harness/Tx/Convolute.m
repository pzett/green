function [y] = Convolute(u, M_generatrice,Memoire,number_output,number_input)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
%number_input=size(u,1);
%[number_output Mem]=size(M_generatrice);
%number_output=number_output/number_input;
%u_temp=zeros(size(u,1),size(u,2)+size(M_generatrice,2)-1);
%y_temp=zeros(number_output,size(u,2)+size(M_generatrice,2)-1);
%u_temp(:,1:size(u,2))=u;
y=zeros(1,size(u,2)*number_output/number_input);

M1=M_generatrice(1:Memoire,:);
M2=M_generatrice(Memoire+1:Memoire+number_output,:);
Memoire_courrante=zeros(Memoire,1);
%output=zeros(number_output,1);

for i=1:length(u)
output=mod(M2*[u(:,i);Memoire_courrante],2);
Memoire_courrante=mod(M1*[u(:,i);Memoire_courrante],2);
y((i-1)*number_output+1:i*number_output)=output;
end
end