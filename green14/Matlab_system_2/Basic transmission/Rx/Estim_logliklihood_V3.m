function [ loglike ] =Estim_logliklihood_V3(data_received_equalized,channel_est,modulation_type,sigma_noise_sqr)
% Estimate the loglikelihood ratio looking at the equalized data, the estimated of the channel (same size has the data equalized table) the power of the noise (which is considered as a constant for everything)
%it works for different modulations type: qpsk, bpsk, 16-QAM, 32-QAM, 64-QAM, 128-QAM, 16-BEES,64-BEES
if strcmp(modulation_type,'qpsk')
    loglike_r=real(data_received_equalized).*sqrt(2)/sigma_noise_sqr.*abs(channel_est);
    loglike_i=imag(data_received_equalized).*sqrt(2)/sigma_noise_sqr.*abs(channel_est);
    loglike_r_row=reshape(loglike_r,1,size(loglike_r,1)*size(loglike_r,2));
    loglike_i_row=reshape(loglike_i,1,size(loglike_i,1)*size(loglike_i,2));
    loglike=upfirdn(loglike_r_row,[1 0],2, 1)+upfirdn(loglike_i_row,[0 1],2, 1);
elseif strcmp(modulation_type,'bpsk')
    loglike_r=real(data_received_equalized)./sigma_noise_sqr.*abs(channel_est);
    loglike=reshape(loglike_r,1,size(loglike_r,1)*size(loglike_r,2))*2;
elseif strcmp(modulation_type,'16-QAM')
    data_received_equalized=reshape(data_received_equalized,1,size(data_received_equalized,1)*size(data_received_equalized,2));
    channel_est=reshape(channel_est,1,size(channel_est,1)*size(channel_est,2));
    nr_bits_axe=2;
    loglike=zeros(2*nr_bits_axe,length(data_received_equalized));
    data_test_bin=zeros(2^nr_bits_axe,nr_bits_axe);
    for i=1:nr_bits_axe
        a=zeros(2^nr_bits_axe/(2^i),(2^(i-1)));
        b=ones(2^nr_bits_axe/(2^i),(2^(i-1)));
        data_test_bin(:,i)=reshape([a;b],2^nr_bits_axe,1);
    end
    data_test_bin=data_test_bin';
    data_test_bin=reshape(data_test_bin,1,size(data_test_bin, 1)*size(data_test_bin, 2));
    data_test=ASK(data_test_bin,nr_bits_axe);
    test_table_real=(((ones(2^nr_bits_axe,1)*real(data_received_equalized)-(data_test.')*ones(1,length(data_received_equalized))).*(ones(2^nr_bits_axe,1)*abs(channel_est))).^2)/(2*sigma_noise_sqr);
    test_table_imag=(((ones(2^nr_bits_axe,1)*imag(data_received_equalized)-(data_test.')*ones(1,length(data_received_equalized))).*(ones(2^nr_bits_axe,1)*abs(channel_est))).^2)/(2*sigma_noise_sqr);
    data_test_bin=reshape(data_test_bin,nr_bits_axe,length(data_test_bin)/nr_bits_axe);
    for i=1:nr_bits_axe
        pos=test_table_real(logical(1-data_test_bin(i,:)),:);
        p_Ma=min(pos);
        pos=pos-ones(2^nr_bits_axe/2,1)*p_Ma;
        neg=test_table_real(logical((data_test_bin(i,:))),:);
        n_Ma=min(neg);
        neg=neg-ones(2^nr_bits_axe/2,1)*n_Ma;
        loglike(i,:)=-p_Ma+n_Ma+log(sum(exp(-pos),1))-log(sum(exp(-neg),1));
        
        pos=test_table_imag(logical(1-data_test_bin(i,:)),:);
        p_Ma=min(pos);
        pos=pos-ones(2^nr_bits_axe/2,1)*p_Ma;
        neg=test_table_imag(logical((data_test_bin(i,:))),:);
        n_Ma=min(neg);
        neg=neg-ones(2^nr_bits_axe/2,1)*n_Ma;
        loglike(nr_bits_axe+i,:)=-p_Ma+n_Ma+log(sum(exp(-pos),1))-log(sum(exp(-neg),1));
    end  
    loglike=reshape(loglike,1,size(loglike,1)*size(loglike,2));
elseif strcmp(modulation_type,'64-QAM')
    data_received_equalized=reshape(data_received_equalized,1,size(data_received_equalized,1)*size(data_received_equalized,2));
    channel_est=reshape(channel_est,1,size(channel_est,1)*size(channel_est,2));
    nr_bits_axe=3;
    loglike=zeros(2*nr_bits_axe,length(data_received_equalized));
    data_test_bin=zeros(2^nr_bits_axe,nr_bits_axe);
    for i=1:nr_bits_axe
        a=zeros(2^nr_bits_axe/(2^i),(2^(i-1)));
        b=ones(2^nr_bits_axe/(2^i),(2^(i-1)));
        data_test_bin(:,i)=reshape([a;b],2^nr_bits_axe,1);
    end
    data_test_bin=data_test_bin';
    data_test_bin=reshape(data_test_bin,1,size(data_test_bin, 1)*size(data_test_bin, 2));
    data_test=ASK(data_test_bin,nr_bits_axe);
    test_table_real=(((ones(2^nr_bits_axe,1)*real(data_received_equalized)-(data_test.')*ones(1,length(data_received_equalized))).*(ones(2^nr_bits_axe,1)*abs(channel_est))).^2)/(2*sigma_noise_sqr);
    test_table_imag=(((ones(2^nr_bits_axe,1)*imag(data_received_equalized)-(data_test.')*ones(1,length(data_received_equalized))).*(ones(2^nr_bits_axe,1)*abs(channel_est))).^2)/(2*sigma_noise_sqr);
    data_test_bin=reshape(data_test_bin,nr_bits_axe,length(data_test_bin)/nr_bits_axe);
    for i=1:nr_bits_axe
        pos=test_table_real(logical(1-data_test_bin(i,:)),:);
        p_Ma=min(pos);
        pos=pos-ones(2^nr_bits_axe/2,1)*p_Ma;
        neg=test_table_real(logical((data_test_bin(i,:))),:);
        n_Ma=min(neg);
        neg=neg-ones(2^nr_bits_axe/2,1)*n_Ma;
        loglike(i,:)=-p_Ma+n_Ma+log(sum(exp(-pos),1))-log(sum(exp(-neg),1));
        
        pos=test_table_imag(logical(1-data_test_bin(i,:)),:);
        p_Ma=min(pos);
        pos=pos-ones(2^nr_bits_axe/2,1)*p_Ma;
        neg=test_table_imag(logical((data_test_bin(i,:))),:);
        n_Ma=min(neg);
        neg=neg-ones(2^nr_bits_axe/2,1)*n_Ma;
        loglike(nr_bits_axe+i,:)=-p_Ma+n_Ma+log(sum(exp(-pos),1))-log(sum(exp(-neg),1));
    end  
    loglike=reshape(loglike,1,size(loglike,1)*size(loglike,2));
elseif strcmp(modulation_type, '32-QAM')
    data_received_equalized = reshape(data_received_equalized,1, size(data_received_equalized,1)*size(data_received_equalized,2));
    channel_est=reshape(channel_est, 1, size(channel_est,1)*size(channel_est,2));
    nr_bits=5;
    loglike=zeros(nr_bits,length(data_received_equalized));
    data_test_bin=zeros(2^nr_bits,nr_bits);
    for i=1:nr_bits
        a=zeros(2^nr_bits/(2^i),(2^(i-1)));
        b=ones(2^nr_bits/(2^i),(2^(i-1)));
        data_test_bin(:,i)=reshape([a;b],2^nr_bits,1);
    end
    data_test_bin=data_test_bin';
    
    data_test_bin=reshape(data_test_bin,1,size(data_test_bin, 1)*size(data_test_bin, 2));
    
    data_test=qam32(data_test_bin);
    test_table=abs((((ones(2^nr_bits,1)*(data_received_equalized)-(data_test.')*ones(1,length(data_received_equalized))).*(ones(2^nr_bits,1)*abs(channel_est)))).^2)/(2*sigma_noise_sqr);
    data_test_bin=reshape(data_test_bin,nr_bits,length(data_test_bin)/nr_bits);
    for i=1:nr_bits
        pos=test_table(logical(1-data_test_bin(i,:)),:);
        p_Ma=min(pos);
        pos=pos-ones(2^nr_bits/2,1)*p_Ma;
        neg=test_table(logical((data_test_bin(i,:))),:);
        n_Ma=min(neg);
        neg=neg-ones(2^nr_bits/2,1)*n_Ma;
        loglike(i,:)=-p_Ma+n_Ma+log(sum(exp(-pos),1))-log(sum(exp(-neg),1));
    end
    loglike=reshape(loglike,1,size(loglike,1)*size(loglike,2));
elseif strcmp(modulation_type, '128-QAM')
    data_received_equalized = reshape(data_received_equalized,1, size(data_received_equalized,1)*size(data_received_equalized,2));
    channel_est=reshape(channel_est, 1, size(channel_est,1)*size(channel_est,2));
    nr_bits=7;
    loglike=zeros(nr_bits,length(data_received_equalized));
    data_test_bin=zeros(nr_bits,2^nr_bits)';
    for i=1:nr_bits
        a=zeros(2^nr_bits/(2^i),(2^(i-1)));
        b=ones(2^nr_bits/(2^i),(2^(i-1)));
        data_test_bin(:,i)=reshape([a;b],2^nr_bits,1);
    end
    data_test_bin=data_test_bin';
    
    data_test_bin=reshape(data_test_bin,1,size(data_test_bin, 1)*size(data_test_bin, 2));
    
    data_test=QAM128(data_test_bin);
    test_table=abs((((ones(2^nr_bits,1)*(data_received_equalized)-(data_test.')*ones(1,length(data_received_equalized))).*(ones(2^nr_bits,1)*abs(channel_est)))).^2)/(2*sigma_noise_sqr);
    data_test_bin=reshape(data_test_bin,nr_bits,length(data_test_bin)/nr_bits);
   for i=1:nr_bits
        pos=test_table(logical(1-data_test_bin(i,:)),:);
        p_Ma=min(pos);
        pos=pos-ones(2^nr_bits/2,1)*p_Ma;
        neg=test_table(logical((data_test_bin(i,:))),:);
        n_Ma=min(neg);
        neg=neg-ones(2^nr_bits/2,1)*n_Ma;
        loglike(i,:)=-p_Ma+n_Ma+log(sum(exp(-pos),1))-log(sum(exp(-neg),1));
    end
    loglike=reshape(loglike,1,size(loglike,1)*size(loglike,2));
elseif strcmp(modulation_type,'64-BEES')
    data_received_equalized = reshape(data_received_equalized,1, size(data_received_equalized,1)*size(data_received_equalized,2));
    channel_est=reshape(channel_est, 1, size(channel_est,1)*size(channel_est,2));
    nr_bits=6;
    loglike=zeros(nr_bits,length(data_received_equalized));
    data_test_bin=zeros(2^nr_bits,nr_bits);
    for i=1:nr_bits
        a=zeros(2^nr_bits/(2^i),(2^(i-1)));
        b=ones(2^nr_bits/(2^i),(2^(i-1)));
        data_test_bin(:,i)=reshape([a;b],2^nr_bits,1);
    end
    data_test_bin=data_test_bin';
    data_test_bin=reshape(data_test_bin,1,size(data_test_bin, 1)*size(data_test_bin, 2));
    
    data_test=BEES64(data_test_bin);
    test_table=abs((((ones(2^nr_bits,1)*(data_received_equalized)-(data_test.')*ones(1,length(data_received_equalized))).*(ones(2^nr_bits,1)*abs(channel_est)))).^2)/(2*sigma_noise_sqr);
    data_test_bin=reshape(data_test_bin,nr_bits,length(data_test_bin)/nr_bits);
    for i=1:nr_bits
        pos=test_table(logical(1-data_test_bin(i,:)),:);
        p_Ma=min(pos);
        pos=pos-ones(2^nr_bits/2,1)*p_Ma;
        neg=test_table(logical((data_test_bin(i,:))),:);
        n_Ma=min(neg);
        neg=neg-ones(2^nr_bits/2,1)*n_Ma;
        loglike(i,:)=-p_Ma+n_Ma+log(sum(exp(-pos),1))-log(sum(exp(-neg),1));
    end
    loglike=reshape(loglike,1,size(loglike,1)*size(loglike,2));
elseif strcmp(modulation_type,'64-WASPS')
    data_received_equalized = reshape(data_received_equalized,1, size(data_received_equalized,1)*size(data_received_equalized,2));
    channel_est=reshape(channel_est, 1, size(channel_est,1)*size(channel_est,2));
    nr_bits=6;
    loglike=zeros(nr_bits,length(data_received_equalized));
    data_test_bin=zeros(2^nr_bits,nr_bits);
    for i=1:nr_bits
        a=zeros(2^nr_bits/(2^i),(2^(i-1)));
        b=ones(2^nr_bits/(2^i),(2^(i-1)));
        data_test_bin(:,i)=reshape([a;b],2^nr_bits,1);
    end
    data_test_bin=data_test_bin';
    data_test_bin=reshape(data_test_bin,1,size(data_test_bin, 1)*size(data_test_bin, 2));
    
    data_test=WASPS64(data_test_bin);
    test_table=abs((((ones(2^nr_bits,1)*(data_received_equalized)-(data_test.')*ones(1,length(data_received_equalized))).*(ones(2^nr_bits,1)*abs(channel_est)))).^2)/(2*sigma_noise_sqr);
    data_test_bin=reshape(data_test_bin,nr_bits,length(data_test_bin)/nr_bits);
    for i=1:nr_bits
        pos=test_table(logical(1-data_test_bin(i,:)),:);
        p_Ma=min(pos);
        pos=pos-ones(2^nr_bits/2,1)*p_Ma;
        neg=test_table(logical((data_test_bin(i,:))),:);
        n_Ma=min(neg);
        neg=neg-ones(2^nr_bits/2,1)*n_Ma;
        loglike(i,:)=p_Ma-n_Ma+(log(sum(exp(-pos),1))-log(sum(exp(-neg),1)));
    end
    loglike=reshape(loglike,1,size(loglike,1)*size(loglike,2));
elseif strcmp(modulation_type,'16-BEES')
    
    data_received_equalized = reshape(data_received_equalized,1, size(data_received_equalized,1)*size(data_received_equalized,2));
    channel_est=reshape(channel_est, 1, size(channel_est,1)*size(channel_est,2));
    nr_bits=4;
    loglike=zeros(nr_bits,length(data_received_equalized));
    data_test_bin=zeros(2^nr_bits,nr_bits);
    for i=1:nr_bits
        a=zeros(2^nr_bits/(2^i),(2^(i-1)));
        b=ones(2^nr_bits/(2^i),(2^(i-1)));
        data_test_bin(:,i)=reshape([a;b],2^nr_bits,1);
    end
    data_test_bin=data_test_bin';
    data_test_bin=reshape(data_test_bin,1,size(data_test_bin, 1)*size(data_test_bin, 2));
    
    data_test=BEES16(data_test_bin);
    test_table=abs((((ones(2^nr_bits,1)*(data_received_equalized)-(data_test.')*ones(1,length(data_received_equalized))).*(ones(2^nr_bits,1)*abs(channel_est)))).^2)/(2*sigma_noise_sqr);
    data_test_bin=reshape(data_test_bin,nr_bits,length(data_test_bin)/nr_bits);
  for i=1:nr_bits
        pos=test_table(logical(1-data_test_bin(i,:)),:);
        p_Ma=min(pos);
        pos=pos-ones(2^nr_bits/2,1)*p_Ma;
        neg=test_table(logical((data_test_bin(i,:))),:);
        n_Ma=min(neg);
        neg=neg-ones(2^nr_bits/2,1)*n_Ma;
        loglike(i,:)=-p_Ma+n_Ma+log(sum(exp(-pos),1))-log(sum(exp(-neg),1));
    endke(i,:)=p_Ma-n_Ma+(log(sum(exp(-pos),1))-log(sum(exp(-neg),1)));
    end
    loglike=reshape(loglike,1,size(loglike,1)*size(loglike,2));
end
end
