% Map bits following 128-QAM constellation
function [ data_map ] = QAM128( data )

f=1;
data_map = zeros(1, length(data)/7);
%64 IV
for v=1:7:length(data)-6 
    if f<=length(data)/7
    if data(v)==0 && data(v+1)==0 && data(v+2)==0 && data(v+3)==0
        if data(v+4)==1 && data(v+5)==0 && data(v+6)==0
            data_map(f)=-7+7*1i; f=f+1;
        elseif data(v+4)==1 && data(v+5)==0 && data(v+6)==1
            data_map(f)=-7+5*1i; f=f+1;
        elseif data(v+4)==1 && data(v+5)==1 && data(v+6)==1
            data_map(f)=-7+3*1i; f=f+1;
        elseif data(v+4)==1 && data(v+5)==1 && data(v+6)==0
            data_map(f)=-7+1*1i; f=f+1;
        end
      
    end
    if data(v)==0 && data(v+1)==0 && data(v+2)==0 && data(v+3)==1
        if data(v+4)==1 && data(v+5)==0 && data(v+6)==0
            data_map(f)=-5+7*1i; f=f+1;
        elseif data(v+4)==1 && data(v+5)==0 && data(v+6)==1
            data_map(f)=-5+5*1i; f=f+1;
        elseif data(v+4)==1 && data(v+5)==1 && data(v+6)==1
            data_map(f)=-5+3*1i; f=f+1;
        elseif data(v+4)==1 && data(v+5)==1 && data(v+6)==0
            data_map(f)=-5+1*1i; f=f+1;
        end
       
    end
    if data(v)==0 && data(v+1)==0 && data(v+2)==1 && data(v+3)==1
        if data(v+4)==1 && data(v+5)==0 && data(v+6)==0
            data_map(f)=-3+7*1i; f=f+1;
        elseif data(v+4)==1 && data(v+5)==0 && data(v+6)==1
            data_map(f)=-3+5*1i; f=f+1;
        elseif data(v+4)==1 && data(v+5)==1 && data(v+6)==1
            data_map(f)=-3+3*1i; f=f+1;
        elseif data(v+4)==1 && data(v+5)==1 && data(v+6)==0
            data_map(f)=-3+1*1i; f=f+1;
        end
     
    end
    if data(v)==0 && data(v+1)==0 && data(v+2)==1 && data(v+3)==0
        if data(v+4)==1 && data(v+5)==0 && data(v+6)==0
            data_map(f)=-1+7*1i; f=f+1;
        elseif data(v+4)==1 && data(v+5)==0 && data(v+6)==1
            data_map(f)=-1+5*1i; f=f+1;
        elseif data(v+4)==1 && data(v+5)==1 && data(v+6)==1
            data_map(f)=-1+3*1i; f=f+1;
        elseif data(v+4)==1 && data(v+5)==1 && data(v+6)==0
            data_map(f)=-1+1*1i; f=f+1;
        end
       
    end
    %64 I
    if data(v)==0 && data(v+1)==1 && data(v+2)==1 && data(v+3)==0
        if data(v+4)==1 && data(v+5)==0 && data(v+6)==0
            data_map(f)=1+7*1i; f=f+1;
        elseif data(v+4)==1 && data(v+5)==0 && data(v+6)==1
            data_map(f)=1+5*1i; f=f+1;
        elseif data(v+4)==1 && data(v+5)==1 && data(v+6)==1
            data_map(f)=1+3*1i; f=f+1;
        elseif data(v+4)==1 && data(v+5)==1 && data(v+6)==0
            data_map(f)=1+1*1i; f=f+1;
        end
      
    end
    
    if data(v)==0 && data(v+1)==1 && data(v+2)==1 && data(v+3)==1
        if data(v+4)==1 && data(v+5)==0 && data(v+6)==0
            data_map(f)=3+7*1i; f=f+1;
        elseif data(v+4)==1 && data(v+5)==0 && data(v+6)==1
            data_map(f)=3+5*1i; f=f+1;
        elseif data(v+4)==1 && data(v+5)==1 && data(v+6)==1
            data_map(f)=3+3*1i; f=f+1;
        elseif data(v+4)==1 && data(v+5)==1 && data(v+6)==0
            data_map(f)=3+1*1i; f=f+1;
        end
        
    end
    
    if data(v)==0 && data(v+1)==1 && data(v+2)==0 && data(v+3)==1
        if data(v+4)==1 && data(v+5)==0 && data(v+6)==0
            data_map(f)=5+7*1i;f=f+1;
        elseif data(v+4)==1 && data(v+5)==0 && data(v+6)==1
            data_map(f)=5+5*1i;f=f+1;
        elseif data(v+4)==1 && data(v+5)==1 && data(v+6)==1
            data_map(f)=5+3*1i;f=f+1;
        elseif data(v+4)==1 && data(v+5)==1 && data(v+6)==0
            data_map(f)=5+1*1i; f=f+1;
        end
        
    end
    
    if data(v)==0 && data(v+1)==1 && data(v+2)==0 && data(v+3)==0
        if data(v+4)==1 && data(v+5)==0 && data(v+6)==0
            data_map(f)=7+7*1i;f=f+1;
        elseif data(v+4)==1 && data(v+5)==0 && data(v+6)==1
            data_map(f)=7+5*1i;f=f+1;
        elseif data(v+4)==1 && data(v+5)==1 && data(v+6)==1
            data_map(f)=7+3*1i;f=f+1;
        elseif data(v+4)==1 && data(v+5)==1 && data(v+6)==0
            data_map(f)=7+1*1i; f=f+1;
        end
        
    end
    % 64 III
    if data(v)==0 && data(v+1)==0 && data(v+2)==0 && data(v+3)==0
        if data(v+4)==0 && data(v+5)==1 && data(v+6)==0
            data_map(f)=-7-1*1i; f=f+1;
        elseif data(v+4)==0 && data(v+5)==1 && data(v+6)==1
            data_map(f)=-7-3*1i; f=f+1;
        elseif data(v+4)==0 && data(v+5)==0 && data(v+6)==1
            data_map(f)=-7-5*1i; f=f+1;
        elseif data(v+4)==0 && data(v+5)==0 && data(v+6)==0
            data_map(f)=-7-7*1i; f=f+1;
        end
    
    end
    if data(v)==0 && data(v+1)==0 && data(v+2)==0 && data(v+3)==1
        if data(v+4)==0 && data(v+5)==1 && data(v+6)==0
            data_map(f)=-5-1*1i; f=f+1;
        elseif data(v+4)==0 && data(v+5)==1 && data(v+6)==1
            data_map(f)=-5-3*1i; f=f+1;
        elseif data(v+4)==0 && data(v+5)==0 && data(v+6)==1
            data_map(f)=-5-5*1i; f=f+1;
        elseif data(v+4)==0 && data(v+5)==0 && data(v+6)==0
            data_map(f)=-5-7*1i; f=f+1;
        end
      
    end
    if data(v)==0 && data(v+1)==0 && data(v+2)==1 && data(v+3)==1
        if data(v+4)==0 && data(v+5)==1 && data(v+6)==0
            data_map(f)=-3-1*1i; f=f+1;
        elseif data(v+4)==0 && data(v+5)==1 && data(v+6)==1
            data_map(f)=-3-3*1i; f=f+1;
        elseif data(v+4)==0 && data(v+5)==0 && data(v+6)==1
            data_map(f)=-3-5*1i; f=f+1;
        elseif data(v+4)==0 && data(v+5)==0 && data(v+6)==0
            data_map(f)=-3-7*1i; f=f+1;
        end
       
    end
    if data(v)==0 && data(v+1)==0 && data(v+2)==1 && data(v+3)==0
        if data(v+4)==0 && data(v+5)==1 && data(v+6)==0
            data_map(f)=-1-1*1i; f=f+1;
        elseif data(v+4)==0 && data(v+5)==1 && data(v+6)==1
            data_map(f)=-1-3*1i; f=f+1;
        elseif data(v+4)==0 && data(v+5)==0 && data(v+6)==1
            data_map(f)=-1-5*1i; f=f+1;
        elseif data(v+4)==0 && data(v+5)==0 && data(v+6)==0
            data_map(f)=-1-7*1i; f=f+1;
        end
        
    end
    
    %64 II
    
    if data(v)==0 && data(v+1)==1 && data(v+2)==1 && data(v+3)==0
        if data(v+4)==0 && data(v+5)==1 && data(v+6)==0
            data_map(f)=1-1*1i; f=f+1;
        elseif data(v+4)==0 && data(v+5)==1 && data(v+6)==1
            data_map(f)=1-3*1i; f=f+1;
        elseif data(v+4)==0 && data(v+5)==0 && data(v+6)==1
            data_map(f)=1-5*1i; f=f+1;
        elseif data(v+4)==0 && data(v+5)==0 && data(v+6)==0
            data_map(f)=1-7*1i; f=f+1;
        end
        
    end
    if data(v)==0 && data(v+1)==1 && data(v+2)==1 && data(v+3)==1
        if data(v+4)==0 && data(v+5)==1 && data(v+6)==0
            data_map(f)=3-1*1i; f=f+1;
        elseif data(v+4)==0 && data(v+5)==1 && data(v+6)==1
            data_map(f)=3-3*1i; f=f+1;
        elseif data(v+4)==0 && data(v+5)==0 && data(v+6)==1
            data_map(f)=3-5*1i; f=f+1;
        elseif data(v+4)==0 && data(v+5)==0 && data(v+6)==0
            data_map(f)=3-7*1i; f=f+1;
        end
        
    end
    if data(v)==0 && data(v+1)==1 && data(v+2)==0 && data(v+3)==1
        if data(v+4)==0 && data(v+5)==1 && data(v+6)==0
            data_map(f)=5-1*1i; f=f+1;
        elseif data(v+4)==0 && data(v+5)==1 && data(v+6)==1
            data_map(f)=5-3*1i; f=f+1;
        elseif data(v+4)==0 && data(v+5)==0 && data(v+6)==1
            data_map(f)=5-5*1i; f=f+1;
        elseif data(v+4)==0 && data(v+5)==0 && data(v+6)==0
            data_map(f)=5-7*1i; f=f+1;
        end
       
    end
    if data(v)==0 && data(v+1)==1 && data(v+2)==0 && data(v+3)==0
        if data(v+4)==0 && data(v+5)==1 && data(v+6)==0
            data_map(f)=7-1*1i; f=f+1;
        elseif data(v+4)==0 && data(v+5)==1 && data(v+6)==1
            data_map(f)=7-3*1i; f=f+1;
        elseif data(v+4)==0 && data(v+5)==0 && data(v+6)==1
            data_map(f)=7-5*1i; f=f+1;
        elseif data(v+4)==0 && data(v+5)==0 && data(v+6)==0
            data_map(f)=7-7*1i; f=f+1;
        end
       
    end
    
    %Surroundings IV
    if data(v)==1 && data(v+1)==0 && data(v+2)==1
        if data(v+3)==0 && data(v+4)==1 && data(v+5)==1 && data(v+6)==0
            data_map(f)=-7+9*1i; f=f+1;
        elseif data(v+3)==1 && data(v+4)==1 && data(v+5)==1 && data(v+6)==0
            data_map(f)=-5+9*1i; f=f+1;
        elseif data(v+3)==1 && data(v+4)==1 && data(v+5)==0 && data(v+6)==0
            data_map(f)=-3+9*1i; f=f+1;
        elseif data(v+3)==0 && data(v+4)==1 && data(v+5)==0 && data(v+6)==0
            data_map(f)=-1+9*1i; f=f+1;
            
        elseif data(v+3)==0 && data(v+4)==1 && data(v+5)==1 && data(v+6)==1
            data_map(f)=-7+11*1i; f=f+1;
        elseif data(v+3)==1 && data(v+4)==1 && data(v+5)==1 && data(v+6)==1
            data_map(f)=-5+11*1i; f=f+1;
        elseif data(v+3)==1 && data(v+4)==1 && data(v+5)==0 && data(v+6)==1
            data_map(f)=-3+11*1i; f=f+1;
        elseif data(v+3)==0 && data(v+4)==1 && data(v+5)==0 && data(v+6)==1
            data_map(f)=-1+11*1i; f=f+1;
            
        elseif data(v+3)==0 && data(v+4)==0 && data(v+5)==1 && data(v+6)==0
            data_map(f)=-7-9*1i; f=f+1;
        elseif data(v+3)==1 && data(v+4)==0 && data(v+5)==1 && data(v+6)==0
            data_map(f)=-5-9*1i; f=f+1;
        elseif data(v+3)==1 && data(v+4)==0 && data(v+5)==0 && data(v+6)==0
            data_map(f)=-3-9*1i; f=f+1;
        elseif data(v+3)==0 && data(v+4)==0 && data(v+5)==0 && data(v+6)==0
            data_map(f)=-1-9*1i; f=f+1;
            
        elseif data(v+3)==0 && data(v+4)==0 && data(v+5)==1 && data(v+6)==1
            data_map(f)=-7-11*1i; f=f+1;
        elseif data(v+3)==1 && data(v+4)==0 && data(v+5)==1 && data(v+6)==1
            data_map(f)=-5-11*1i; f=f+1;
        elseif data(v+3)==1 && data(v+4)==0 && data(v+5)==0 && data(v+6)==1
            data_map(f)=-3-11*1i; f=f+1;
        elseif data(v+3)==0 && data(v+4)==0 && data(v+5)==0 && data(v+6)==1
            data_map(f)=-1-11*1i; f=f+1;
        end
        
    end
    
    if data(v)==1 && data(v+1)==0 && data(v+2)==0 && data(v+3)==1
        if data(v+4)==1 && data(v+5)==0 && data(v+6)==0
            data_map(f)=-11+7*1i; f=f+1;
        elseif data(v+4)==1 && data(v+5)==0 && data(v+6)==1
            data_map(f)=-11+5*1i; f=f+1;
        elseif data(v+4)==1 && data(v+5)==1 && data(v+6)==1
            data_map(f)=-11+3*1i; f=f+1;
        elseif data(v+4)==1 && data(v+5)==1 && data(v+6)==0
            data_map(f)=-11+1*1i; f=f+1;
        end
       
    end
    if data(v)==1 && data(v+1)==0 && data(v+2)==0 && data(v+3)==0
        if data(v+4)==1 && data(v+5)==0 && data(v+6)==0
            data_map(f)=-9+7*1i; f=f+1;
        elseif data(v+4)==1 && data(v+5)==0 && data(v+6)==1
            data_map(f)=-9+5*1i; f=f+1;
        elseif data(v+4)==1 && data(v+5)==1 && data(v+6)==1
            data_map(f)=-9+3*1i; f=f+1;
        elseif data(v+4)==1 && data(v+5)==1 && data(v+6)==0
            data_map(f)=-9+1*1i; f=f+1;
        end
       
    end
    %Surroundings I
    if data(v)==1 && data(v+1)==1 && data(v+2)==1
        if data(v+3)==0 && data(v+4)==1 && data(v+5)==0 && data(v+6)==0
            data_map(f)=1+9*1i; f=f+1;
        elseif data(v+3)==1 && data(v+4)==1 && data(v+5)==0 && data(v+6)==0
            data_map(f)=3+9*1i; f=f+1;
        elseif data(v+3)==1 && data(v+4)==1 && data(v+5)==1 && data(v+6)==0
            data_map(f)=5+9*1i; f=f+1;
        elseif data(v+3)==0 && data(v+4)==1 && data(v+5)==1 && data(v+6)==0
            data_map(f)=7+9*1i; f=f+1;
        elseif data(v+3)==0 && data(v+4)==1 && data(v+5)==0 && data(v+6)==1
            data_map(f)=1+11*1i; f=f+1;
        elseif data(v+3)==1 && data(v+4)==1 && data(v+5)==0 && data(v+6)==1
            data_map(f)=3+11*1i; f=f+1;
        elseif data(v+3)==1 && data(v+4)==1 && data(v+5)==1 && data(v+6)==1
            data_map(f)=5+11*1i; f=f+1;
        elseif data(v+3)==0 && data(v+4)==1 && data(v+5)==1 && data(v+6)==1
            data_map(f)=7+11*1i; f=f+1;
            
        elseif data(v+3)==0 && data(v+4)==0 && data(v+5)==0 && data(v+6)==0
            data_map(f)=1-9*1i; f=f+1;
        elseif data(v+3)==1 && data(v+4)==0 && data(v+5)==0 && data(v+6)==0
            data_map(f)=3-9*1i; f=f+1;
        elseif data(v+3)==1 && data(v+4)==0 && data(v+5)==1 && data(v+6)==0
            data_map(f)=5-9*1i; f=f+1;
        elseif data(v+3)==0 && data(v+4)==0 && data(v+5)==1 && data(v+6)==0
            data_map(f)=7-9*1i; f=f+1;
        elseif data(v+3)==0 && data(v+4)==0 && data(v+5)==0 && data(v+6)==1
            data_map(f)=1-11*1i; f=f+1;
        elseif data(v+3)==1 && data(v+4)==0 && data(v+5)==0 && data(v+6)==1
            data_map(f)=3-11*1i; f=f+1;
        elseif data(v+3)==1 && data(v+4)==0 && data(v+5)==1 && data(v+6)==1
            data_map(f)=5-11*1i; f=f+1;
        elseif data(v+3)==0 && data(v+4)==0 && data(v+5)==1 && data(v+6)==1
            data_map(f)=7-11*1i; f=f+1;
        end
        
    end
    
    if data(v)==1 && data(v+1)==1 && data(v+2)==0 && data(v+3)==0
        if data(v+4)==1 && data(v+5)==0 && data(v+6)==0
            data_map(f)=9+7*1i; f=f+1;
        elseif data(v+4)==1 && data(v+5)==0 && data(v+6)==1
            data_map(f)=9+5*1i; f=f+1;
        elseif data(v+4)==1 && data(v+5)==1 && data(v+6)==1
            data_map(f)=9+3*1i; f=f+1;
        elseif data(v+4)==1 && data(v+5)==1 && data(v+6)==0
            data_map(f)=9+1*1i; f=f+1;
        end
       
    end
    if data(v)==1 && data(v+1)==1 && data(v+2)==0 && data(v+3)==1
        if data(v+4)==1 && data(v+5)==0 && data(v+6)==0
            data_map(f)=11+7*1i; f=f+1;
        elseif data(v+4)==1 && data(v+5)==0 && data(v+6)==1
            data_map(f)=11+5*1i; f=f+1;
        elseif data(v+4)==1 && data(v+5)==1 && data(v+6)==1
            data_map(f)=11+3*1i; f=f+1;
        elseif data(v+4)==1 && data(v+5)==1 && data(v+6)==0
            data_map(f)=11+1*1i; f=f+1;
        end
        
    end
    
    %Surroundings II
    
    if data(v)==1 && data(v+1)==1 && data(v+2)==0 && data(v+3)==0
        if data(v+4)==0 && data(v+5)==1 && data(v+6)==0
            data_map(f)=9-1*1i; f=f+1;
        elseif data(v+4)==0 && data(v+5)==1 && data(v+6)==1
            data_map(f)=9-3*1i; f=f+1;
        elseif data(v+4)==0 && data(v+5)==0 && data(v+6)==1
            data_map(f)=9-5*1i; f=f+1;
        elseif data(v+4)==0 && data(v+5)==0 && data(v+6)==0
            data_map(f)=9-7*1i; f=f+1;
        end
        
    end
    if data(v)==1 && data(v+1)==1 && data(v+2)==0 && data(v+3)==1
        if data(v+4)==0 && data(v+5)==1 && data(v+6)==0
            data_map(f)=11-1*1i; f=f+1;
        elseif data(v+4)==1 && data(v+5)==0 && data(v+6)==1
            data_map(f)=11-3*1i; f=f+1;
        elseif data(v+4)==1 && data(v+5)==1 && data(v+6)==1
            data_map(f)=11-5*1i; f=f+1;
        elseif data(v+4)==1 && data(v+5)==1 && data(v+6)==0
            data_map(f)=11-7*1i; f=f+1;
        end
        
    end
    
    %Surroundings III
    
    if data(v)==1 && data(v+1)==0 && data(v+2)==0 && data(v+3)==1
        if data(v+4)==0 && data(v+5)==1 && data(v+6)==0
            data_map(f)=-11-1*1i; f=f+1;
        elseif data(v+4)==0 && data(v+5)==1 && data(v+6)==1
            data_map(f)=-11-3*1i; f=f+1;
        elseif data(v+4)==0 && data(v+5)==0 && data(v+6)==1
            data_map(f)=-11-5*1i; f=f+1;
        elseif data(v+4)==0 && data(v+5)==0 && data(v+6)==0
            data_map(f)=-11-7*1i; f=f+1;
        end
       
    end
    if data(v)==1 && data(v+1)==0 && data(v+2)==0 && data(v+3)==0
        if data(v+4)==0 && data(v+5)==1 && data(v+6)==0
            data_map(f)=-9-1*1i; f=f+1;
        elseif data(v+4)==0 && data(v+5)==1 && data(v+6)==1
            data_map(f)=-9-3*1i; f=f+1;
        elseif data(v+4)==0 && data(v+5)==0 && data(v+6)==1
            data_map(f)=-9-5*1i; f=f+1;
        elseif data(v+4)==0 && data(v+5)==0 && data(v+6)==0
            data_map(f)=-9-7*1i; f=f+1;
        end
       
    end
    end
end
end
