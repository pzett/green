function [coef_m_alpha coef_m_beta] = coef_m(coef_beta_gamma,coef_alpha_gamma,number_input,number_state)
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here


for k=1:2^number_input
coef_m_alpha(coef_beta_gamma(:,k))=(1:number_state);
end
for k=1:2^number_input
coef_m_beta(coef_alpha_gamma(:,k))=(1:number_state);
end
coef_m_alpha=coef_m_alpha';
coef_m_beta=coef_m_beta';

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
% number_possible_transitions=(2^number_input)*number_states;
% coef_m_alpha=zeros(number_states,number_input);
% coef_m_beta=zeros(number_states,number_input);
% 
% 
% 
% 
% 
% for i=1:number_possible_transitions
%     num=dec2base(i-1, 2) - '0';
%     transition=zeros(1,number_input+Memory);
%     transition(number_input+Memory-length(num)+1:number_input+Memory)=num;
%     new_codeword=zeros(1,Memory);
%     indice_courrant=1;
%     old_codeword=transition(number_input+1:length(transition));
%     for k=1:number_input
%         new_codeword(indice_courrant)=transition(k);
%         new_codeword(indice_courrant+1:indice_courrant+Memory_input(k)-1)=old_codeword(indice_courrant:indice_courrant+Memory_input(k)-2);
%         indice_courrant=indice_courrant+Memory_input(k);
%     end
%     coef_m_beta(i)=sum((2*ones(1,length(new_codeword))).^(length(new_codeword)-1:-1:0).*new_codeword)+1;
%     coef_m_alpha(i)=sum((2*ones(1,length(old_codeword))).^(length(old_codeword)-1:-1:0).*old_codeword)+1;
% end
end



