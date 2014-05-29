% if we need to test the channel... only plot the received spectrum
clear all
for i=1:1000
rx_data_tot=rx_60GHz(200000, 0, 18, 25e6, 0);
plotspectrum(rx_data_tot);
drawnow
end