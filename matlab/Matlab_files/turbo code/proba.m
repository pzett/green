function [p] = proba(input, ref, sigmaSquare_Epsilon,i,mask)
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here
if i==0
%     input
%     ref
%     abs((input-(2*ref-1))).^2
    p=(-1/2*log(2*pi*sigmaSquare_Epsilon)-(abs((input-(2*ref-1))).^2)/(2*sigmaSquare_Epsilon)); %.*(mask; %ref =0 if the symbole is -1 and is equal to 1 if the symbole is +1
%p
 % p=-(abs((input-(2*ref-1))).^2)/(2*sigmaSquare_Epsilon); %ref =0 if the symbole is -1 and is equal to 1 if the symbole is +1

else
    if input==0
        p=0;
    else
    test=input~=ref;
    p=(-1).^test*log((1-sigmaSquare_Epsilon)/sigmaSquare_Epsilon);
    end
end
end

