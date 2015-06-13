// JavaScript Document
/*页头处理*/
var _bdhmProtocol = (("https:" == document.location.protocol) ? " https://" : " http://");
document.write(unescape("%3Cscript src='" + _bdhmProtocol + "hm.baidu.com/h.js%3F9029e8997581c32cccf0f8a005769881' type='text/javascript'%3E%3C/script%3E"));
function Head(){
	this.main=function(){
		var a=["<div style=\"overflow:hidden;height:29px\">"];
		for(var i=0;i<main.length;i++){a.push("<span class=\"main\""+(main[i][2]!=null?" onmouseover=\"$H.sub("+i+",'submenu')\" ontouchstart=\"$H.sub("+i+",'submenu')\"":"")+"><a href=\"javascript:window.location='"+main[i][1]+"?main="+i+"'\">"+main[i][0]+"</a></span>");}
		a.push("</div>");
		return a.join("");
	};
	/*子菜单*/
	this.sub=function(j,output){
		var subs=main[j][2].split(",");
		var a=[];
		for(var i=0;i<subs.length;i++){a.push("<span class=\"sub\"><a href=\"javascript:window.location='"+submenu[subs[i]][1]+"?main="+j+"'\">"+submenu[subs[i]][0]+"</a></span>");}
		if(output==null)return a.join(" | ");
		else{ 
			_$(output).innerHTML=a.join(" | ");
			_$(output).className="submenu"+j;
		}
	};
	this.show=function(template,j){
		var p1=template.indexOf("<主菜单>");
		var p2=template.indexOf("<子菜单>");
		if(p1==-1||p2==-1)return template;
		return template.sub(0,p1)+main()+template.sub(p1+5,p2)+sub(j)+template.sub(p2+5);
	};
}
var $H=new Head();
/*页脚处理*/
function Foot(){
	var a=[];
	for(var i=0;i<foot.length;i++){a.push("<span class=\"foot\"><a href=\"javascript:window.location='"+foot[i][1]+"'\">"+foot[i][0]+"</a></span>");}
	return a.join(" | ");
}
/*播放flv视频*/
function Flv(url,width,height){
	var p=url.indexOf("http:");
document.write("<object id=\"presentation\" width=\""+width+"\" height=\""+height+"\" classid=\"clsid:d27cdb6e-ae6d-11cf-96b8-444553540000\" codebase=\""+(p==-1?"":"http://download.macromedia.com/pub/shockwave/cabs/flash/")+"swflash.cab#version=8,0,0,0\" align=\"middle\" >"+
"			<param name=\"allowScriptAccess\" value=\"sameDomain\" />"+
"			<param name=\"movie\" value=\""+(p==-1?"flvplayer.swf?vcastr_file="+url+".flv":url)+"\" />"+
"			<param name=\"quality\" value=\"high\" />"+
"			<param name=\"bgcolor\" value=\"#000000\" />"+
"			<param name=\"allowFullScreen\" value=\"true\" />"+
"			<embed src=\""+(p==-1?"flvplayer.swf?vcastr_file="+url+".flv":url)+"\" type=\"application/x-shockwave-flash\" allowscriptaccess=\"never\" allowfullscreen=\"true\" width=\""+width+"\" height=\""+height+"\"></embed>"+
"		</object>");
}
/*文章标题列表，会自动加上省略号和时间，并对齐*/
function Titles(){
}
function Display(obj){
	if(obj==null&&_$('content_div')==null)return;
	var pc=(obj!=null?obj.parentNode:_$('content_div')).children;/*注意，这里不能采用？：方式赋值，firefox不支持*/
	for(var i=0;i<pc.length;i++){
		if(pc[i+1]==null)return;
		var style=pc[i+1].style;
		if(obj!=null){if(obj==pc[i])style.display=style.display==""?"none":"";
		}else if(pc[i].tagName=="UL"&&pc[i].onclick!=null){style.display=style.display==""?"none":"";
		}
		
	}
}
/*显示多张图片的翻页*/
function MoreImgs(id,imgs,w1,w2,h2){
	var l=imgs.length;
	var aa=new Array("<div style=\"text-align:center\"><img src=\""+imgs[0][1]+"\" id=\""+id+"_0\" width="+w1+" onmouseMove=\"this.className=window.event.offsetX>=this.clientWidth/2?'arrR':'arrL';\" onclick=\"var lang=window.event.offsetX>=this.clientWidth/2?(this.lang*1+1):(this.lang-1);this.lang=lang<0?0:(lang=="+l+"?"+(l-1)+":lang);_$('"+id+"_small_'+this.lang).onclick();\" lang=0></div><div id=\""+id+"_1\" style=\"text-align:center\">系列图"+(1)+" "+imgs[0][0]+"</div><div style=\"text-align:center\">");
	for(var i=0;i<imgs.length;i++){
		aa.push("<img id=\""+id+"_small_"+i+"\" src=\""+imgs[i][1]+"\" width="+w2+" height="+h2+" onclick=\"_$('"+id+"_0').src='"+imgs[i][1]+"';_$('"+id+"_1').innerHTML='系列图"+(i+1)+" "+imgs[i][0]+"';\">");
	}
	aa.push("</div>");
	return aa.join("");
}
var prov=["北京市","天津市","河北省","山西省","辽宁省","吉林省","上海市","江苏省","浙江省","安徽省","福建省","江西省","山东省","河南省","内蒙古自治区","黑龙江省","湖北省","湖南省","广东省","广西壮族自治区","海南省","四川省","重庆市","台湾省","贵州省","云南省","西藏自治区","陕西省","甘肃省","青海省","宁夏回族自治区","新疆维吾尔自治区","香港特别行政区","澳门特别行政区","海外"];
function Area(){
}
/*针对表单的处理类*/
function Form(){
	/*表单提交前验证*/
	this.onsubmit=function(form,str){
		if($F.notNull(form,str)&&$F.isNumber(form))return confirm('您确定填写的内容正确无误吗？点击确定提交，或者点击取消修改信息！');
		else return false;
	};
	/*不能为空的项*/
	this.notNull=function(form,str){
		    for(var i=0;i<form.elements.length;i++){
				var name=form.elements[i].name;
				if(form.elements[i].value==""&&(str==null||(","+str+",").indexOf(","+name+",")!=-1)){
					alert(form.elements[i].title+"是必填项，不能为空！");
					form.elements[i].focus();
					return false;
				}
			}
			return true;
	};
	this.isNumber=function(form){
		    for(var i=0;i<form.elements.length;i++){
//				alert(form.elements[i].lang);
				if(form.elements[i].lang=="number"&&!(/^[0-9\.]+$/ig.test(form.elements[i].value))){
					alert(form.elements[i].title+"只能填数字！");
					form.elements[i].focus();
					return false;
				}
			}
			return true;
	};
}
var $F=new Form();
/*数据处理类*/
function Data(){
	this.toTable=function(array){
		return "<table width=100% border=1><thead align=center style=\"font-weight:bold\"><tr><td>"+array[0].join("</td><td>")+"</td></tr></thead><tbody align=center><tr><td>"+replaceAll(array.slice(1).join("</td></tr><tr><td>"),",","</td><td>")+"</td></tr></tbody></table>";
	};
}
var $D=new Data();
