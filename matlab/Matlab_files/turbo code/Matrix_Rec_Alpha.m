function [M_rec_alpha_alpha M_rec_gamma_alpha] = Matrix_Rec_Alpha(M_generator ,number_input)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here




[number_output Mem]=size(M_generator);
number_output=number_output/number_input;
M=zeros(number_input, Mem);
vect=(1:size(M,2));
for j=1:number_input
    for i=1:number_output
        M(j,:)=sign(M(j,:)+M_generator((i-1)*number_input+j,:));
    end
    M(j,:)=M(j,:).*vect;
end
Memory_input=max(M,[],2)-1; % vector giving the number of memory in function the input index.
Memory=sum(Memory_input);
number_states=2^Memory;
number_possible_transitions=(2^number_input)*number_states;







M_rec_gamma_alpha=zeros(number_states,number_possible_transitions,number_states);
M_rec_alpha_alpha=zeros(number_states,number_states);
for i=1:number_states
    
    num=dec2base(i-1, 2) - '0';
    new_codeword=zeros(1,Memory);
    new_codeword(Memory-length(num)+1:Memory)=num;
    old_codeword=zeros(1,Memory);
    transition=zeros(1,Memory+number_input);
    %coef_gamma=zeros(1,number_input);
    %coef_alpha=zeros(1,number_input);
    indice_courrant=1;
    for k=1:number_input
        transition(k)=new_codeword(indice_courrant);
        old_codeword(indice_courrant:indice_courrant+Memory_input(k)-2)=new_codeword(indice_courrant+1:indice_courrant+Memory_input(k)-1);
        indice_courrant=indice_courrant+Memory_input(k);
    end
    for k=1:2^number_input
        indice_courrant=0;
        
        
        num=zeros(1,number_input);
        p=dec2base(k-1, 2) - '0';
        num(number_input-length(p)+1:number_input)=p;
        
        for l=1:number_input
            
            indice_courrant=indice_courrant+Memory_input(l);
            old_codeword(indice_courrant)=num(l);
        end
        
        transition(1,number_input+1:length(transition))=old_codeword(1,:);
        
        
        
        coef_alpha=sum((2*ones(1,length(old_codeword))).^(length(old_codeword)-1:-1:0).*old_codeword)+1;
        coef_gamma=sum((2*ones(1,length(transition))).^(length(transition)-1:-1:0).*transition)+1;
        
        %i
        M_rec_gamma_alpha(coef_alpha,coef_gamma,i)=1;
        %M_rec_gamma_alpha(:,:,j)
        % M_rec_alpha_alpha(coef_alpha,j)=1;
    end
end

for i=1:number_states
    
    num=dec2base(i-1, 2) - '0';
    new_codeword=zeros(1,Memory);
    new_codeword(Memory-length(num)+1:Memory)=num;
    old_codeword=zeros(1,Memory);
    %coef_gamma=zeros(1,number_input);
    %coef_alpha=zeros(1,number_input);
    indice_courrant=1;
    for k=1:number_input
        old_codeword(indice_courrant:indice_courrant+Memory_input(k)-2)=new_codeword(indice_courrant+1:indice_courrant+Memory_input(k)-1);
        indice_courrant=indice_courrant+Memory_input(k);
    end
    for k=1:2^number_input
        indice_courrant=0;
        
        
        num=zeros(1,number_input);
        p=dec2base(k-1, 2) - '0';
        num(number_input-length(p)+1:number_input)=p;
        
        for l=1:number_input
            
            indice_courrant=indice_courrant+Memory_input(l);
            old_codeword(indice_courrant)=num(l);
        end
        
        
        coef_alpha=sum((2*ones(1,length(old_codeword))).^(length(old_codeword)-1:-1:0).*old_codeword)+1;
        
        
        M_rec_alpha_alpha(coef_alpha,i)=1;
    end
end



M_rec_alpha_alpha=log(M_rec_alpha_alpha);
end

