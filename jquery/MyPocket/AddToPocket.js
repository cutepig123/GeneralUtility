javascript:(function(){ 
function AddJSToWebPage(){
	var t=document,n=t.location.href,r=t.title;
	var s=t.createElement('script');
	s.type='text/javascript';
	s.text='AddToPocket("' + (n) + '","' + (r) + '")';
	var o=t.getElementsByTagName('head')[0]||t.documentElement;
	o.appendChild(s)
}

	function AddToPocket(href_, title_) {
  $.ajax({
	url: 'msg.php',
	data: {href: href_, title: title_},
	error: function(xhr) {
	  alert('Ajax request ∞l…˙Âe’`');
	},
	success: function(response) {
	  $('#msg').html("success! return value:<br> " + response);
	}
  });
}

})()
