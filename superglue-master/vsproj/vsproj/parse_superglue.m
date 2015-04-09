file='superglue.log'
f=fopen(file,'r')
s=fgets(f)

figure,hold on

Threads = containers.Map();

while ~feof(f)
    s=fgets(f)
    parts = strread(s,'%s','delimiter',' ')
    assert(length(parts)==6)
    if ~isKey(Threads, parts{1})
        Threads(parts{1}) =length(Threads)*100;
    end
    tid =str2num(parts{1});
    tstart =str2num(parts{2});
    tend =str2num(parts{3});
    tlen =str2num(parts{4});
    name =[parts{5} parts{6}];
    y =Threads(parts{1});
    plot([tstart tend],[y y])
    text(tstart, y,name)
end

fclose(f)
