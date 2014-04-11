function [outS,period]=LFSR(nTaps,polGen,init)
%polGen says which taps are connected, the greatest is the independent
%term, and the others are polinomic values of order nTaps-polGen(i).
%length(polGen)=N


estat=init;
period=1;
outS=zeros(2^nTaps-1,1);
N=length(polGen);
polGen=-nTaps+polGen;

	outS(period)=bitand(estat,1);  
    bitTmp=bitshift(estat,polGen(1));
    for i=2:N
        bitTmp=bitxor(bitTmp,bitshift(estat,polGen(i)));
    end
    bitCarry=bitand(bitTmp,1);
    estat=bitor(bitshift(estat,-1),bitshift(bitCarry,(nTaps-1)));
	period=period+1;
    
while(estat~=init)
	outS(period)=bitand(estat,1);  
    bitTmp=bitshift(estat,polGen(1));
    for i=2:N
        bitTmp=bitxor(bitTmp,bitshift(estat,polGen(i)));
    end
    bitCarry=bitand(bitTmp,1);
    estat=bitor(bitshift(estat,-1),bitshift(bitCarry,(nTaps-1)));
	period=period+1;
end
