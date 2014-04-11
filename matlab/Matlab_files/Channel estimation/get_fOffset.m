function v=get_fOffset(data)
% This function tries to remove the frequency offset between both
% oscillators. It takes into account that we will have these frequencies
% because they are leakage from the oscillator. And the TX power from the
% LO_TX is assumed to be higher than the noise.
%
%Input:
%  data    vector of the frequency response of the data.
%
%Result:
%  v       offset of the frequency between fRX and fTX
%          a negative value means that fTX is higher. See that the correction
%          should be of the opposite sign.


%Comments: 
%     1. Take care of the second maximum, because it can be close to the
%        first since we have not infinite resolution.
%     2. Take care also about the sign of v

nr_data=length(data);
[~, fLO_rx]=max(data(floor(nr_data*0.55):floor(nr_data*0.65)));
A(floor(nr_data*0.55)+fLO_recever-1-delta_f:floor(nr_data*0.55)+fLO_recever-1+delta_f)=0;
[~, fLO_tx]=max(A(floor(nr_data*0.55):floor(nr_data*0.65)));
v=-(fLO_rx-fLO_tx)/nr_data;
