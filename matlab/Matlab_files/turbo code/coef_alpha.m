function [coef_alpha_alpha coef_alpha_gamma coef_m_beta] = coef_alpha(number_input,number_states,M_generatrice,memory)
%UNTITLED3 Summary of this function goes here
%   Detailed explanation goes here

%[number_output Mem]=size(M_generator);
%number_output=number_output/number_input;
%M=zeros(number_input, Mem);
%vect=(1:size(M,2));
%for j=1:number_input
%    for i=1:number_output
%        M(j,:)=sign(M(j,:)+M_generator((i-1)*number_input+j,:));
%    end
%    M(j,:)=M(j,:).*vect;
%end
%Memory_input=max(M,[],2)-1; % vector giving the number of memory in function the input index.
%Memory=sum(Memory_input);
%number_states=2^Memory;


coef_alpha_gamma=zeros(number_states,number_input);
coef_alpha_alpha=zeros(number_states,number_input);

%coef_alpha_alpha=Table(:,1);
%coef_alpha_gamma=Table(:,1);

% for i=1:number_states
%     num=dec2base(i-1, 2) - '0';
%     new_codeword=zeros(1,Memory);
%     new_codeword(Memory-length(num)+1:Memory)=num;
%     old_codeword=zeros(1,Memory);
%     transition=zeros(1,Memory+number_input);
%     
%     indice_courrant=1;
%     for k=1:number_input
%         transition(k)=new_codeword(indice_courrant);
%         old_codeword(indice_courrant:indice_courrant+Memory_input(k)-2)=new_codeword(indice_courrant+1:indice_courrant+Memory_input(k)-1);
%         indice_courrant=indice_courrant+Memory_input(k);
%     end
%     for k=1:2^number_input
%         indice_courrant=0;
%         
%         
%         num=zeros(1,number_input);
%         p=dec2base(k-1, 2) - '0';
%         num(number_input-length(p)+1:number_input)=p;
%         
%         for l=1:number_input
%             
%             indice_courrant=indice_courrant+Memory_input(l);
%             old_codeword(indice_courrant)=num(l);
%         end
%         
%         transition(1,number_input+1:length(transition))=old_codeword(1,:);
%         
%         
%         
%         coef_alpha_alpha(i,k)=sum((2*ones(1,length(old_codeword))).^(length(old_codeword)-1:-1:0).*old_codeword)+1;
%         coef_alpha_gamma(i,k)=sum((2*ones(1,length(transition))).^(length(transition)-1:-1:0).*transition)+1;
%     end
%     
% end

% for k=1:2^number_input
%     A(:,(k-1)*number_states+1:k*number_states)=Table(:,[1 k+1])';
%     B(:,(k-1)*number_states+1:k*number_states)=(k-1)*number_states+Table(:,[1 k+1])';
% end
% 
% A=sortrows(A',2)';
% B=sortrows(B',2)';
% coef_alpha_alpha=reshape(A(1,:)',2^number_input,number_states)';
% coef_alpha_gamma=reshape(B(1,:)',2^number_input,number_states)';
M1=M_generatrice(1:memory,:);
for k=1:number_states
        num=zeros(1,memory);
        p=dec2base(k-1, 2) - '0';
        num(memory-length(p)+1:memory)=p;
    %    M1
    %    M1*[0 num]'
    %    M1*[1 num]'
o0=mod((M1*[0 num]')',2);
o1=mod((M1*[1 num]')',2);
%size(sum((2*ones(1,length(o0))).^(length(o0)-1:-1:0)))
%size(o0)
a0=sum((2*ones(1,length(o0))).^(length(o0)-1:-1:0).*o0)+1;
a1=sum((2*ones(1,length(o1))).^(length(o1)-1:-1:0).*o1)+1;
coef_alpha_alpha(a0,1+sign(coef_alpha_alpha(a0)))=k;
coef_alpha_alpha(a1,1+sign(coef_alpha_alpha(a1)))=k;
coef_alpha_gamma(a0,1+sign(coef_alpha_gamma(a0)))=k;
coef_alpha_gamma(a1,1+sign(coef_alpha_gamma(a1)))=number_states+k;
coef_m_beta(k)=a0;
coef_m_beta(number_states+k)=a1;
end
end

