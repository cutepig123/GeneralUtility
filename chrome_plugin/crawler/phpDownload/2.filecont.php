<?php

if(!isset($_POST['name']))
{
	echo 'name is not set!' ;
	die();
}	
	
$name =$_POST['name'];
$url = $name;

//$_POST['content']


function write_file($data, $file) {
	$fp =fopen($file,'ab');
	fwrite($fp,$data);
	fclose($fp);
}
//echo 'start: ';

if (false)
{
	$homepage = file_get_contents($url);
	//echo $homepage;
	write_file($url . $homepage . "\n\n", 'Data.log');
}
else	
{
	//echo 'name: '.  $name;
	write_file($name."\n", 'Data.txt');
}
echo '1';

?>
