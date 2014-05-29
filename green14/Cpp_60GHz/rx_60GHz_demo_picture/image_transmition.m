% clear all;
% close all;
%% Transmitter side
%I=imread('Obama.png');
%II=imresize(I,[128 128]);
%inLine=(reshape(II,1,128*128));
% Write to file
%fid=fopen('data_toSend.dat','w');
%fwrite(fid,inLine,'double');
%fclose(fid);

%% Receiver side
% addpath('Data');
% itload('pack.it')
% itload('bits.it')
% itload('receivedBin.it')
% itload('errorVec.it')
itload('picture.it');
% Read file
% fid1=fopen('data_received.dat','r');
% Dec=fread(fid1,128*128,'int16');
% fclose(fid1);

Dec_reshaped=reshape(myPicture,128,128);
C=uint8(Dec_reshaped);
imshow(C);

usleep(1e9);
pause;

