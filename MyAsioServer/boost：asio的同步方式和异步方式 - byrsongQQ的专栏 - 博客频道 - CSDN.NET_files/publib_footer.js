;(function () {
protocol = location.protocol.substr(0, 4) === 'http' ? '' : 'http:';
document.write("<link rel='stylesheet' type='text/css' href='" + protocol + "//csdnimg.cn/pubfooter/css/pub_footer_2012.css' />");
document.write("<div id='pub_footerall' class='pub_footerall'>");
document.write("<dl>");
document.write("<dt></dt>");
document.write(" <dd><a href='http://www.csdn.net/company/about.html' target='_blank'>公司简介</a>|<a href='http://www.csdn.net/company/recruit.html' target='_blank'>招贤纳士</a>|<a href='http://www.csdn.net/company/marketing.html' target='_blank'>广告服务</a>|<a href='http://www.csdn.net/company/account.html' target='_blank'>银行汇款帐号</a>|<a href='http://www.csdn.net/company/contact.html' target='_blank'>联系方式</a>|<a href='http://www.csdn.net/company/statement.html' target='_blank'>版权声明</a>|<a href='http://www.csdn.net/company/layer.html' target='_blank'>法律顾问</a>|<a href='mailto:webmaster@csdn.net'>问题报告</a>|<a target='_blank' href='http://www.csdn.net/friendlink.html'>合作伙伴</a>|<a href='http://bbs.csdn.net/forums/Service' target='_blank'>论坛反馈</a></dd>");
document.write("<dd><a href='http://wpa.qq.com/msgrd?v=3&uin=2355263776&site=qq&menu=yes' target='_blank' class='qq'>网站客服</a> <a href='http://wpa.qq.com/msgrd?v=3&uin=2251809102&site=qq&menu=yes' target='_blank' class='qq'>杂志客服</a> <a href='http://e.weibo.com/csdnsupport/profile' target='_blank' class='weibo'>微博客服</a> <a href='mailto:webmaster@csdn.net' class='email' title='联系邮箱'>webmaster@csdn.net</a> <span class='phone' title='服务热线'>400-600-2320</span></dd>")
document.write("<dd>京&nbsp;ICP&nbsp;证&nbsp;070598&nbsp;号</dd>");
document.write("<dd>北京创新乐知信息技术有限公司 版权所有</dd>");
document.write("<dd>江苏乐知网络技术有限公司 提供商务支持</dd>");
//document.write("<dd><img src='http://csdnimg.cn/pubfooter/images/ico_email.gif' alt='' title='' /> 联系邮箱：webmaster(at)csdn.net</dd>");
document.write("<dd>Copyright &copy; 1999-2014, CSDN.NET, All Rights Reserved&nbsp;<a href='http://www.hd315.gov.cn/beian/view.asp?bianhao=010202001032100010' target='_blank'><img src='http://csdnimg.cn/pubfooter/images/gongshang_logos.gif' alt='GongshangLogo' alt='' title='' /></a></dd>");
//document.write("<dd><a href='http://www.hd315.gov.cn/beian/view.asp?bianhao=010202001032100010' target='_blank'><img src='http://www.csdn.net/ui/images/gongshang_logos.gif' alt='GongshangLogo' alt='' title='' /></a></dd>");
document.write("</dl>");
document.write("</div>");
document.write('<script id="noticeScript" type="text/javascript"  btnId="header_notice_num" wrapId="note1" count="5" subcount="5" src="' + protocol + '//csdnimg.cn/rabbit/notev2/js/notify.js?9d86d94"></script>');

var head = document.getElementsByTagName('head')[0]
	, s, ss = [
		protocol + '//csdnimg.cn/pubfooter/js/tracking.js?f3107aa'
		, protocol + '//csdnimg.cn/rabbit/tracking-ad/main.js?75eacd6'
	]
	;

for(var i = 0; i < ss.length; i ++){
	s = document.createElement('script')
	s.type = 'text/javascript';
	s.charset = 'utf-8';
	s.src = ss[i];
	head.insertBefore(s, null);
}
}());