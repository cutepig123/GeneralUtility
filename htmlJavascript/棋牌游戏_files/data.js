// JavaScript Document
var main=[["首页","index.htm","0,1,2,4,5,6,8"],["软件&方案","emenu.htm","0,1,2,3,4,5,8,10"],["硬件","printer.htm","9,0,1,4"],["移动应用","emenu.htm","0,5,6,7"],["招商&合作","oem.htm","11,12,13,14"],["个人中心","self.php","15,16,17,18,19"]];
var submenu=[["电子菜谱","emenu.htm"],["网络触摸屏","touch.htm"],["餐饮软件","reapast.htm"],["客房软件","hotel.htm"],["点菜宝","order.htm"],["手机点餐","phone.htm"],["手机查报表","report.htm"],["无线游戏","chess.htm"],["电视点餐","tv.htm"],["餐饮特供打印机","printer.htm"],["网上点餐","online.htm"],["技术优势","it.htm"],["OEM合作","oem.htm"],["公司简介","company.htm"],["官方微博","http://weibo.com/u/2642756931"],["做方案","self.php"],["订货","ordergoods.php"],["快递查询","bill.php"],["技术咨询","self.php"],["在线试用","self.php"]];
var foot=[["公司简介","company.htm"],["联系我们","link.htm"],["论坛BBS","http://www.chofo.com/bbs"]];
var mIndex=getParameter("main")==null?0:getParameter("main");
var head_t="<div style=\"height:100px;background-image:url(img/top.gif);background-size:100% 100%;\">"+
"<table border=\"0\" cellspacing=\"0\" cellpadding=\"0\" width=\"100%\" style=\"color:#2F160F;\">"+
"  <tr>"+
"    <td>"+$H.main()+"</td>"+
"    <td rowspan=\"2\" width=\"230\"><div style=\"font-size:11px;text-align:center\">订货电话：010-80547623 59434987</div><img src=\"img/logo.gif\" /><div style=\"margin:5px;font-size:9px\">上天既命侯于周服,周服就是周到服务</div></td>"+
"  </tr>"+
"  <tr>"+
"    <td><div class=\"submenu\"><div class=\"submenu"+mIndex+"\" id=\"submenu\">"+$H.sub(mIndex*1)+"</div></div></td>"+
"  </tr>"+
"</table>"+
"</div>";
var foot_t="<div style=\"height:35px; background-color:#2B120B;padding-top:20px;\" align=\"center\"><div style=\"width:695px;background-image:url(img/split.jpg);color:#735F58;font-size:13px\" align=\"center\">欢迎光临周服官网</div></div>"+
"<div style=\"background-color:#1E0F0A;color:#483934\">"+
"<table border=\"0\" cellspacing=\"0\" cellpadding=\"0\" width=\"100%\" style=\"color:#483934;font-weight:bold;font-size:13px\" align=\"center\">"+
"  <tr>"+
"    <td height=\"20px\"><div style=\"text-align:center\">"+Foot()+"</div></td>"+
"  </tr>"+
"  <tr>"+
"    <td><div style=\"text-align:center\"><img src=\"img/biaoshi.gif\" align=\"absmiddle\"  border=\"0\"/> 2012 (C) Copyright周服科技, All right reserved 京ICP备05023056号</div></td>"+
"  </tr>"+
"</table>"+
"</div>";

var head_t2="<div style=\"height:120px;background-color:#FFF;\">"+
'<table width="100%" height="120" cellspacing="0" cellpadding="0" border="0"><tr><td width="160" background="img/topleft1.gif" rowspan="2"></td><td height=40 valign=bottom>'+$H.main()+'</td><td width="427" background="img/topright1.gif" rowspan="2"><img src=\"img/logo.gif\" style=\"padding-left:130px;padding-top:30px;\"/><div style=\"padding-left:130px;margin:5px;font-size:9px\">上天既命侯于周服,周服就是周到服务</div></td></tr>'+
"  <tr>"+
"    <td><div class=\"submenu\"><div class=\"submenu"+mIndex+"\" id=\"submenu\">"+$H.sub(mIndex*1)+"</div></div></td>"+
"  </tr>"+
"</table>"+
"</div>";
var foot_t2="<div style=\"height:35px; background-color:#2B120B;padding-top:20px;\" align=\"center\"><div style=\"width:695px;background-image:url(img/split.jpg);color:#735F58;font-size:13px\" align=\"center\">欢迎光临周服官网</div></div>"+
"<div style=\"background-color:#1E0F0A;color:#483934\">"+
"<table border=\"0\" cellspacing=\"0\" cellpadding=\"0\" width=\"100%\" style=\"color:#483934;font-weight:bold;font-size:13px\" align=\"center\">"+
"  <tr>"+
"    <td height=\"20px\"><div style=\"text-align:center\">"+Foot()+"</div></td>"+
"  </tr>"+
"  <tr>"+
"    <td><div style=\"text-align:center\"><img src=\"img/biaoshi.gif\" align=\"absmiddle\"  border=\"0\"/> 2012 (C) Copyright周服科技, All right reserved 京ICP备05023056号</div></td>"+
"  </tr>"+
"</table>"+
"</div>";
var qq="";
