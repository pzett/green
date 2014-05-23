clear all;
close all;
%% Transmitter side
I=imread('Obama.png');
II=imresize(I,[128 128]);
%imshow(II);
%pause()
inLine=int16(reshape(II,1,128*128));
% Write to file
fid=fopen('data_toSend.dat','w');
fwrite(fid,inLine,'int16');
fclose(fid);

%% Receiver side
% % Read file
% fid1=fopen('data_received.dat','r');
% Dec=fread(fid1,128*128,'int16');
% fclose(fid1);
% 
% Dec_reshaped=reshape(Dec,128,128);
% C=uint8(Dec_reshaped);
% imshow(C);