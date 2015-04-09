file='superglue.log'
f=fopen(file,'r')
s=fgets(f)

figure,hold on

Threads = containers.Map();
Colors =containers.Map();
AllColors='rgbk';
TOTAL_time_start=-1;
TOTAL_time_end=-1;
        
while ~feof(f)
    s=fgets(f);
    parts = strread(s,'%s','delimiter',' ');
    assert(length(parts)==6)
    if ~isKey(Threads, parts{1})
        t =0
        t.y =length(Threads)*100;
        t.busy =0;
        Threads(parts{1}) =t;
    end
    
    if ~isKey(Colors, parts{5})
        id =mod(length(Colors),length(AllColors))+1;
        Colors(parts{5}) =AllColors(id);
    end

    tid =str2num(parts{1});
    tstart =str2num(parts{2});
    tend =str2num(parts{3});
    tlen =str2num(parts{4});
    name =[parts{5} parts{6}];
    y =Threads(parts{1}).y;
    plot([tstart tend],[y y],Colors(parts{5}))
    
    if( TOTAL_time_start<0 )
        TOTAL_time_start=tstart;
    else
        TOTAL_time_start=min( tstart, TOTAL_time_start);
    end
    
     if( TOTAL_time_end<0 )
        TOTAL_time_end=tstart;
    else
        TOTAL_time_end=max( tstart, tend);
     end
    
     t =Threads(parts{1});
     t.busy = t.busy +tend -tstart;
     Threads(parts{1}) =t;
    %text(tstart, y,name)
end

fclose(f)

keys(Colors)
values(Colors)
 
total_time =TOTAL_time_end -TOTAL_time_start;
for i=1:length(Threads)
    keyss =keys(Threads);
    key=keyss{i}
    t =Threads(key);
    ratio=t.busy/total_time
end