(function(){ 

function AddToPocket(href_, title_)
{
var xmlhttp;
if (window.XMLHttpRequest)
  {// code for IE7+, Firefox, Chrome, Opera, Safari
  xmlhttp=new XMLHttpRequest();
  }
else
  {// code for IE6, IE5
  xmlhttp=new ActiveXObject("Microsoft.XMLHTTP");
  }
xmlhttp.onreadystatechange=function()
  {
  if (xmlhttp.readyState==4 && xmlhttp.status==200)
    {
	alert("success! return value:<br> " + xmlhttp.responseText);
    }
  }
 var url ="http://localhost/git_repos/GitHub/GeneralUtility/jquery/MyPocket/msg.php?href=" + encodeURIComponent(href_) + '&title=' + encodeURIComponent(title_);
xmlhttp.open("GET",url,true);
xmlhttp.send();
};

var t=document,n=t.location.href,r=t.title;
AddToPocket(n,r);

})();
