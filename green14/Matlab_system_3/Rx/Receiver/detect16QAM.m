function bhat = detect16QAM(r)


bhat = zeros(1,length(r)*4);
k=1;
for i=1:length(r)
    signR=sign(real(r(i)));
    signI=sign(imag(r(i)));

    if( signR==+1)
        bhat((i-1)*4+1)=0;
       if(signI==+1)
           bhat((i-1)*4+3)=0;
           if(floor(abs(real(r(i))-1))==0)
              bhat((i-1)*4+2)=1;
           else
              bhat((i-1)*4+2)=0;
           end
           if (floor(abs(imag(r(i))-1))==0)
              bhat(i*4)=1;
           else
              bhat(i*4)=0;
           end
       else
           bhat((i-1)*4+3)=1;
           if(floor(abs(real(r(i))-1))==0)
              bhat((i-1)*4+2)=1;
           else
              bhat((i-1)*4+2)=0;
           end
           if (floor(abs(imag(r(i))+1))==0)
              bhat(i*4)=1;
           else
              bhat(i*4)=0;
           end
       end      
    else
        bhat((i-1)*4+1)=1;
       if(signI==+1)
           bhat((i-1)*4+3)=0;
           if(floor(abs(real(r(i))+1))==0)
              bhat((i-1)*4+2)=1;
           else
              bhat((i-1)*4+2)=0;
           end
           if (floor(abs(imag(r(i))-1))==0)
              bhat(i*4)=1;
           else
              bhat(i*4)=0;
           end
       else
           bhat((i-1)*4+3)=1;
           if(floor(abs(real(r(i))+1))==0)
              bhat((i-1)*4+2)=1;
           else
              bhat((i-1)*4+2)=0;
           end
           if (floor(abs(imag(r(i))+1))==0)
              bhat(i*4)=1;
           else
              bhat(i*4)=0;
           end
       end
    end
end