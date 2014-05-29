% close all;
% fid1=fopen('sent.dat','r');
% x=fread(fid1,82682*2,'short');
% fclose(fid1);
% 
% xComplex2= zeros(1,82682);
% count = 1;
% for i=1:2:82682*2
%     xComplex2(count) = complex(x(i),x(i+1));
%     count = count+1;
% en
% 
% xComplex2=conj(xComplex2)';


close all;
filename = 'sent';
ending = '.dat';

lengthPacket =37020;%60023;%37028;

I=imread('Obama.png');
II=imresize(I,[128 128]);

%pause();

totalMessage = [];
for m=0:1:4
    fid1=fopen([filename num2str(m) ending],'r');
    x=fread(fid1,lengthPacket*2,'short');
    fclose(fid1);

    xComplex1 = zeros(1,lengthPacket);
    count = 1;
    for i=1:2:lengthPacket*2
        xComplex1(count) = complex(x(i),x(i+1));
        count = count+1;
    end

    xComplex1=conj(xComplex1)';

    %disp(['max= ' num2str(max(abs(xComplex1)))]);
    %disp(['rms= ' num2str(rms(xComplex1))]);
    %disp(['p2a= ' num2str(peak2rms(xComplex1))]);
    
    totalMessage= [totalMessage; xComplex1];


    %figure()
    %plot(real(xComplex1));

    %figure();
    %semilogy(abs(fft(xComplex1)));

end

%totalrms= rms(totalMessage);


figure()
subplot(2,1,1)
plot(real(totalMessage)); 
title('Transmitted Data');
xlabel('n');
ylabel('Amplitude');
subplot(2,1,2)
semilogy(linspace(0,1,lengthPacket),abs(fft(totalMessage(1:1:lengthPacket))));
title('Spectrum of Transmitted Data');
xlabel('f (normalized frequency');
ylabel('Gain (dB)');
figure()
imshow(II);
pause();
