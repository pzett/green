function b = training_sequence(nr_training_bits)
% b = training_sequence(nr_training_bits)
%
% Generate a training sequence consisting of n bits. Currently, a random
% sequence is used, but a deterministic sequence with better
% autocorrelation properties could be used instead.
%
% Input
%   nr_training_bits = length of training sequence (should be an even number)
%
% Output
%   b = training sequence {0/1}
%
% History:
%   2000-09-28  written /Stefan Parkvall
%   2001-10-21  modified / George Jöngren

%if the training sequence is random
b = (rand(1, nr_training_bits) > .5);


%if the training sequence is optimized
%  b=zeros(1,1);
%  load optimal5.mat
%  for i=1:5
%      b(2*i-1)=(u(i)+1)/2;
%  b(2*i)=(2*u(6-i)+1)/2;
%  
%  end




% following: code to create the optimized trainning sequences

% b=zeros(1,nr_training_bits);
% b(1)=1;
% %number_boucles=0;
% for i=2:nr_training_bits
%     int=xcorr([b(1:i-1) -1]);
%     int(i)=0;
%     int2=xcorr([b(1:i-1) 1]);
%     int2(i)=0;
%     b(i)=sign(max(abs(int))-max(abs(int2)));
%     if b(i)==0
%         b(i)=-sign(sum(b));
%     end
%     if b(i)==0
%         b(i)=1;
%     end
% end

% for j=1:nr_training_bits
%     j
%     length(b)
%     for i=j+1:nr_training_bits
%         i
%         length(b)
%         for k=i+1:nr_training_bits
%             for l=k+1:nr_training_bits
%                 for m=l+1:nr_training_bits
%                     %for n=m+1:nr_training_bits
%                         int=zeros(2^5,1);
%                         for q=1:2^5
%                             b(j)=sign(2*q-(2^5+0.5));
%                             b(i)=sign(2*q-2^4*b(j)-(2^5+0.5));
%                             b(k)=sign(2*q-2^3*b(i)-2^4*b(j)-(2^5+0.5));
%                             b(l)=sign(2*q-2^2*b(k)-2^3*b(i)-2^4*b(j)-(2^5+0.5));
%                             b(m)=sign(2*q-2*b(l)-2^2*b(k)-2^3*b(i)-2^4*b(j)-(2^5+0.5));
%                             %b(n)=sign(q-b(m)-2*b(l)-2^2*b(k)-2^3*b(i)-2^4*b(j)-(2^5+0.5));
%                             o=xcorr(b);
%                             o(nr_training_bits)=0;
%                             int(q)=max(abs(o));
%                         end
%                         [val_min indice_min]=min(int);
%                         %length(b(j))
%                         %length(sign(indice_min-(2^3+0.5)))
%                         b(j)=sign(2*indice_min-(2^5+0.5));
%                         b(i)=sign(2*indice_min-2^4*b(j)-(2^5+0.5));
%                         b(k)=sign(2*indice_min-2^3*b(i)-2^4*b(j)-(2^5+0.5));
%                         b(l)=sign(2*indice_min-2^2*b(k)-2^3*b(i)-2^4*b(j)-(2^5+0.5));
%                         b(m)=sign(2*indice_min-2*b(l)-2^2*b(k)-2^3*b(i)-2^4*b(j)-(2^5+0.5));
%                         %b(n)=sign(indice_min-b(m)-2*b(l)-2^2*b(k)-2^3*b(i)-2^4*b(j)-(2^5+0.5));
%                         %         [val_min indice_min]=min([max(abs(int11)) max(abs(int12)) max(abs(int21)) max(abs(int22))]);
%                         %         b(j)=sign(indice_min-2.5);
%                         %         b(i)=sign(indice_min-b(j)-2.5);
%                     %end
%                 end
%             end
%         end
%     end
% end
% t=xcorr(b);
% plot(t)
% t(nr_training_bits)=0;
% max(abs(t))
end

