function main
filepath='692481080200_n50f34.png';
Pall=imread(filepath);
P=Pall(:,:,1);
%scan 
y=100;
line=P(y,:);
detectJump(line);

function m_val =initmap()
m_val = containers.Map(int32(1),[int32(-1) int32(1)]); %根据代码推到数字, %某代码属于奇数代码还是偶数代码
m_val(3211)=[0 0];  m_val(1123)=[0 1]; 
m_val(2221)=[1 0];  m_val(1222)=[1 1]; 
m_val(2122)=[2 0];  m_val(2212)=[2 1]; 
m_val(1411)=[3 0];  m_val(1141)=[3 1]; 
m_val(1132)=[4 0];  m_val(2311)=[4 1]; 
m_val(1231)=[5 0];  m_val(1321)=[5 1]; 
m_val(1114)=[6 0];  m_val(4111)=[6 1]; 
m_val(1312)=[7 0];  m_val(2131)=[7 1]; 
m_val(1213)=[8 0];  m_val(3121)=[8 1]; 
m_val(3112)=[9 0];  m_val(2113)=[9 1]; 


function data=detectJump(line)
%cunt how many pixels inside the 
data=[]; %num pixels of a value
flag=[]; %1 means beight value, 0 means dark value

d=0;
f=0;
line_seg=line<128;
for i=1:length(line_seg)
    if i==1 || line_seg(i)~=line_seg(i-1)
        if i>1
            data=[data d];
            flag=[flag f];
        end
        
        f=line_seg(i);
        d=1;
    else
        d=d+1;
    end
end
data2=data(2:length(data));
flag2=flag(2:length(flag));
unitSz=data2(1);
data3=data2/unitSz;
data3
assert(length(data3)==59)
%起始位3
i=1;
assert(all(data3(i-1+(1:3))==[1 1 1]));
i=i+3;
%左边6x4个
M=initmap();
for j=1:6
     data_t=data3(i-1+(j-1)*4+(1:4));
     m=M(data_t(1)*1000+data_t(2)*100+data_t(3)*10+data_t(4));
     fprintf('right [%d] %d %d %d %d (%d %d)\n',j, data_t,m)
end
i=i+24;
%中间5个
assert(all(data3(i-1+(1:5))));
i=i+5;
%you边6x4个
for j=1:6
     data_t=data3(i-1+(j-1)*4+(1:4));
     m=M(data_t(1)*1000+data_t(2)*100+data_t(3)*10+data_t(4));
     fprintf('right [%d] %d %d %d %d (%d %d)\n',j, data_t,m)
end
i=i+24;
%jieshu位3
assert(all(data3(i-1+(1:3))));
i=i+3;
assert(i==59+1);
