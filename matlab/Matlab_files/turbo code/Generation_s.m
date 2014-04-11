function [s gamma_pre] = Generation_s (leng_u,number_input,M_generator, p1, Poncture,number_output,Memory)
%UNTITLED4 Summary of this function goes here
%   Detailed explanation goes here
% M=zeros(number_input, Memory);
% vect=(1:size(M,2));
% for j=1:number_input
%     for i=1:number_output
%         M(j,:)=sign(M(j,:)+M_generator((i-1)*number_input+j,:));
%     end
%     M(j,:)=M(j,:).*vect;
% end
number_states=2^Memory;
%leng_u=L_y/number_output;
number_possible_transitions=(2^number_input)*number_states;
M2=M_generator(Memory+1:Memory+number_output,:);


s=zeros(number_possible_transitions,number_output);
for i=1:number_possible_transitions
    temp=zeros(1,Memory+number_input);
    t=dec2base(i-1, 2) - '0';
    temp(Memory+number_input-length(t)+1:Memory+number_input)=t;
    %size(M2)
    %size(temp)
    %temp'
    s(i,:)=mod(M2*temp',2);
end

gamma_pre=zeros(number_possible_transitions,leng_u);
% for i=1:number_possible_transitions
%     for j=1:leng_u
%         coef_courrant=number_possible_transitions/2;
%         for k=1:number_input
%             gamma_pre(i,j)=gamma_pre(i,j)+log((p1(k)*(sign(mod(i-1,2*coef_courrant)+1-coef_courrant-0.5)))+(1-sign(mod(i-1,2*coef_courrant)+1-coef_courrant-0.5))/2); %log of the probability p1 if the input is 1, 1-p1 if the input is 0
%             coef_courrant=coef_courrant/2;
%         end
%     end
% end

end

