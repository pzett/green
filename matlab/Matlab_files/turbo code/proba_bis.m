function [p] = proba_bis(input, ref, sigmaSquare_Epsilon,i)
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here
if i==0
    p=1/sqrt(2*pi*sigmaSquare_Epsilon)*exp(-((input-(2*ref-1))^2)/(2*sigmaSquare_Epsilon)); %ref =0 if the symbole is -1 and is equal to 1 if the symbole is +1
else
    if input==ref
        p=1-sigmaSquare_Epsilon;
    else
        p=sigmaSquare_Epsilon;
    end
end
end

