function phihat = phase_estimation(r, b_train)
train_comp=qpsk(b_train);
r_train=r(1:length(train_comp));
% p=r_train.*conj(train_comp);
% p(1:10)
% phi=arg(p);
phi0=angle(r_train(1).*conj(train_comp(1)));
train_comp=train_comp*exp(1j*(phi0));
phi=angle(r_train.*conj(train_comp));
phi1=sum(phi)/length(phi);
train_comp=train_comp*exp(1j*(phi1));
phi=angle(r_train.*conj(train_comp));
phihat=sum(phi)/length(phi)+phi0+phi1;
phihat=mod(phihat,2*pi);
end
% phihat = phase_estimation(r, b_train)
%
% Phase estimator using the training sequence. The phase estimate is
% obtained by minimizing the norm of the difference between the known
% transmitted QPSK-modulated training sequence and the received training
% part. NB! There are other ways of estimating the phase, this is just
% one example.
%
% Input:
%   r       = received baseband signal
%   b_train = the training sequence bits
%
% Output:
%   phihat     = estimated phase