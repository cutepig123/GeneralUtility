/**/
function Chess(){
	var order=0,nameBak,row=18,col=18,minL;//�����˳��
	/*��ʼ����*/
	function start(){
		if("turnover"==nameBak){moreMan([3+"_"+3,4+"_"+3,4+"_"+4,3+"_"+4]);
		}else if("four"==nameBak){man(minL,minL,nameBak+"_"+row/2+"_"+0);
		}else if("gogame"==nameBak){moreMan([3+"_"+3,15+"_"+3,15+"_"+15,3+"_"+15]);
		}
	}
	/*������*/
	this.board=function(name,width,height,output){
		nameBak=name;
		if("turnover"==name){row=8;col=8;}else {row=18;col=18;}
		var aW=Math.floor(width/(col+2)),aH=Math.floor(height/(row+2));
		minL=(aW>aH?aH:aW)-4;//�����������Ҫ���������ʱ��ѱ��Ŵ�
		var array=new Array("<div style=\"margin:"+minL+"px;\"><table  width=\""+(minL*col)+"\" height=\""+(minL*row)+"\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\"><tr><td width=\""+(minL*(col-1))+"\"><table border=1 cellspacing=0 cellpadding=\"0\">");
		for(var i=0;i<row;i++){
			array.push("<tr>");
			for(var j=0;j<col;j++){array.push("<td align=center>"+evt(i,j,minL,minL,minL/2*-1-4,minL/2*-1-4)+"</td>");}
			array.push("</tr>");
		}
		array.push("</table></td><td valign=\"top\">");
		if(nameBak!="four"&&nameBak!="turnover"){for(var j=0;j<col;j++){array.push(evt(row,j,minL,minL+2,minL/2*-1-2,minL/2*-1-4));}}
		array.push("</td></tr><tr><td>");
		if(nameBak!="four"&&nameBak!="turnover"){for(var j=0;j<row;j++){array.push(evt(j,col,minL+2,minL,minL/2*-1-4,minL/2*-1-2));}}
		var rs=array.join("")+"</td><td>"+evt(row,col,minL,minL,minL/2*-1-4,minL/2*-1-4)+"</td></tr></table></div>";
		if(output==null)document.write(rs);else output.innerHTML=rs;
		setClick(row,col,minL,minL);
		start();
	}
	/*������*/
	function man(width,height,id,colorBak){
		var color=colorBak==null?(order++%2==0?"000":"CCC"):colorBak;
		var r="border-radius:"+width/2+"px;";
		var obj=id==null?event.srcElement:_$(id);
		obj.innerHTML="<div id=\"man_"+color+"_"+order+"\" style=\"display:block;-webkit-"+r+"-moz-"+r+""+r+"-moz-box-shadow:inset 0 -10px 40px rgba(0,0,0,1);box-shadow:inset 0 -10px 40px rgba(0,0,0,1);background:-webkit-gradient(radial, 50 40, 30, center center, 80, from(#"+color+"), to(rgba(255,255,255,1)));width:"+width+"px;height:"+height+"px;\"></div>";
	}
	/*���ƶ������*/
	function moreMan(array){for(var i=0;i<array.length;i++)man(minL,minL,nameBak+"_"+array[i]);}
	/*�����¼�*/
	function evt(i,j,width,height,top,left){
		return "<div id=\""+nameBak+"_"+i+"_"+j+"\" style=\"float:left;position:"+(nameBak=="four"||nameBak=="turnover"?"block;":"relative;top:"+top+"px;left:"+left+"px;")+"border:0px solid #000;width:"+width+"px;height:"+height+"px;\"></div>";
	}
	/*�������*/
	function rule(){
		var id=event.srcElement.id;
		if(id.indexOf("man_")==0){alert("���������ӵĵط�����");return false;}else{
			var p=id.indexOf("_"),p1=id.lastIndexOf("_");
			var row=id.substr(p+1,p1-p-1)*1,col=id.substr(p1+1)*1;
			if("gobang"==nameBak)return gobang(row,col);
			else if("four"==nameBak){
				if(isMan(row,col+1)==null&&isMan(row,col-1)==null&&isMan(row+1,col)==null&&isMan(row-1,col)==null){alert("�����岻�������ܿհ׵ĵط����ӣ�");return false;}
				return gobang(row,col,3);
			}else if("turnover"==nameBak){
				if(isMan(row,col+1)==null&&isMan(row,col-1)==null&&isMan(row+1,col)==null&&isMan(row-1,col)==null&&isMan(row-1,col-1)==null&&isMan(row+1,col+1)==null){alert("��ת�岻�������ܿհ׵ĵط����ӣ�");return false;}
				turnover();
			}else if("gogame"==nameBak){
			}
		}
		return true;
	}
	
	/*�ж��Ƿ�������*/function isMan(row,col){var obj=_$(nameBak+"_"+row+"_"+col,1);if(obj==null||obj.indexOf("man_")==-1)return null;else if(obj.indexOf("000")!=-1)return 0;else if(obj.indexOf("CCC")!=-1)return 1;}
	/*������������������Ӯ����жϣ�������������������Ӯ��������������������Ӯ*/
	function gobang(row,col,num){
		num=num==null?4:num;
		var rs=[[],[],[],[]],b=[],w=[];/*���������ά�������洢����λ��*/
		for(var i=0,j=0;i<num*2+1;i++,j++){
			rs[0].push(isMan(row-num+i,col));rs[1].push(isMan(row,col-num+j));rs[2].push(isMan(row-num+i,col-num+j));rs[3].push(isMan(row-num+i,col-num+j));
			if(i<num){b.push(0);w.push(1);}
		}
		if(rs.join("#").indexOf(b.join(","))!=-1){alert("����ʤ");return false;}else if(rs.join("#").indexOf(w.join(","))!=-1){alert("����ʤ");return false;}
		return true;
	}
	/*Χ����Ӯ����ж�*/
	function gogame(row,col,num){
	}
	/*��ת����Ӯ����ж�*/
	function turnover(){
		if(order<64)return;
		var num=0;var total=row*col;for(var i=0;i<row;i++){for(var j=0;j<col;j++){num+=isMan(i+"_"+j);}}
		if(num<total/2)alert("����ʤ"+(total-num*2)+"��");else if(num>row*col/2)alert("����ʤ"+(num*2-total)+"��");else alert("ƽ��");
	}
	/*
		ע���������¼��ķ����ǳ����⣬ʵ��������ÿ�����̵Ľ���ĵط�����һ��div��Ȼ���div����¼�
	*/
	function setClick(row,col,width,height){
	    for(var i=0;i<=row;i++){
			for(var j=0;j<=col;j++){
				var els=_$(nameBak+"_"+i+"_"+j);
				if(els!=null)els.onclick=function(){if(rule())man(width,height);};
			}
	    }
	}
}
function _$(CHOFO,CHOF0){var CH0F0=document.getElementById(CHOFO);if(CH0F0!=null){if(CHOF0==0)return CH0F0.style;else if(CHOF0==1)return CH0F0.innerHTML;}return CH0F0;}