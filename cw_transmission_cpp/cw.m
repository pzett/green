function [ mod ] = cw( freq )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
% freq between -12.5 to 12.5 MHz


freq_samp=25e6;

v=freq/freq_samp;

a=exp(j*v*2*pi.*(1:10000));

mod=5000.*a;   %constant wave


end

