function [M_rec_beta_beta M_rec_gamma_beta] = Matrix_Rec_Beta(M_generator ,number_input)
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







M_rec_gamma_beta=zeros(number_states,number_possible_transitions,number_states);
M_rec_beta_beta=zeros(number_states,number_states);
for i=1:number_states
    
    num=dec2base(i-1, 2) - '0';
    old_codeword=zeros(1,Memory);
    old_codeword(Memory-length(num)+1:Memory)=num;
    new_codeword=zeros(1,Memory);
    transition=zeros(1,Memory+number_input);
    indice_courrant=1;
    transition(1,number_input+1:length(transition))=old_codeword(1,:);
    
    for k=1:number_input
        new_codeword(1,indice_courrant+1:indice_courrant+Memory_input(k)-1)=old_codeword(1,indice_courrant:indice_courrant+Memory_input(k)-2);
        indice_courrant=indice_courrant+Memory_input(k)+1;
    end
    
    for k=1:2^number_input
        indice_courrant=1;
        
        
        num=zeros(1,number_input);
        p=dec2base(k-1, 2) - '0';
        num(number_input-length(p)+1:number_input)=p;
        
        for l=1:number_input
            new_codeword(indice_courrant)=num(l);
            indice_courrant=indice_courrant+Memory_input(l);
        end
        
        transition(1,number_input+1:length(transition))=old_codeword(1,:);
        transition(1:number_input)=num;
        coef_beta=sum((2*ones(1,length(new_codeword))).^(length(new_codeword)-1:-1:0).*new_codeword)+1;
        coef_gamma=sum((2*ones(1,length(transition))).^(length(transition)-1:-1:0).*transition)+1;

        M_rec_gamma_beta(coef_beta,coef_gamma,i)=1;
    end
end

for i=1:number_states
    
    num=dec2base(i-1, 2) - '0';
    old_codeword=zeros(1,Memory);
    old_codeword(Memory-length(num)+1:Memory)=num;
    new_codeword=zeros(1,Memory);
    indice_courrant=1;
    for k=1:number_input
        new_codeword(1,indice_courrant+1:indice_courrant+Memory_input(k)-1)=old_codeword(1,indice_courrant:indice_courrant+Memory_input(k)-2);
        indice_courrant=indice_courrant+Memory_input(k)+1;
    end
    for k=1:2^number_input
        indice_courrant=1;
        
        
        num=zeros(1,number_input);
        p=dec2base(k-1, 2) - '0';
        num(number_input-length(p)+1:number_input)=p;
        
        for l=1:number_input
            new_codeword(indice_courrant)=num(l);
            indice_courrant=indice_courrant+Memory_input(l);
        end
        
        
        coef_beta=sum((2*ones(1,length(new_codeword))).^(length(new_codeword)-1:-1:0).*new_codeword)+1;
        
        
        M_rec_beta_beta(coef_beta,i)=1;
    end
end

size(M_rec_beta_beta)

M_rec_beta_beta=log(M_rec_beta_beta);
end

