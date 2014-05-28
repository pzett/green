close all;
n=37020*5
fid1=fopen('received0.dat','r');
x=fread(fid1,2*n,'int16');
fclose(fid1);

xComplex= zeros(1,n);
count = 1;
for i=1:2:2*n
    xComplex(count) = complex(x(i),x(i+1));
    count = count+1;
end

figure()
plot(abs(xComplex));

figure; pwelch(xComplex);