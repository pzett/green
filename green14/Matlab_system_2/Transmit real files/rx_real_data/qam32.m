% map bits with 32-QAM constellation
function [ d ] = qam32( data )

d=zeros(1, length(data)/5);
k=1;
for i=1:5:length(data)-4
    if data(i)==1&& data(i+1)==1 %first two bits 11
        if data(i+2)==1 && data(i+3)==1 && data(i+4)==1
            d(k) = (3-1i);%/sqrt(10);
        end
        if data(i+2)==0&& data(i+3)==1 && data(i+4)==1
            d(k) = (1-1i);%/sqrt(2);
        end
        if data(i+2)==0&& data(i+3)==0 && data(i+4)==1
            d(k) = (1+1i);%/sqrt(2);
        end
        if data(i+2)==1&& data(i+3)==0 && data(i+4)==1
            d(k) = (3+1i);%/sqrt(10);
        end
        if data(i+2)==0&& data(i+3)==0 && data(i+4)==0
            d(k) = (1+3*1i);%/sqrt(10);
        end
        if data(i+2)==1&& data(i+3)==0 && data(i+4)==0
            d(k) = (3+3*1i);%/sqrt(18);
        end
        if data(i+2)==0&& data(i+3)==1 && data(i+4)==0
            d(k) = (1-3*1i);%/sqrt(10);
        end
        if data(i+2)==1&& data(i+3)==1 && data(i+4)==0
            d(k) = (3-3*1i);%/sqrt(10);
        end
        k=k+1;
    end
    
    if data(i)==1 && data(i+1)==0 %first two bits are 10
        if data(i+2)==1 && data(i+3)==1 && data(i+4)==1
            d(k) = (5-1i);%/sqrt(26);
        end
        if data(i+2)==0&& data(i+3)==1 && data(i+4)==1
            d(k) = (1-5*1i);%/sqrt(26);
        end
        if data(i+2)==0&& data(i+3)==0 && data(i+4)==1
            d(k) = (1+5*1i);%/sqrt(26);
        end
        if data(i+2)==1&& data(i+3)==0 && data(i+4)==1
            d(k) =(5+1i);%/sqrt(26);
        end
        if data(i+2)==0&& data(i+3)==0 && data(i+4)==0
            d(k) = (3+5*1i);%/sqrt(34);
        end
        if data(i+2)==1&& data(i+3)==0 && data(i+4)==0
            d(k) = (5+3*1i);%/sqrt(34);
        end
        if data(i+2)==0&& data(i+3)==1 && data(i+4)==0
            d(k) = (3-5*1i);%/sqrt(34);
        end
        if data(i+2)==1&& data(i+3)==1 && data(i+4)==0
            d(k) = (5-3*1i);%/sqrt(34);
        end
        k=k+1;
    end
    
    if data (i)==0 && data(i+1)==1 %first two bits are 01
        if data(i+2)==1 && data(i+3)==1 && data(i+4)==1
            d(k) = (-3-1i);%/sqrt(10);
        end
        if data(i+2)==0&& data(i+3)==1 && data(i+4)==1
            d(k) = (-1-1i);%/sqrt(2);
        end
        if data(i+2)==0&& data(i+3)==0 && data(i+4)==1
            d(k) = (-1+1i);%/sqrt(2);
        end
        if data(i+2)==1&& data(i+3)==0 && data(i+4)==1
            d(k) = (-3+1i);%/sqrt(10);
        end
        if data(i+2)==0&& data(i+3)==0 && data(i+4)==0
            d(k) = (-1+3*1i);%/sqrt(10);
        end
        if data(i+2)==1&& data(i+3)==0 && data(i+4)==0
            d(k) = (-3+3*1i);%/sqrt(18);
        end
        if data(i+2)==0&& data(i+3)==1 && data(i+4)==0
            d(k) = (-1-3*1i);%/sqrt(10 );
        end
        if data(i+2)==1&& data(i+3)==1 && data(i+4)==0
            d(k) = (-3-3*1i);%/sqrt(18);
        end
        k=k+1;
    end
    
    
    if data (i)==0 && data(i+1)==0 %first two bits are 00
        if data(i+2)==1 && data(i+3)==1 && data(i+4)==1
            d(k) = (-5-1i);%/sqrt(26);
        end
        if data(i+2)==0&& data(i+3)==1 && data(i+4)==1
            d(k) = (-1-5i);%/sqrt(26);
        end
        if data(i+2)==0&& data(i+3)==0 && data(i+4)==1
            d(k) = (-1+5*1i);%/sqrt(26);
        end
        if data(i+2)==1&& data(i+3)==0 && data(i+4)==1
            d(k) = (-5+1i);%/sqrt(26);
        end
        if data(i+2)==0&& data(i+3)==0 && data(i+4)==0
            d(k) = (-3+5*1i);%/sqrt(34);
        end
        if data(i+2)==1&& data(i+3)==0 && data(i+4)==0
            d(k) = (-5+3*1i);%/sqrt(34);
        end
        if data(i+2)==0&& data(i+3)==1 && data(i+4)==0
            d(k) = (-3-5*1i);%/sqrt(34);
        end
        if data(i+2)==1&& data(i+3)==1 && data(i+4)==0
            d(k) = (-5-3*1i);%/sqrt(34);
        end
   
        k=k+1;
        
    end
 end

