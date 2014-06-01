 //声明用于保存链接的数组 和 用于分析链接的正则表达式
  //因为我仅想抓取墙外的中文版chrome文档
  //所以这个正则非常简单，并且不一定能在别的网站上使用
  var aDuiLie = [ ] , aP = /<a href="[^.#][^"/#:]+?(?=["#])/g ;
  //请无视这丑陋的变量名。= =
  var i = 0 ;
  
  //第一个函数用于获取指定链接的 HTML 字符串
function getHtml(u) {
    //当开始获取一个链接的HTML字符串时，说明已经开始对新的链接进行分析了，所以这里的索引值要加一
    i += 1;
    //如果这个索引值大于数组里链接的个数，说明数组里的链接都处理完了，此时退出这个函数
    if (i > aDuiLie.length) { return; }
    //在控制台显示进度以方便查看
    console.log('正在请求' + u);
    //写扩展就是好，不用考虑各种蛋疼的兼容性- -
    var request = new XMLHttpRequest() ;
    request.open('GET', u);
    request.onreadystatechange = function () {
        if (request.readyState === 4 && request.status === 200) {
            //这里的 handlerHtml 函数就是上面所说的最后一个部分的函数
            setTimeout(handlerHtml, 300, request.responseText);
            //思考题：上面的语句中为什么会延迟调用 handlerHtml 函数？
        } else if (request.readyState === 4 && request.status === 404) {
            //如果这个链接打不开，就跳过这个链接，直接进行下一个
            console.error('请求' + u + '时失败，跳过。');
            console.log('\n队列中现有' + aDuiLie.length + '个目标，下面请求第' + (i + 1) + '个。');
            //这里调用了这个函数本身，而索引 i 会在这个函数开始时加一，所以这里无需担心会重复处理这个错误的链接
            //同时我手动添加了链接的前缀：前面说过，写这个扩展我只是想抓取chrome文档。- -
            setTimeout(getHtml, 300, 'https://crxdoc-zh.appspot.com/extensions/' + aDuiLie[i]);
        }
    };
    request.send(null);
}

//将获取到的 html 字符串发送至接口
//参数 h 是上一个函数获取到的 HTML 字符串，参数 n 是文件名
function sendHtml(t, n) {
    var r = new XMLHttpRequest();
    //这个地址是我在自己的电脑上搭建的网站的地址，本文开头进行过说明
    r.open('POST', 'http://localhost:12345/savehtml/savehtml.ashx');
    r.onreadystatechange = function () {
        //如果保存失败（保存成功会返回字符串 1），则进行提示
        if (r.readyState === 4 && r.responseText !== '1') { console.error('服务器端保存' + n + '文件失败。'); }
    };
    r.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
    //当没有文件名时（也就是在点击扩展按钮的那个页面是没有文件名的，因为我是从主页开始抓取，所以此时这里的文件名是 index.html）
    r.send('h=' + encodeURIComponent(t) + '&n=' + (n || 'index.html'));
}

//我们需要最后一个函数 handlerHtml 来分析链接并将上面两个函数“串”起来：
function handlerHtml(targetHtml) {
    //首先将内容发送至服务器保存（思考题：这里的索引为什么要减一？ ^ ^）
    sendHtml(targetHtml, aDuiLie[i - 1]);  //若你没有本文开头提到的“接口”，可以注释掉这一行，仅看一看控制台显示的内容
    //分析出所有的链接
    var re = targetHtml.match(aP) , s ;
    for (var x = re.length , y = 0; y < x; y += 1) {
        //因为分析出的链接是  <a href="getstarted.html  这样的形式，所以首先得截取出真正需要的部分（也就是文件名）
        s = re[y].slice(9);
        //如果队列数组里没有这个文件名，就把它加入数组，并在控制台给出提示
        aDuiLie.some(function (z) { return z === s; }) || (aDuiLie.push(s), console.warn(s + '加入了队列。'));
        //如果这一次循环是最后一次，则开始对队列数组里的下一个链接进行处理
        if (y + 1 === re.length) {
            console.log('\n队列中现有' + aDuiLie.length + '个目标，下面请求第' + (i + 1) + '个。');
            getHtml('https://crxdoc-zh.appspot.com/extensions/' + aDuiLie[i]);
        }
    }
}

//这里我加入了文档类型声明，让接口能生成一个完整的网页文件
handlerHtml('<!DOCTYPE html><html>' + document.getElementsByTagName('html')[0].innerHTML + '</html>');

