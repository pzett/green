function [ nr_used_car, nr_pilot_car] = check_pattern( car_pat, pil_pat, nr_car )
%UNTITLED3 Summary of this function goes here
%   Detailed explanation goes here
total_car=[car_pat, pil_pat];
if max(total_car)<=nr_car && min(total_car)>0
     if  isempty(intersect(car_pat, pil_pat))
%         disp('OK')
    else
        disp('Overlappig of carriers')
    end
 else
    disp('Revise the allocation pattern and length')
end
nr_used_car=length(car_pat);
nr_pilot_car=length(pil_pat);
end