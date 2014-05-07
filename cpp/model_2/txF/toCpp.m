% data=round(rand(6250,1));

% temp=zeros(1,2*length(data));
% for i1=1:length(pilot)
%     i1
%   temp(i1*2-1)=real(train_seq(i1));
%   temp(i1*2-0)=imag(train_seq(i1));
% end;

% Write to file (note the type of the data used)
fid=fopen('data.dat','w');
fwrite(fid,data,'double');
fclose(fid);