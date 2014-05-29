%read a data file and create the data frame as defined
[filename, pathname,filterindex]=uigetfile('*.*','pick a file');
str=[pathname,filename];
fid=fopen(str);
data=fread(fid);
y=dec2bin(filename,8)-'0';
x=dec2bin(data,8)-'0';
a=dec2bin(size(y,1))-'0';
b=dec2bin(size(x,1))-'0';
a2=dec2bin(length(a),4)-'0';
b2=dec2bin(length(b),5)-'0';
data=[a2,a,b2,b,reshape(y.',1,8*size(y,1)),reshape(x.',1,8*size(x,1))];
save('data','data');