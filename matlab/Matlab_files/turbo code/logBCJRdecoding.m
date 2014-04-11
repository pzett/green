function [uhat Alpha] = logBCJRdecoding(y,number_input,M_generator, p1,SNR_epsilon,hard_soft, terminated) 
%hard_soft =0 if soft decision, something else if hard decision
%p1 vector of probability to have one of lenght the number of input
%M_generator: 1 line = 1 polynome. the first line, tcorrespond to the firt
%input, the first output, then second input, first output, ext...
%y: receved vetor
%terminated: if equal to 0:  decoding as terminated message, else decoding as unterminated message

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
leng_u=length(y)/number_output;
number_possible_transitions=(2^number_input)*number_states;
Gamma=zeros(number_possible_transitions,leng_u);
Alpha=zeros(number_states,leng_u);
Beta=zeros(number_states,leng_u);
test=zeros(number_input,leng_u);
m_coef=zeros(number_possible_transitions,leng_u);

if hard_soft==0         %transformation of SNR in sigma (considering a BPSK mudulation)
    sigmaSquare_epsilon=1/(2*SNR_epsilon);
else
    sigmaSquare_epsilon=SNR_epsilon;
end

%calculation of gamma coefficients
for i=1:number_possible_transitions
    temp=zeros(1,Memory+number_input);
    t=dec2base(i-1, 2) - '0';
    temp(Memory+number_input-length(t)+1:Memory+number_input)=t;
    for j=1:leng_u
        coef_courrant=number_possible_transitions/2;
        for k=1:number_input
            Gamma(i,j)=Gamma(i,j)+log((p1(k)*(sign(mod(i-1,2*coef_courrant)+1-coef_courrant-0.5)))+(1-sign(mod(i-1,2*coef_courrant)+1-coef_courrant-0.5))/2); %log of the probability p1 if the input is 1, 1-p1 if the input is 0
            coef_courrant=coef_courrant/2;
        end
        for k=1:number_output
            Mask=zeros(1, number_input+Memory);
            indice_courrant_Mask=number_input+1;
            for l=1:number_input
                Mask(l)=M_generator((k-1)*number_input+l,1);
                Mask(indice_courrant_Mask:indice_courrant_Mask+Memory_input(l)-1)=M_generator((k-1)*number_input+l,2:1+Memory_input(l));
                indice_courrant_Mask=indice_courrant_Mask+Memory_input(l);
            end
            s=mod(sum(temp.*Mask),2);
            Gamma(i,j)=Gamma(i,j)+log(proba_bis(y((j-1)*number_output+k),s,sigmaSquare_epsilon,hard_soft));
        end
    end
end

%calculation of alpha coefficients
Alpha(1,1)=1;
for i=1:number_states
    Alpha(i,1)=log(Alpha(i,1));
end
for j=2:leng_u
    for i=1:number_states
        num=dec2base(i-1, 2) - '0';
        new_codeword=zeros(1,Memory);
        new_codeword(Memory-length(num)+1:Memory)=num;
        old_codeword=zeros(1,Memory);
        transition=zeros(1,Memory+number_input);
        coef_gamma=zeros(1,number_input);
        coef_alpha=zeros(1,number_input);
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
            
            
            
            coef_alpha(k)=sum((2*ones(1,length(old_codeword))).^(length(old_codeword)-1:-1:0).*old_codeword)+1;
            coef_gamma(k)=sum((2*ones(1,length(transition))).^(length(transition)-1:-1:0).*transition)+1;
        end
        %         num=dec2base(85, 2) - '0';
        % k=sum((2*ones(1,length(num))).^(length(num)-1:-1:0).*num)
        
        
        Alpha(i,j)=max(Alpha(coef_alpha,j-1)+Gamma(coef_gamma,j-1));
    end
end


%Calculation of beta coefficients
%has we don't know the final state of our correlator, all the states as the
%same probability when we start calculating Beta, and as we use log, we can
%give 0 as initial value of calculation.
if terminated==0
   Beta(1,length(Beta))=1;
   Beta(:,length(Beta))=log(Beta(:,length(Beta)));
end
for j=2:leng_u
    for i=1:number_states
        
        
        
        
        num=dec2base(i-1, 2) - '0';
        old_codeword=zeros(1,Memory);
        old_codeword(Memory-length(num)+1:Memory)=num;
        new_codeword=zeros(1,Memory);
        transition=zeros(1,Memory+number_input);
        coef_gamma=zeros(1,number_input);
        coef_beta=zeros(1,number_input);
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
            transition(1:number_input)=num;
            coef_beta(k)=sum((2*ones(1,length(new_codeword))).^(length(new_codeword)-1:-1:0).*new_codeword)+1;
            coef_gamma(k)=sum((2*ones(1,length(transition))).^(length(transition)-1:-1:0).*transition)+1;
        end
        Beta(i,leng_u+1-j)=max(Beta(coef_beta,leng_u+1-(j-1))+Gamma(coef_gamma,leng_u+1-(j-1)));
    end
end

%Calcuation of m coefficient
for i=1:number_possible_transitions
    num=dec2base(i-1, 2) - '0';
    transition=zeros(1,number_input+Memory);
    transition(number_input+Memory-length(num)+1:number_input+Memory)=num;
    new_codeword=zeros(1,Memory);
    indice_courrant=1;
    old_codeword=transition(number_input+1:length(transition));
    for k=1:number_input
        new_codeword(indice_courrant)=transition(k);
        new_codeword(indice_courrant+1:indice_courrant+Memory_input(k)-1)=old_codeword(indice_courrant:indice_courrant+Memory_input(k)-2);
        indice_courrant=indice_courrant+Memory_input(k);
    end
    coef_beta=sum((2*ones(1,length(new_codeword))).^(length(new_codeword)-1:-1:0).*new_codeword)+1;
    coef_alpha=sum((2*ones(1,length(old_codeword))).^(length(old_codeword)-1:-1:0).*old_codeword)+1;
    m_coef(i,:)=Gamma(i,:)+Alpha(coef_alpha,:)+Beta(coef_beta,:);
end

tall_group=number_possible_transitions/2;
for i=1:number_input;
    s_minus=log(zeros(1,leng_u));
    s_plus=s_minus;
    for k=1:2*tall_group:number_possible_transitions
        s_minus=max(max(m_coef(k:k+tall_group-1,:)),s_minus);
        s_plus=max(max(m_coef(k+tall_group:k+2*tall_group-1,:)),s_plus);
    end
    tall_group=tall_group/2;
    test(i,:)=s_plus(1,:)-s_minus(1,:);
end
%Alpha
%Beta
%Gamma

uhat=(1+sign(test))/2;
end

