(function(){ 
	function AddToPocket(href_, title_) {
  $.ajax({
	url: 'http://localhost/git_repos/GitHub/GeneralUtility/jquery/MyPocket/msg.php',
	data: {href: href_, title: title_},
	error: function(xhr) {
	  alert('Ajax request ∞l…˙Âe’`');
	},
	success: function(response) {
	  $('#msg').html("success! return value:<br> " + response);
	}
  });
};

var t=document,n=t.location.href,r=t.title;
AddToPocket(n,r);

})();
