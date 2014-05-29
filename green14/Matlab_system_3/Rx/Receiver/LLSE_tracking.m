function [delta_nu, eta] = LLSE_tracking(data_FFT,pilot_pattern,nr_car, length_cpre,L)

M=length(pilot_pattern);
delta_phase=angle(data_FFT);

weight=[0:1:L-1]*(nr_car+length_cpre)+length_cpre;
weightM=ones(M,1)*weight;

num=sum(sum(weightM.*delta_phase));
den=2*pi*M*sum(weight.^2);
delta_nu=num/den;

k=pilot_pattern-1;
%k(2)=mod(k(2), nr_car/2)-nr_car/2;
weightM2=k'*weight;

num=sum(sum(weightM2.*delta_phase));
den=2*pi/nr_car*sum(weight.^2)*sum(k.^2);

eta=num/den;

end
% L=20;
% channel_filter=[1 0 0 0 0 0]; 
% channel=fft(channel_filter, nr_car);
% channel_est=channel([14 19]).';
% %for L=1:40
% 
% M=size(channel_pilot,1);
% delta_phase=angle(channel_pilot./(channel_est*ones(1, size(channel_pilot,2))));
% plot(delta_phase.')
% figure; hold on;
% 
% for i=0:size(channel_pilot,2)-L-1
% 
% delta_phase=angle(channel_pilot(:,1+i:L+i)./(channel_est*ones(1, L)));
% plot(delta_phase(1,:))
% weight=[0:1:*L-1]*(nr_car+length_cpre+length_suf)+length_cpre;
% weightM=ones(M,1)*weight;
% 
% num=sum(sum(weightM.*delta_phase));
% den=2*pi*M*weight*weight';
% L;
% delta_nu(i+1)=num/den;
% 
% pilot_pattern=[4 126]/2;
% k=pilot_pattern-1;
% k(2)=mod(k(2), nr_car/2)-nr_car/2;
% weightM2=k'*weight;
% num=sum(sum(weightM2.*delta_phase));
% den=2*pi/nr_car*weight*weight'*(k*k');
% 
% eta(i+1)=num/den;
% 
% %% Apply correction
% weight2=
% correction(:,i+1)=2*pi*delta_nu(i+1).*weight2
% 
% end
% figure(1); hold on; plot(1*10^6*delta_nu, 'o')
% delta_nu
