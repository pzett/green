function [epsilon,phi_hat,channel]=LLH_est(data_rx_train,train,nr_car, PHN_cov,sigma_sqr,L,rho)
%This function tries to estimate the Channel Impulse Response trying to
%mitigate the effect of CFO and PHN in the estimate. It can be shown that
%for low SNR it reaches de CRLB. It uses joint loglikelihood function with
%those three variables.
%1st step: estimating the frequency offset
%2nd step: estimating the phase offset
%3rd step: estimate CIR (its length it's a parameter)

%train=without zeroes interpolated
%rho=1/sqrt(2) for bpsk modulation.

%     data_rx_train=rx_data_reshaped(l_cpre+1:l_cpre+nr_car,1); %Because the first OFDM symbol is training
    R=diag(data_rx_train); %I need a diagonal matrix of the received training
    R1=diag(data_rx_train(1:nr_car/2)); %I need a diagonal matrix with half of the sequence (since it is repeated twice in time domain)
    PHN_cos_delta=2*PHN_cov(1:nr_car/2,1:nr_car/2)-PHN_cov(1:nr_car/2,nr_car/2+1:end)-PHN_cov(nr_car/2+1:end,1:nr_car/2); %Compute the delta covariance as the paper says
    D=diag(train); %Diagonal matrix with the transformed sequence
    F=dftmtx(nr_car/2)/sqrt(nr_car/2); %Fourier matrix of the size of half training sequence
    W=F(:,1:L); %Decomposition of the Fourier matrix
    V=F(:,L+1:end); %Decomposition of the Fourier matrix
%     epsilon=1/(pi*nr_car)*angle(data_rx_train(1:nr_car/2)'*data_rx_train(nr_car/2+1:end))
    epsilon=1/(pi*nr_car)*(mod(angle(data_rx_train(1:nr_car/2)'*(R1*PHN_cos_delta*R1'+4*sigma_sqr*eye(nr_car/2))^-1*data_rx_train(nr_car/2+1:end)),2*pi)); %Offset frequence
%     epsilon=1/(pi*nr_car)*angle(data_rx_train(1:nr_car/2)'*(R1*PHN_cos_delta*R1'+4*sigma_sqr*eye(nr_car/2))^-1*data_rx_train(nr_car/2+1:end)) %Offset frequence
    E=diag(exp(1i*2*pi*epsilon*[0:nr_car-1])); %Diagonal matrix with the f_offset
    A=R'*[F'*D zeros(size(F'*D)); zeros(size(F'*D)) F'*D]*[eye(nr_car/2)+V*V' -W*W';-W*W' eye(nr_car/2)+V*V']*[F'*D zeros(size(F'*D)); zeros(size(F'*D)) F'*D]'*R; %Matrix used in calculations
    tmp=(real(E*A*E')+4*sigma_sqr*rho^2*PHN_cov^(-1))^-1*imag(E*A*E');
%     imagesc(tmp);
%     colorbar
phi_hat=(real(E*A*E')+4*sigma_sqr*rho^2*PHN_cov^(-1))^-1*imag(E*A*E')*ones(nr_car,1);%Phase_offset
    P_est=diag(exp(1i*phi_hat));%Rotation constant
channel=(4*rho^2)^-1*W'*D'*[F F]*P_est'*E'*data_rx_train;%CIR
%     tmp=W'*D'*[F F]*P_est'*E';
%     figure,
%     subplot(2,2,1), plot(real(tmp));
%     subplot(2,2,2), plot(imag(tmp));
%     subplot(2,2,3), plot(abs(tmp));
%     subplot(2,2,4), plot(angle(tmp));
%     figure
%     subplot(2,2,1), plot(real(data_rx_train));
%     subplot(2,2,2), plot(imag(data_rx_train));
%     subplot(2,2,3), plot(abs(data_rx_train));
%     subplot(2,2,4), plot(angle(data_rx_train));
%     figure
%     subplot(1,2,1), plot(abs(tmp)), hold on, plot(abs(data_rx_train),'r');
%     subplot(1,2,2), plot(angle(tmp)), hold on, plot(angle(data_rx_train),'r');
%     figure, plot(phi_hat);