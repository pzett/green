% write a file from the defined file frame
load('data_decoded')
sequence=data_decoded;
pointer=1;
a2=sequence(pointer:pointer-1+4);
pointer=pointer+4;
L1=sum(a2.*2.^(3:-1:0));
a=sequence(pointer:pointer-1+L1);
pointer=pointer+L1;
a=sum(a.*2.^(L1-1:-1:0));
b2=sequence(pointer:pointer-1+5);
L2=sum(b2.*2.^(4:-1:0));
pointer=pointer+5;
b=sequence(pointer:pointer-1+L2);
b=sum(b.*2.^(L2-1:-1:0));
pointer=pointer+L2;
y=sequence(pointer:pointer-1+8*a);
pointer=pointer+8*a;
x=sequence(pointer:pointer-1+8*b);
y=reshape(y,8,a).';
x=reshape(x,8,b).';
mat=ones(a,1)*(2.^(7:-1:0));
filename=sum(mat.*y,2);
mat=ones(b,1)*(2.^(7:-1:0));
data=sum(mat.*x,2);
str2=[native2unicode(filename.')];
fid=fopen(str2,'w+');
fwrite(fid,data);
fclose('all');