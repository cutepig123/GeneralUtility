// JavaScript Document
var main=[["��ҳ","index.htm","0,1,2,4,5,6,8"],["���&����","emenu.htm","0,1,2,3,4,5,8,10"],["Ӳ��","printer.htm","9,0,1,4"],["�ƶ�Ӧ��","emenu.htm","0,5,6,7"],["����&����","oem.htm","11,12,13,14"],["��������","self.php","15,16,17,18,19"]];
var submenu=[["���Ӳ���","emenu.htm"],["���紥����","touch.htm"],["�������","reapast.htm"],["�ͷ����","hotel.htm"],["��˱�","order.htm"],["�ֻ����","phone.htm"],["�ֻ��鱨��","report.htm"],["������Ϸ","chess.htm"],["���ӵ��","tv.htm"],["�����ع���ӡ��","printer.htm"],["���ϵ��","online.htm"],["��������","it.htm"],["OEM����","oem.htm"],["��˾���","company.htm"],["�ٷ�΢��","http://weibo.com/u/2642756931"],["������","self.php"],["����","ordergoods.php"],["��ݲ�ѯ","bill.php"],["������ѯ","self.php"],["��������","self.php"]];
var foot=[["��˾���","company.htm"],["��ϵ����","link.htm"],["��̳BBS","http://www.chofo.com/bbs"]];
var mIndex=getParameter("main")==null?0:getParameter("main");
var head_t="<div style=\"height:100px;background-image:url(img/top.gif);background-size:100% 100%;\">"+
"<table border=\"0\" cellspacing=\"0\" cellpadding=\"0\" width=\"100%\" style=\"color:#2F160F;\">"+
"  <tr>"+
"    <td>"+$H.main()+"</td>"+
"    <td rowspan=\"2\" width=\"230\"><div style=\"font-size:11px;text-align:center\">�����绰��010-80547623 59434987</div><img src=\"img/logo.gif\" /><div style=\"margin:5px;font-size:9px\">������������ܷ�,�ܷ������ܵ�����</div></td>"+
"  </tr>"+
"  <tr>"+
"    <td><div class=\"submenu\"><div class=\"submenu"+mIndex+"\" id=\"submenu\">"+$H.sub(mIndex*1)+"</div></div></td>"+
"  </tr>"+
"</table>"+
"</div>";
var foot_t="<div style=\"height:35px; background-color:#2B120B;padding-top:20px;\" align=\"center\"><div style=\"width:695px;background-image:url(img/split.jpg);color:#735F58;font-size:13px\" align=\"center\">��ӭ�����ܷ�����</div></div>"+
"<div style=\"background-color:#1E0F0A;color:#483934\">"+
"<table border=\"0\" cellspacing=\"0\" cellpadding=\"0\" width=\"100%\" style=\"color:#483934;font-weight:bold;font-size:13px\" align=\"center\">"+
"  <tr>"+
"    <td height=\"20px\"><div style=\"text-align:center\">"+Foot()+"</div></td>"+
"  </tr>"+
"  <tr>"+
"    <td><div style=\"text-align:center\"><img src=\"img/biaoshi.gif\" align=\"absmiddle\"  border=\"0\"/> 2012 (C) Copyright�ܷ��Ƽ�, All right reserved ��ICP��05023056��</div></td>"+
"  </tr>"+
"</table>"+
"</div>";

var head_t2="<div style=\"height:120px;background-color:#FFF;\">"+
'<table width="100%" height="120" cellspacing="0" cellpadding="0" border="0"><tr><td width="160" background="img/topleft1.gif" rowspan="2"></td><td height=40 valign=bottom>'+$H.main()+'</td><td width="427" background="img/topright1.gif" rowspan="2"><img src=\"img/logo.gif\" style=\"padding-left:130px;padding-top:30px;\"/><div style=\"padding-left:130px;margin:5px;font-size:9px\">������������ܷ�,�ܷ������ܵ�����</div></td></tr>'+
"  <tr>"+
"    <td><div class=\"submenu\"><div class=\"submenu"+mIndex+"\" id=\"submenu\">"+$H.sub(mIndex*1)+"</div></div></td>"+
"  </tr>"+
"</table>"+
"</div>";
var foot_t2="<div style=\"height:35px; background-color:#2B120B;padding-top:20px;\" align=\"center\"><div style=\"width:695px;background-image:url(img/split.jpg);color:#735F58;font-size:13px\" align=\"center\">��ӭ�����ܷ�����</div></div>"+
"<div style=\"background-color:#1E0F0A;color:#483934\">"+
"<table border=\"0\" cellspacing=\"0\" cellpadding=\"0\" width=\"100%\" style=\"color:#483934;font-weight:bold;font-size:13px\" align=\"center\">"+
"  <tr>"+
"    <td height=\"20px\"><div style=\"text-align:center\">"+Foot()+"</div></td>"+
"  </tr>"+
"  <tr>"+
"    <td><div style=\"text-align:center\"><img src=\"img/biaoshi.gif\" align=\"absmiddle\"  border=\"0\"/> 2012 (C) Copyright�ܷ��Ƽ�, All right reserved ��ICP��05023056��</div></td>"+
"  </tr>"+
"</table>"+
"</div>";
var qq="";
