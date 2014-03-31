% harness.m
% For more information search for "README:" in harness.cpp.

% Randmomize the input data
number_of_elements=4+ceil(30*rand);
data_to_harness=randn(number_of_elements,1);

% Write to file (note the type of the data used)
fid=fopen('data_to_harness.dat','w');
fwrite(fid,data_to_harness,'float');
fclose(fid);

% Run the harness command
cmd_str=['./harness '];
cmd_str=[cmd_str,' --no_elements=',num2str(length(data_to_harness))];
cmd_str
system(cmd_str);

% Load data from file (note the type)
fid=fopen('data_from_harness.dat','r');
result_from_harness=fread(fid,length(data_to_harness),'float');
fclose(fid);

% Run the matlab implementation
result_from_matlab=square_elements_of_array(data_to_harness);

figure(1);
hold off
plot(result_from_harness,'ok');
hold on
plot(result_from_matlab,'-x');
legend('C++ implementation (harness)','matlab implementation');

