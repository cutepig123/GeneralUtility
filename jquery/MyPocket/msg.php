<html xmlns="http://www.w3.org/1999/xhtml">
<head>
    <title>Hello World jQuery!</title>
    <script src="http://ajax.googleapis.com/ajax/libs/jquery/1.8.3/jquery.min.js"></script>
</head>

<?php
  $href =$_GET['href'];
  $title = $_GET['title'];
  echo $href . $title;
?>

<body>
<script type="text/javascript" >
	//var t=document,n=t.location.href;
	//alert(n);
	alert('(msg.php): <?php  echo $href . $title; ?>');
</script>
</body>
