% data=round(rand(6250,1));

temp=zeros(1,2*length(X));
for i1=1:length(X)
    i1
  temp(i1*2-1)=real(X(i1));
  temp(i1*2-0)=imag(X(i1));
end;

% Write to file (note the type of the data used)
fid=fopen('data_receive.dat','w');
fwrite(fid,temp,'double');
fclose(fid);