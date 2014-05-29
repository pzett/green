clear all

nr_data_bits=18760;

%Generate random data:
bin_data=(rand(1, nr_data_bits) > .5);

%Save generated data to file:

fid=fopen('data.dat','w');
fwrite(fid,bin_data,'double');
fclose(fid);

