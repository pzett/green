clear
nr_iteration_max=20;
nr_bloc=10000;
nr_data_bits = 1000;
nr_bits_per_symbol = 1;
memory=3;
number_input=1;
number_state=2^memory;
% epsilon=0.18;
Poncture1=[1,1,1,1,1,1,1,1];
%Poncture2=[1,1,1,1,1,1,1,1];
%Poncture3=[1,1,1,1,1,1,1,1];
Poncture2=[1,0,0,0,0,1,0,0];
Poncture3=[0,0,1,0,0,0,0,1];
%Poncture2=[1,0,0,0,0,1,0,0];
%Poncture3=[0,0,1,0,0,0,0,1];
a=upsample(ones(nr_data_bits/4,1),8);
a0=upsample(ones(nr_data_bits/8,1),8);
mask1=filter(Poncture1,1,a0);
mask2=filter(Poncture2,1,a);
mask3=filter(Poncture3,1,a);
hard_soft=0;
number_output=2;
number_possible_transitions=(2^number_input)*number_state;
sum_err=0;
rate=length(mask1)/(sum([mask1;mask2;mask3]));
EbN0_db=2.4;
M_generatrice=[1 0 1 1; 0 1 0 0 ; 0 0 1 0 ; 1 1 1 0; 1 1 0 0];
[coef_alpha_alpha coef_alpha_gamma coef_m_beta]=coef_alpha(number_input,number_state,M_generatrice,memory);
[coef_beta_beta coef_beta_gamma coef_m_alpha] = coef_beta(number_input,number_state,M_generatrice,memory);
%[coef_m_alpha coef_m_beta] = coef_m(coef_beta_gamma,coef_alpha_gamma,number_input,number_state);
[s gamma_prei]=Generation_s(nr_data_bits,number_input,M_generatrice, 0.5,[],number_output,memory);
terminated=1;
%s2=(2*s-1).*mask2';
%s3=(2*s-1).*mask3';
mat=ones(number_possible_transitions/2,1);
EbN0_db=10*log10(rate)+EbN0_db;
sigmasquare=(1/(2*10^(EbN0_db/10)));

for bloc=1:nr_bloc
    bloc
%PI=(1:nr_data_bits);
PI=randperm(nr_data_bits);
PIinv(PI)=(1:length(PI));

%L_y=nr_data_bits;
sigma=sqrt(sigmasquare);


y1= random_data(nr_data_bits);
gamma_pre=gamma_prei;
%M_generatrice=[1 0 0 0; 0 1 0 0 ; 0 0 1 0 ; 1 0 0 0; 0 1 0 0];
y2bis=2*Convolute(y1,M_generatrice,memory,number_output,number_input)-1; 
y3bis=2*Convolute(y1(PI),M_generatrice,memory,number_output,number_input)-1;
y1=2*y1-1;
y2=(y2bis+sigma*randn(1,length(y2bis))).*mask2';
y3=(y3bis+sigma*randn(1,length(y3bis))).*mask3';
y1bis=y1;
y1=(y1+sigma*randn(1,length(y1))).*mask1';
%y2=zeros(size(y2));
%y3=zeros(size(y3));

for i=1:length(y1)
    B1=proba(y1(i),[0],sigmasquare,hard_soft,0);
    B2=proba(y1(i),[1],sigmasquare,hard_soft,0);
    gamma_pre(1:number_possible_transitions/2,i)=gamma_pre(1:number_possible_transitions/2,i)+B1; %log(epsilon/(1-epsilon))*y1(i);
    gamma_pre(number_possible_transitions/2+1:number_possible_transitions,i)= gamma_pre(number_possible_transitions/2+1:number_possible_transitions,i)+B2;  %log(epsilon/(1-epsilon))*y1(i);
end

A1=gamma_pre;
%E2=gamma_pre;
iteration=1;
test=1;
count=0;
while iteration<nr_iteration_max&&test&&count<3
  %  iteration
    [uhat1 Loglike1] = logBCJRdecoding2(y2,number_input,number_output,memory,M_generatrice,sigma,hard_soft,terminated,coef_alpha_alpha,coef_alpha_gamma,coef_beta_beta,coef_beta_gamma,coef_m_alpha,coef_m_beta,s,A1,mask2);
   % size(Loglike)
%    for j=1:length(y1)
%        E1(1:number_possible_transitions/2,j)=Loglike1(1,j)+gamma_pre(number_possible_transitions/2+1:number_possible_transitions,j)-A1(1:number_possible_transitions/2,j);
       % E(1:number_possible_transitions/2,j)=Loglike(1,j)-E(1:number_possible_transitions/2,j);
       % E(number_possible_transitions/2+1:number_possible_transitions,j)=gamma_pre(number_possible_transitions/2+1:number_possible_transitions,j)-E(number_possible_transitions/2+1:number_possible_transitions,j);
%E1(number_possible_transitions/2+1:number_possible_transitions,j)=-Loglike1(1,j)+gamma_pre(number_possible_transitions/2+1:number_possible_transitions,j)-A1(number_possible_transitions/2+1:number_possible_transitions,j);
%E1(number_possible_transitions/2+1:number_possible_transitions,j)=gamma_pre(number_possible_transitions/2+1:number_possible_transitions,j)-A1(number_possible_transitions/2+1:number_possible_transitions,j);
%    end
    
   E1(1:number_possible_transitions/2,:)=mat*Loglike1(1,:)+gamma_pre(1:number_possible_transitions/2,:)-A1(1:number_possible_transitions/2,:); 
   E1(number_possible_transitions/2+1:number_possible_transitions,:)=gamma_pre(number_possible_transitions/2+1:number_possible_transitions,:)-A1(number_possible_transitions/2+1:number_possible_transitions,:);
 %E1=gamma_pre;
    
    k=abs(2*uhat1-1-y1bis);
    miaou=sum(k);
    
    
 %   i
 %   sum(uhat)
 %   k=abs(2*uhat-1-y1bis);
  %  miaou=sum(k)
    %E2=E;
    %E=Loglike-E+gamma_pre;
    for j=1:number_possible_transitions
        A2(j,:)=E1(j,PI);
    end
    [uhat Loglike2] = logBCJRdecoding2(y3,number_input,number_output,memory,M_generatrice,sigma,hard_soft,terminated,coef_alpha_alpha,coef_alpha_gamma,coef_beta_beta,coef_beta_gamma,coef_m_alpha,coef_m_beta,s,A2,mask3);
%     for j=1:length(y1)
%         E2(1:number_possible_transitions/2,j)=Loglike2(1,j)-A2(1:number_possible_transitions/2,j); 
%         %E(1:number_possible_transitions/2,j)=Loglike(1,j);
%      %   E2(number_possible_transitions/2+1:number_possible_transitions,j)=-Loglike2(1,j)-A2(number_possible_transitions/2+1:number_possible_transitions,j);
%       E2(number_possible_transitions/2+1:number_possible_transitions,j)=-A2(number_possible_transitions/2+1:number_possible_transitions,j);
%     end
    test=sum(uhat~=uhat1);
    if test
        count=0;
    end
    E2(1:number_possible_transitions/2,:)=mat*Loglike2(1,:)-A2(1:number_possible_transitions/2,:);
    E2(number_possible_transitions/2+1:number_possible_transitions,:)=-A2(number_possible_transitions/2+1:number_possible_transitions,:);
    
    
    
%    E=Loglike-E;
    for j=1:number_possible_transitions
        A1(j,:)=E2(j,PIinv);
    end
    A1=A1+gamma_pre;
    uhat=uhat(PIinv);
  %      sum(uhat)
    k=abs(2*uhat-1-y1bis);
   miaou=sum(k);
    iteration=iteration+1;
    test=sum(uhat~=uhat1);
    count=count+1;
    if test
        count=0;
    end
end

sum_err=sum_err+miaou/2
end
