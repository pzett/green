function  [uhat Loglike] = logBCJRdecoding2(y,number_input,number_output,Memory,M_generator,sigma_epsilon,hard_soft, terminated,coef_alpha_alpha,coef_alpha_gamma,coef_beta_beta,coef_beta_gamma,coef_m_alpha,coef_m_beta,s,gamma_pre,mask)
%hard_soft =0 if soft decision, something else if hard decision
%p1 vector of probability to have one of lenght the number of input
%M_generator: 1 line = 1 polynome. the first line, tcorrespond to the firt
%input, the first output, then second input, first output, ext...
%y: receved vetor
%terminated: if equal to 0:  decoding as terminated message, else decoding as unterminated message

number_states=2^Memory;
leng_u=length(y)/number_output;
number_possible_transitions=(2^number_input)*number_states;
Gamma=gamma_pre;
Alpha=zeros(number_states,leng_u);
Beta=zeros(number_states,leng_u);
test=zeros(number_input,leng_u);

if hard_soft==0         %transformation of SNR in sigma (considering a BPSK mudulation)
    sigmaSquare_epsilon=1/(2*sigma_epsilon);
else
  sigmaSquare_epsilon=sigma_epsilon^2;
end

%calculation of gamma coefficients

Mat=ones(number_possible_transitions,1);

for j=1:leng_u

A=proba(Mat*y((j-1)*number_output+1:j*number_output),s(:,:),sigmaSquare_epsilon,hard_soft,mask);
R=Mat*mask((j-1)*number_output+1:j*number_output)';
A=sum(A.*R,2);

Gamma(:,j)=Gamma(:,j)+A;
end


%calculation of alpha coefficients
Alpha(1,1)=1;
for i=1:number_states
    Alpha(i,1)=log(Alpha(i,1));
end

for j=2:leng_u
    Alpha_temp=Alpha(:,j-1);
    Gamma_temp=Gamma(:,j-1);
    Alpha(:,j)=max(Alpha_temp(coef_alpha_alpha(:,:))+Gamma_temp(coef_alpha_gamma(:,:)),[],2);
end



%Calculation of beta coefficients
%has we don't know the final state of our correlator, all the states as the
%same probability when we start calculating Beta, and as we use log, we can
%give 0 as initial value of calculation.
if terminated==0
    Beta(1,size(Beta,2))=1;
    Beta(:,size(Beta,2))=log(Beta(:,size(Beta,2)));
end


for j=2:leng_u
    Beta_temp=Beta(:,leng_u+1-(j-1));
    Gamma_temp=Gamma(:,leng_u+1-(j-1));
    Beta(:,leng_u+1-j)=max(Beta_temp(coef_beta_beta(:,:))+Gamma_temp(coef_beta_gamma(:,:)),[],2);
    
end



%Calcuation of m coefficient

m_coef=Gamma(:,:)+Alpha(coef_m_alpha,:)+Beta(coef_m_beta,:);

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

Loglike=-test;
uhat=(1+sign(test))/2;
end

