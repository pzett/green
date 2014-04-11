function [coef_beta_beta coef_beta_gamma coef_m_alpha] = coef_beta(number_input,number_states,M_generatrice,memory)
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here



%for k=1:2^number_input
%    A(:,(k-1)*number_states+1:k*number_states)=[Table(:,[1 k+1])';(k-1)*number_states+Table(:,1)'];
  %  B(1,(k-1)*number_states+1:k*number_states)=(k-1)*number_states+Table(:,1)';
  %  B(2,(k-1)*number_states+1:k*number_states)=(k-1)*number_states+Table(:,k+1)';
%end

%A=sortrows(A',1)';
%B=sortrows(B',1)';
%coef_beta_beta=reshape(A(2,:)',2^number_input,number_states)';
%coef_beta_gamma=reshape(B(2,:)',2^number_input,number_states)';
%coef_beta_gamma=reshape(A(3,:)',2^number_input,number_states)';

% [number_output Mem]=size(M_generator);
% number_output=number_output/number_input;
% M=zeros(number_input, Mem);
% vect=(1:size(M,2));
% for j=1:number_input
%     for i=1:number_output
%         M(j,:)=sign(M(j,:)+M_generator((i-1)*number_input+j,:));
%     end
%     M(j,:)=M(j,:).*vect;
% end
% Memory_input=max(M,[],2)-1; % vector giving the number of memory in function the input index.
% Memory=sum(Memory_input);
% number_states=2^Memory;
% coef_beta_gamma=zeros(number_states,number_input);
% coef_beta_beta=zeros(number_states,number_input);
% 
% for i=1:number_states
%     num=dec2base(i-1, 2) - '0';
%     old_codeword=zeros(1,Memory);
%     old_codeword(Memory-length(num)+1:Memory)=num;
%     new_codeword=zeros(1,Memory);
%     transition=zeros(1,Memory+number_input);
%     
%     indice_courrant=1;
%     transition(1,number_input+1:length(transition))=old_codeword(1,:);
%     
%     for k=1:number_input
%         new_codeword(1,indice_courrant+1:indice_courrant+Memory_input(k)-1)=old_codeword(1,indice_courrant:indice_courrant+Memory_input(k)-2);
%         indice_courrant=indice_courrant+Memory_input(k)+1;
%     end
%     
%     for k=1:2^number_input
%         indice_courrant=1;
%         
%         num=zeros(1,number_input);
%         p=dec2base(k-1, 2) - '0';
%         num(number_input-length(p)+1:number_input)=p;
%         
%         for l=1:number_input
%             new_codeword(indice_courrant)=num(l);
%             indice_courrant=indice_courrant+Memory_input(l);
%         end
%         transition(1:number_input)=num;
%         coef_beta_beta(i,k)=sum((2*ones(1,length(new_codeword))).^(length(new_codeword)-1:-1:0).*new_codeword)+1;
%         coef_beta_gamma(i,k)=sum((2*ones(1,length(transition))).^(length(transition)-1:-1:0).*transition)+1;
%     end
% end

M1=M_generatrice(1:memory,:);
for k=1:number_states
        num=zeros(1,memory);
        p=dec2base(k-1, 2) - '0';
        num(memory-length(p)+1:memory)=p;
o0=mod((M1*[0 num]')',2);
o1=mod((M1*[1 num]')',2);
%size(sum((2*ones(1,length(o0))).^(length(o0)-1:-1:0)))
%size(o0)
coef_beta_beta(k,1)=sum((2*ones(1,length(o0))).^(length(o0)-1:-1:0).*o0)+1;
coef_beta_beta(k,2)=sum((2*ones(1,length(o1))).^(length(o1)-1:-1:0).*o1)+1;
coef_beta_gamma(k,1)=k;
coef_beta_gamma(k,2)=number_states+k;
%coef_m_beta(sum((2*ones(1,length(o0))).^(length(o0)-1:-1:0).*o0)+1)=sum((2*ones(1,length(o0))).^(length(o0)-1:-1:0).*o0)+1;
%coef_m_beta(number_states+sum((2*ones(1,length(o1))).^(length(o1)-1:-1:0).*o1)+1)=sum((2*ones(1,length(o1))).^(length(o1)-1:-1:0).*o1)+1;
%coef_m_beta=[(1:number_states) (1:number_states)];
coef_m_alpha(k)=k;
coef_m_alpha(number_states+k)=k;
end





end

