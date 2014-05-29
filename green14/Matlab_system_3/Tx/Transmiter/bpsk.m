function data_bpsk=bpsk(data)
%This function map the bits to be transmitted into BPSK symbols.
%         ^
%         |
%         |          (odd bit, even bit)
%     0   |   1
%  ---x---+---x--->  real part (I channel)
%         |
%         |   
%         |
%

data_bpsk=data;
pos_zeros= data==0;
data_bpsk(pos_zeros)=-1;

