function [ data_DFT ] = DFT_precode( data, N, command )
%DFT_precode Do a DFT precoding of mapped (QPSK) data vector (before S/P and IFFT)
%   The purpose of the DFT precoding is to reduce the deterioration of the signal when clipping it. 
%   It should achieve something like 1.5 dB increase in signal quality for arbitrary clipping levels.
%   I e, it should never be a problem to use DFT-precoding. Drawback is in increased processing time.
%   INPUT
%	data - the modulated data vector (QPSK, M-QAM, whatever)
%	N - the size of the DFT window
%   command - 'encode' or 'decode'
%    ------------!!!MAKE SURE THAT THE NUMBER OF ELEMENTS IN DATA IS AN INTEGER MULTIPLE OF N!!!------------

L = max(size(data));
data_DFT = zeros(L,1);
m_P = (0:N-1)';
n_P = 0:N-1;
P = 1/sqrt(N)*exp(-1i*2*pi*(m_P*n_P)/N);
windows = floor(L/N);       % nr of DFT precoded windows with chosen N
remainder = mod(L,N);       % nr of elements left alone!

if ne(remainder,0)
    disp('Poorly chosen DFT size, choose a new N')
    keyboard
end

% this is the encoder
if strcmp(command,'encode')
    for window = 0:windows-1
        data_DFT(1 + window*N:N + window*N) = P*(data(1 + window*N: N + window*N));
    end
    
% this is the decoder
elseif strcmp(command,'decode')
    for window = 0:windows-1
        data_DFT(1 + window*N:N + window*N) = P\data(1 + window*N: N + window*N);
    end
    
else
    disp('Choose encode or decode for DFT_precode')
    keyboard
end



end
