figure(2)
hold on
plot(reshape(data_received_equalized,1,size(data_received_equalized,1)*size(data_received_equalized,2)),'.b')
nr_bits=6;
    data_test_bin=zeros(2^nr_bits,nr_bits);
    for i=1:nr_bits
        a=zeros(2^nr_bits/(2^i),(2^(i-1)));
        b=ones(2^nr_bits/(2^i),(2^(i-1)));
        data_test_bin(:,i)=reshape([a;b],2^nr_bits,1);
    end
     data_test_bin=data_test_bin';
    data_test_bin=reshape(data_test_bin,1,size(data_test_bin, 1)*size(data_test_bin, 2));
    data_test=BEES64(data_test_bin);
    plot(data_test,'+k');
    legend('64 carriers','32 carriers','Mapping')
    grid on