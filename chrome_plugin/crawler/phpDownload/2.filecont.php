<?php
$url = 'http://www.qq.com';
$homepage = file_get_contents($url);
echo $homepage;
write_file($homepage, 'Data.html');

function write_file($data, $file) {
	$fp =fopen($file,'wb');
	fwrite($fp,$data);
	fclose($fp);
}

?>
