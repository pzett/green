nr_bits_bloc=4;
data_b=[data zeros(1,nr_bits_bloc-mod(length(data)-1,nr_bits_bloc)-1)];
data_bin=zeros(2^nr_bits_bloc,nr_bits_bloc);
for i=1:nr_bits_bloc
    a=zeros(2^nr_bits_bloc/(2^i),(2^(i-1)));
    b=ones(2^nr_bits_bloc/(2^i),(2^(i-1)));
    data_bin(:,i)=reshape([a;b],2^nr_bits_bloc,1);
end
data_b_r=reshape(data_b,nr_bits_bloc,length(data_b)/nr_bits_bloc);
mat=(2.^(nr_bits_bloc-1:-1:0))'*ones(1,length(data_b)/nr_bits_bloc);
a=sum(data_b_r.*mat)+1;
for i=1:2^nr_bits_bloc
    proba(i)=sum(a==i)/(length(data_b)/nr_bits_bloc);
end
%proba=p1.^sum(data_bin,2).*(1-p1).^(nr_bits_bloc-sum(data_bin,2));
proba_i=proba;
for i=1:2^nr_bits_bloc-1
    [p_min index_min]=min(proba);
    proba_temp=proba;
    proba_temp(index_min)=1;
    [p_min2 index_min2]=min(proba_temp);
    t(i,:)=[index_min index_min2];
    proba(index_min2)=proba(index_min2)+proba(index_min);
    proba(index_min)=1;
end
index=zeros(1,2^nr_bits_bloc);
code=2*ones(2^nr_bits_bloc);
for j=i:-1:1
    ind=index(t(j,2));
    if ind>0
        code(t(j,1),1:ind)=code(t(j,2),1:ind);
    end
    code(t(j,1),ind+1)=1;
    code(t(j,2),ind+1)=0;
    index(t(j,1))=ind+1;
    index(t(j,2))=ind+1;
end
pointer=1;
data_coded_info=zeros(1,length(data));
for i=1:(length(data_b)/nr_bits_bloc)
    index_bloc=a(i);
    L=index(index_bloc);
    coded_bloc=code(index_bloc,(1:L));
    data_coded_info(pointer:pointer+L-1)=coded_bloc;
    pointer=pointer+L;
end
data_coded_info=data_coded_info(1,1:pointer-1);
data_coded=(dec2bin(nr_bits_bloc,4)-'0');
for i=1:2^nr_bits_bloc
    data_coded=[data_coded (dec2bin(index(i),nr_bits_bloc)-'0') code(i,(1:index(i)))];
end
data_coded=[data_coded data_coded_info];
data_coded=[(dec2bin(length(data_coded)+24,24)-'0') data_coded];

nr_blocs=floor((length(data_coded)-1)/(100000-2*4))+1;
if nr_blocs>16
   fprintf('Error file too big \n')
    break
end
data_coded=[data_coded, generate_data(nr_blocs*(100000-2*4)-length(data_coded))];
nr_bloc_bin=dec2bin(nr_blocs-1,4)-'0';
entete=zeros(nr_blocs,8);
for i=1:nr_blocs
    q=dec2bin(i-1,4)-'0';
    entete(i,:)=[nr_bloc_bin,q];
end
data_coded=[entete reshape(data_coded,(100000-2*4),nr_blocs).'];
save('data_coded','data_coded','nr_blocs');