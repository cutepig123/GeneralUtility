var aDuiLie = [ ];
var i = 0 ;
var g_root_url='';

  
function getHtml(u2) {
   
	if( !aDuiLie.some(function (z) { return z === u2; }) )
	{
		aDuiLie.push(u2);
		console.warn(u2 + '加入了队列。');
	}
	
	while( i<aDuiLie.length )
    {
		var u=aDuiLie[i];
		i++;
		
		console.log('正在请求' + u);
		
		var request = new XMLHttpRequest() ;
		request.open('GET', u);
		request.onreadystatechange = function () {
			if (request.readyState === 4 && request.status === 200) {
				var m=[u, request.responseText];
				setTimeout(handlerHtml, 300, m);
			} else if (request.readyState === 4 && request.status === 404) {
				console.error('请求' + u + '时失败，跳过。');
			}
		};
		request.send(null);
	
	}
}

//将获取到的 html 字符串发送至接口
//参数 h 是上一个函数获取到的 HTML 字符串，参数 n 是文件名
function sendHtml(t, n) {
    var r = new XMLHttpRequest();
    r.open('POST', 'http://localhost/git_repos/GitHub/GeneralUtility/chrome_plugin/crawler/phpDownload/2.filecont.php');
    r.onreadystatechange = function () {
        if (r.readyState === 4 && r.responseText !== '1') { console.error('服务器端保存' + n + '文件失败。'); }
    };
    r.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
    
    //r.send('content=' + encodeURIComponent(t) + '&name=' + (n || 'index.html'));
	r.send('name=' + (n || 'index.html'));
}

function handlerHtml(m) {
	var u=m[0];
	var targetHtml=m[1];
	
	console.log("handlerHtml: " +u  + " Data: " + targetHtml);
	
    sendHtml(targetHtml, u); 
    
	var arrUrls =extractLinks(targetHtml, g_root_url);
    for (var x = arrUrls.length , y = 0; y < x; y += 1) {
        
        s = arrUrls[y];
        getHtml(s);
    }
}

function extractLinks(targetHtml, root_url){
	var arr = new Array();

	var regex1 =/<a([^>]+)>/ig;
	var result1;
	// extract <a>
	while(result1 = regex1.exec(targetHtml))
	{
		// extract href=
		var regex2 =/href=[\"\']?([^\s\"\']+)[\"\']?/ig;
		var result2 = regex2.exec(result1[1]);
		if(result2)
		{
			console.log("Checking: " +result2[1]);
			if( result2[1].toLowerCase().indexOf(root_url.toLowerCase()) >=0 )
			{
				console.log("Finding: " +result2[1]);
				arr.push(result2[1]);
			}
		}
	}
	
	return arr;
}

g_root_url =document.URL;
getHtml(g_root_url);

