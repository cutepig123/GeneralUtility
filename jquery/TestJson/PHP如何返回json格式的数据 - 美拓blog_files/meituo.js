window.onload=function(){
	//图片的背景边框效果
	var offsetX = 20;
	var offsetY = 10;
    var maxwidth;

    contentwidth=$(".content").width();
    maxwidth = contentwidth - 10; //减去padding和border的宽度

	$(".content img:not('.no')").each(function(){
		$(this).wrap("<div />")
		         .attr("class","addimgbor")
		         .css("display","block")
		         .parent().attr("class","addcaption");
		$imgtitle=$(this).attr("title") || $(this).attr("alt");
		if($imgtitle) $(this).parent().append("<span>"+$imgtitle +"</span>");
		 //出现悬停效果的宽度
             var thiswidth=$(this).width();
		if(maxwidth<thiswidth)
		{
			thiswidth=maxwidth-10;   // 图片宽度等于最大宽度-10
			$(this).width(thiswidth);
			$(this).parent().width(maxwidth);
			//鼠标悬停的放大效果
			var $imgsrc=$(this).attr("src");
			$(this).hover(function(e){
				$('<img id="largeImage" src="' + $imgsrc + '" alt="big image" />')
				.css('top',e.pageY +offsetY)
				.css('left', e.pageX + offsetX)
				.appendTo('body');
			}, function() {
				$("#largeImage").remove();
			});

			$(this).mousemove(function(e) {
				$("#largeImage").css('top', e.pageY + offsetY).css('left', e.pageX + offsetX);
			});
		}

	});
}

/* mtAnnounce for ie6 */
if($.browser.msie&&($.browser.version == "6.0")&&!$.support.style  ){
	$('body').mtAnnounce({
		className : 'notice',
		text : '<h2>你的浏览器是IE6，请升级或者更换你的浏览器</h2>'
	})
}

// 头部导航
$('#nav .cat-item').hover(function(){
	$(this).children('ul').width($(this).width()).show();
},function(){
	$(this).children('ul').hide();
});

//top animate
var $top = $('#top'),
    topHeight = $top.height(),
    textHeight = $top.children('.w970').height(),
	$sidebar = $('#sidebar'),
 	$main = $('#main'),
 	mainw =$main.width(),
 	$nav = $('#nav');
if(textHeight>topHeight){
	var dcolor = $('.description').css('color');
	$top.hover(function(){
		header_timeout = setTimeout('headershow()',500);
	},function(){
		if(header_timeout)clearTimeout(header_timeout);
		$(this).animate({height:topHeight},100);
		$('.description').css('color',dcolor);
	});
	
	function headershow(){
		$top.animate({height:textHeight},200);
		$('.description').css('color','#eaeaea');
	}
}

//sidebar 

if($sidebar.size()){
	//slide
	$sidebar.delegate('h2','click',function(){
		$(this).next('ul').slideToggle();
	});	
	//toggle
	var toggle_sidebar ='<div class="toggle_sidebar">'+
					'	<a href="javascript:;">'+
					'	隐藏侧边栏>>'+
					'	</a>'+
					'</div>';
	$sidebar.parent().removeClass(' mainwrap');				
	$(toggle_sidebar).insertAfter($nav).find('a').click(function(){
		var sidebarv = $sidebar.is(':visible');	
		if(sidebarv){
			$sidebar.hide();
			$main.width('100%');
			$(this).text('显示侧边栏<<');
		} else{
			$sidebar.show();
			$main.width(mainw);
			$(this).text('隐藏侧边栏>>');
		}
	});
}					

//share
function getParamsOfShareWindow(width, height) {
	return ['toolbar=0,status=0,resizable=1,width=' + width + ',height=' + height + ',left=',(screen.width-width)/2,',top=',(screen.height-height)/2].join('');
}

function bindShareList() {
	var link = encodeURIComponent(document.location); // 文章链接
	var title = encodeURIComponent(document.title.substring(0,76)); // 文章标题
	var source = encodeURIComponent('美拓blog'); // 网站名称
	var windowName = 'share'; // 子窗口别称
	var site = 'http://blog.meituo.net/'; // 网站链接
 
	jQuery('#facebook-share').click(function() {
		var url = 'http://facebook.com/share.php?u=' + link + '&t=' + title;
		var params = getParamsOfShareWindow(626, 436);
		window.open(url, windowName, params);
	});
 
	jQuery('#twitter-share').click(function() {
		var url = 'http://twitter.com/share?url=' + link + '&text=' + title;
		var params = getParamsOfShareWindow(500, 375);
		window.open(url, windowName, params);
	});
 
	jQuery('#delicious-share').click(function() {
		var url = 'http://delicious.com/post?url=' + link + '&title=' + title;
		var params = getParamsOfShareWindow(550, 550);
		window.open(url, windowName, params);
	});
 
	jQuery('#kaixin001-share').click(function() {
		var url = 'http://www.kaixin001.com/repaste/share.php?rurl=' + link + '&rcontent=' + link + '&rtitle=' + title;
		var params = getParamsOfShareWindow(540, 342);
		window.open(url, windowName, params);
	});
 
	jQuery('#renren-share').click(function() {
		var url = 'http://share.renren.com/share/buttonshare?link=' + link + '&title=' + title;
		var params = getParamsOfShareWindow(626, 436);
		window.open(url, windowName, params);
	});
 
	jQuery('#douban-share').click(function() {
		var url = 'http://www.douban.com/recommend/?url=' + link + '&title=' + title;
		var params = getParamsOfShareWindow(450, 350);
		window.open(url, windowName, params);
	});
 
	jQuery('#sina-share').click(function() {
		var url = 'http://v.t.sina.com.cn/share/share.php?url=' + link + '&title=' + title;
		var params = getParamsOfShareWindow(607, 523);
		window.open(url, windowName, params);
	});
 
	jQuery('#netease-share').click(function() {
		var url = 'http://t.163.com/article/user/checkLogin.do?link=' + link + 'source=' + source + '&info='+ title + ' ' + link;
		var params = getParamsOfShareWindow(642, 468);
		window.open(url, windowName, params);
	});
 
	jQuery('#tencent-share').click(function() {
		var url = 'http://v.t.qq.com/share/share.php?title=' + title + '&url=' + link + '&site=' + site;
		var params = getParamsOfShareWindow(634, 668);
		window.open(url, windowName, params);
	});
}
 
bindShareList();

